#define _USE_MATH_DEFINES
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <imgui/imgui.h>

#include <nfd.h>

#include <stdio.h>
#include <iostream>
#include <memory>
#include <random>
#include <string>
#include <sstream>
#include <stdlib.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Renderer.h"
#include "Scene.h"
#include "Camera.h"
#include "ImguiMenus.h"
#include "AmbientLight.h"
#include "PointLight.h"
#include "Utils.h"


double zoomFactor = 1;
int windowWidth = 1280;
int windowHeight = 720;
char* windowTitle = "OpenGL Demo";
glm::vec4 clearColor = glm::vec4(0.8f, 0.8f, 0.8f, 1.00f);
bool zoomChanged = false;
std::shared_ptr<Scene> scene;

ImGuiIO* imgui;
GLFWwindow* window;

GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name);
ImGuiIO& SetupImgui(GLFWwindow* window);
bool Setup(int windowWidth, int windowHeight, const char* windowName);
void Cleanup();

static void GlfwErrorCallback(int error, const char* description);
//void RenderFrame(GLFWwindow* window, std::shared_ptr<Scene> scene, Renderer& renderer, ImGuiIO& io);

void glfw_OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset);
void glfw_OnFramebufferSize(GLFWwindow* window, int width, int height);

float GetAspectRatio();
void DrawImguiMenus();
void HandleImguiInput();

int main(int argc, char** argv)
{

	if (!Setup(windowWidth, windowHeight, windowTitle))
	{
		std::cerr << "Setup failed" << std::endl;
		return -1;
	}

	scene = std::make_shared<Scene>();
	glm::vec3 eye = glm::vec3(0, 0, 10);
	glm::vec3 at = glm::vec3(0, 0, 0);
	glm::vec3 up = glm::vec3(0, 1, 0);
	Camera camera = Camera(eye, at, up, GetAspectRatio());
	scene->AddCamera(camera);

	scene->AddLight(std::make_shared<PointLight>(glm::vec3(0, 0, 15), glm::vec3(1, 1, 1)));
	scene->AddLight(std::make_shared<PointLight>(glm::vec3(0, 5, 5), glm::vec3(0, 0, 0)));
	scene->AddLight(std::make_shared<PointLight>(glm::vec3(-5, 0, 0), glm::vec3(0, 0, 0)));

	Renderer renderer;
	renderer.LoadShaders();
	renderer.LoadTextures();

	while (!glfwWindowShouldClose(window))
	{
		// Poll and process events
		glfwPollEvents();

		// Imgui stuff
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		DrawImguiMenus();
		ImGui::Render();
		HandleImguiInput();

		// Clear the screen and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render scene
		renderer.Render(scene);

		// Imgui stuff
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap front and back buffers
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}

static void GlfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

bool Setup(int windowWidth, int windowHeight, const char* windowName)
{
	GLFWwindow* window = SetupGlfwWindow(windowWidth, windowHeight, windowName);
	if (!window)
	{
		std::cerr << "Window setup failed" << std::endl;
		return false;
	}

	imgui = &SetupImgui(window);

	glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
	glEnable(GL_DEPTH_TEST);

	return true;
}

GLFWwindow* SetupGlfwWindow(int windowWidth, int windowHeight, const char* windowName)
{
	// Intialize GLFW
	if (!glfwInit())
	{
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// forward compatible with newer versions of OpenGL as they become available but not backward compatible (it will not run on devices that do not support OpenGL 3.3
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create an OpenGL 3.3 core, forward compatible context window
	window = glfwCreateWindow(windowWidth, windowHeight, windowName, NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	// Make the window's context the current one
	glfwMakeContextCurrent(window);

	// Setup window events callbacks
	glfwSetFramebufferSizeCallback(window, glfw_OnFramebufferSize);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		// An error occured
		std::cerr << "GLAD initialization failed" << std::endl;
		return false;
	}

	return window;
}

ImGuiIO& SetupImgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	// Setup style
	ImGui::StyleColorsDark();

	glfwSetScrollCallback(window, glfw_OnMouseScroll);

	return io;
}

void HandleImguiInput()
{
	if (!imgui->WantCaptureKeyboard)
	{
		if (imgui->KeysDown[49]) // 1
		{
			scene->GetActiveModel()->RotateXModel(M_PI / 200);
		}

		if (imgui->KeysDown[50]) // 2
		{
			scene->GetActiveModel()->RotateXModel(-M_PI / 200);
		}

		if (imgui->KeysDown[51]) // 3
		{
			scene->GetActiveModel()->RotateYModel(M_PI / 200);
		}

		if (imgui->KeysDown[52]) // 4
		{
			scene->GetActiveModel()->RotateYModel(-M_PI / 200);
		}

		if (imgui->KeysDown[53]) // 5
		{
			scene->GetActiveModel()->RotateZModel(M_PI / 200);
		}

		if (imgui->KeysDown[54]) // 6
		{
			scene->GetActiveModel()->RotateZModel(-M_PI / 200);
		}

		if (imgui->KeysDown[45]) // -
		{
			scene->GetActiveModel()->ScaleModel(1 / 1.01);
		}

		if (imgui->KeysDown[61]) // +
		{
			scene->GetActiveModel()->ScaleModel(1.01);
		}

		if (imgui->KeysDown[65]) // a
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(-0.02, 0, 0));
		}
		if (imgui->KeysDown['R']) // r
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(0, 0.02, 0));
		}
		if (imgui->KeysDown['T']) // t
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(0, -0.02, 0));
		}

		if (imgui->KeysDown[68]) // d
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(0.02, 0, 0));
		}

		if (imgui->KeysDown[83]) // s
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(0, 0, 0.02));
		}

		if (imgui->KeysDown[87]) // w
		{
			scene->GetActiveModel()->TranslateModel(glm::vec3(0, 0, -0.02));
		}
	}

	if (!imgui->WantCaptureMouse)
	{
		if (zoomChanged)
		{
			scene->GetActiveCamera().Zoom(zoomFactor);
			zoomChanged = false;
		}
	}
}

void Cleanup()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

//-----------------------------------------------------------------------------
// Is called when the window is resized
//-----------------------------------------------------------------------------
void glfw_OnFramebufferSize(GLFWwindow* window, int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	glViewport(0, 0, windowWidth, windowHeight);
	scene->GetActiveCamera().SetAspectRatio(GetAspectRatio());
}

void glfw_OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	zoomFactor = glm::pow(1.1, -yoffset);
	zoomChanged = true;
}

float GetAspectRatio()
{
	return static_cast<float>(windowWidth) / static_cast<float>(windowHeight);
}

void DrawImguiMenus()
{
	static bool model_transformation = false, model_controllers = false, camera_transformation = false, camera_controllers = false, addCamera = false, light_controllers = false;
	ImGui::SetNextWindowPos(ImVec2(0, 20), ImGuiCond_Once);
	{
		ImGui::Begin("Scene Menu");
		if (ImGui::ColorEdit3("Clear Color", (float*)&clearColor))
		{
			glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		}

		if (ImGui::CollapsingHeader("Cameras"))
		{
			if (ImGui::Button("Add Camera"))
			{

				std::random_device rd;
				std::mt19937 mt(rd());
				std::uniform_real_distribution<double> dist(0, 2 * M_PI);
				std::uniform_real_distribution<double> dist2(2, 10);
				double angle = dist(mt);
				double radius = dist2(mt);

				glm::vec4 eye = glm::vec4(radius * glm::cos(angle), 0, radius * glm::sin(angle), 1);
				glm::vec4 at = glm::vec4(0, 0, 0, 1);
				glm::vec4 up = glm::vec4(0, 1, 0, 1);
				scene->AddCamera(Camera(-scene->GetActiveCamera().GetEye(), at, up, GetAspectRatio()));
			}

			const char** items;
			std::vector<std::string> cameraStrings;
			items = new const char* [scene->GetCameraCount()];
			for (int i = 0; i < scene->GetCameraCount(); i++)
			{
				std::ostringstream s;
				s << "Camera " << i;
				std::string mystring = s.str();
				cameraStrings.push_back(mystring);
			}

			for (int i = 0; i < scene->GetCameraCount(); i++)
			{
				items[i] = cameraStrings[i].c_str();
			}

			int currentCamera = scene->GetActiveCameraIndex();
			ImGui::Combo("Active Camera", &currentCamera, items, scene->GetCameraCount());

			if (currentCamera != scene->GetActiveCameraIndex())
			{
				scene->SetActiveCameraIndex(currentCamera);
				scene->GetActiveCamera().SetAspectRatio(GetAspectRatio());
			}

			delete items;

			int newProjectionType = scene->GetActiveCamera().IsPrespective() ? 0 : 1;
			ImGui::RadioButton("Prespective", &newProjectionType, 0);
			ImGui::RadioButton("Orthographic", &newProjectionType, 1);

			if (newProjectionType == 0)
			{
				float fovy;
				float zNear;
				float zFar;

				scene->GetActiveCamera().SwitchToPrespective();

				if (ImGui::SliderFloat("Fovy", &fovy, 0.0f, M_PI))
				{
					scene->GetActiveCamera().SetFovy(fovy);
				}

				if (ImGui::SliderFloat("Near", &zNear, 1.0f, 10.0f))
				{
					scene->GetActiveCamera().SetNear(zNear);
				}

				if (ImGui::SliderFloat("Far", &zFar, 1.0f, 10.0f))
				{
					scene->GetActiveCamera().SetFar(zFar);
				}
			}
			else if (newProjectionType == 1)
			{
				float height;
				float zNear;
				float zFar;

				scene->GetActiveCamera().SwitchToOrthographic();

				if (ImGui::SliderFloat("Height", &height, 0.0f, M_PI))
				{
					scene->GetActiveCamera().SetHeight(height);
				}

				if (ImGui::SliderFloat("Near", &zNear, 1.0f, 10.0f))
				{
					scene->GetActiveCamera().SetNear(zNear);
				}

				if (ImGui::SliderFloat("Far", &zFar, 1.0f, 10.0f))
				{
					scene->GetActiveCamera().SetFar(zFar);
				}
			}
		}

		if (ImGui::CollapsingHeader("Models"))
		{
			const char** items;
			std::vector<std::string> modelStrings;
			items = new const char* [scene->GetModelCount()];
			for (int i = 0; i < scene->GetModelCount(); i++)
			{
				std::ostringstream s;
				s << scene->GetModel(i)->GetModelName();
				std::string mystring = s.str();
				modelStrings.push_back(mystring);
			}

			for (int i = 0; i < scene->GetModelCount(); i++)
			{
				items[i] = modelStrings[i].c_str();
			}

			int currentModelIndex = scene->GetActiveModelIndex();
			ImGui::Combo("Active Model", &currentModelIndex, items, scene->GetModelCount());

			if (currentModelIndex != scene->GetActiveModelIndex())
			{
				scene->SetActiveModelIndex(currentModelIndex);
			}

			delete  items;

			glm::vec3 modelColor = scene->GetActiveModel()->GetColor();
			if (ImGui::ColorEdit3("Model Color", (float*)&modelColor))
			{
				scene->GetActiveModel()->SetColor(modelColor);
			}

			std::shared_ptr<MeshModel> meshModel = std::dynamic_pointer_cast<MeshModel>(scene->GetActiveModel());
			if (meshModel)
			{
				//glm::vec4 normalColor = meshModel->GetNormalModel().GetColor();
				//if (ImGui::ColorEdit3("Normal Color", (float*)&normalColor))
				//{
				//	meshModel->GetNormalModel().SetColor(normalColor);
				//}
			}
		}

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}


	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoFocusOnAppearing;
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Open", "CTRL+O"))
				{
					nfdchar_t* outPath = NULL;
					nfdresult_t result = NFD_OpenDialog("obj;png,jpg", NULL, &outPath);
					if (result == NFD_OKAY) {
						scene->AddModel(Utils::LoadMeshModel(outPath));
						free(outPath);
					}
					else if (result == NFD_CANCEL) {
					}
					else {
					}

				}
				ImGui::EndMenu();
			}
			
			if (ImGui::MenuItem("model transformation"))
						{
							model_transformation = true;
						}
				
						if (ImGui::MenuItem("model controllers"))
						{
							model_controllers = true;
						}
				
						if (ImGui::MenuItem("camera transformation"))
						{
							camera_transformation = true;
						}
				
						if (ImGui::MenuItem("camera controllers"))
						{
							camera_controllers = true;
						}
						if (ImGui::MenuItem("Add a camera"))
						{
							addCamera = true;
						}
						if (ImGui::MenuItem("Light"))
						{
							light_controllers = true;
						}
			ImGui::EndMainMenuBar();
		}
	}
	static int counter = 0;
	const int modelCount = scene->GetModelCount();
	const int cameraCount = scene->GetCameraCount();
	static char* items[] = { "0","1","2","3","4","5","6","7" };
	static char* items2[] = { "0","1","2","3","4","5","6","7" };
	static int selectedItemModel = modelCount;
	static int selectedItemCamera = 0;
	
	//Model controls
	if (modelCount > 0)
	{
		{
			if (model_transformation)

			{
				ImGui::Begin("Change Model Position");
				ImGui::Combo("modelPicker", &selectedItemModel, items, modelCount);
				scene->SetActiveModelIndex(selectedItemModel);
				int index = scene->GetActiveModelIndex();
				MeshModel& myModel = scene->GetModel2(index);
				glm::vec3 translationObject(myModel.GetTranslationObject());
				glm::vec3 rotationObject(myModel.GetRotationObject());
				glm::vec3 scaleObject(myModel.GetScaleObject());

				glm::vec3 translationWorld(myModel.GetTranslationWorld());
				glm::vec3 rotationWorld(myModel.GetRotationWorld());
				glm::vec3 scaleWorld(myModel.GetScaleWorld());

				bool axisLocal = myModel.axisLocal;
				bool axisWorld = myModel.axisWorld;
				bool bBoxLocal = myModel.bounding_box_local;
				bool bBoxWorld = myModel.bounding_box_world;
				bool verticeNormals = myModel.drawVerticeNormals;
				bool faceNormals = myModel.drawFaceNormals;
				bool rectangles = myModel.drawRectangle;

				ImGui::Text("Local Transformation");
				ImGui::SliderFloat3("Translate-Local", &translationObject.x, -3.0f, 3.0f);
				ImGui::SliderFloat3("Rotate-Local", &rotationObject.x, 0.0f, 360.0f);
				ImGui::SliderFloat("Scale-Local", &scaleObject.x, 0.0f, 4.0f);
				scaleObject.y = scaleObject.x;
				scaleObject.z = scaleObject.x;
				myModel.SetTranslationObject(translationObject);
				myModel.SetRotationObject(rotationObject);
				myModel.SetScaleObject(scaleObject);
				myModel.SetObjectTransform();

				ImGui::Text("World Transformation");

				ImGui::SliderFloat3("Translate-World", &translationWorld.x, -3.0f, 3.0f);
				ImGui::SliderFloat3("Rotate-World", &rotationWorld.x, 0.0f, 360.0f);
				ImGui::SliderFloat("Scale-World", &scaleWorld.x, 0.0f, 4.0f);
				scaleWorld.y = scaleWorld.x;
				scaleWorld.z = scaleWorld.x;
				myModel.SetTranslationWorld(translationWorld);
				myModel.SetRotationWorld(rotationWorld);
				myModel.SetScaleWorld(scaleWorld);
				myModel.SetWorldTransform();

				if (ImGui::Button("Center"))
				{
					myModel.SetTranslationObject(glm::vec3(0.0f));
					myModel.SetRotationObject(glm::vec3(0.0f));
					myModel.SetTranslationWorld(glm::vec3(0.0f));
					myModel.SetRotationWorld(glm::vec3(0.0f));
				}

				if (ImGui::Checkbox("Axis Local", &axisLocal))
				{
					myModel.axisLocal = !myModel.axisLocal;
				}

				if (ImGui::Checkbox("Axis World", &axisWorld))
				{
					myModel.axisWorld = !myModel.axisWorld;
				}

				if (ImGui::Checkbox("B_Box Local", &bBoxLocal))
				{
					myModel.bounding_box_local = !myModel.bounding_box_local;
				}

				if (ImGui::Checkbox("B_Box World", &bBoxWorld))
				{
					myModel.bounding_box_world = !myModel.bounding_box_world;
				}
				if (ImGui::Checkbox("vertice normals", &verticeNormals))
				{
					myModel.drawVerticeNormals = !myModel.drawVerticeNormals;
				}
				if (ImGui::Checkbox("face normals", &faceNormals))
				{
					myModel.drawFaceNormals = !myModel.drawFaceNormals;
				}
				if (ImGui::Checkbox("ractangles", &rectangles))
				{
					myModel.drawRectangle = !myModel.drawRectangle;
				}
				ImGui::End();
			}
		}


		//Camera controls



		if (cameraCount > 0)
		{
			Camera& cam = scene->GetActiveCamera();
			static float aspect = float(windowWidth) / float(windowHeight);
			if (camera_controllers)
			{
				ImGui::Begin("Camera Controller");
				ImGui::Combo("Pick a Camera", &selectedItemCamera, items2, cameraCount);
				scene->SetActiveCameraIndex(selectedItemCamera);
				Camera& cam = scene->GetActiveCamera();
				if (ImGui::RadioButton("Ortho", cam.my_ortho_or_perspective)) { cam.my_ortho_or_perspective = true; }ImGui::SameLine();
				if (ImGui::RadioButton("Perspective", !cam.my_ortho_or_perspective)) { cam.my_ortho_or_perspective = false; }
				if (cam.my_ortho_or_perspective) // Ortho
				{
					ImGui::SliderFloat("Up", &cam.my_up, -10.0f, 10.0f);
					ImGui::SliderFloat("Down", &cam.my_down, -10.0f, 10.0f);
					ImGui::SliderFloat("Left", &cam.my_left, -10.0f, 10.0f);
					ImGui::SliderFloat("right", &cam.my_right, -10.0f, 10.0f);
					ImGui::SliderFloat("Near", &cam.my_zNearOrtho, -10.0f, 10.0f);
					ImGui::SliderFloat("Far", &cam.my_zFarOrtho, -10.0f, 10.0f);

					if (ImGui::Button("auto"))
					{
						cam.my_up = 1;
						cam.my_down = -1;
						cam.my_left = -1;
						cam.my_right = 1;
						cam.my_zNearOrtho = 1;
						cam.my_zFarOrtho = -1;
					}
					cam.Ortho(cam.my_left, cam.my_right, cam.my_down, cam.my_up, cam.my_zNearOrtho, cam.my_zFarOrtho);
				}
				else // Perspective
				{

					ImGui::SliderFloat("Fovy in degrees", &cam.my_fovy, 1.0f, 360.0f);
					ImGui::SliderFloat("Near", &cam.my_zNearPerspective, -10.0f, 10.0f);
					ImGui::SliderFloat("Far", &cam.my_zFarPerspective, -100.0f, 100.0f);

					if (ImGui::Button("reset"))
					{

						cam.my_fovy = 45.0f;
						cam.my_zNearPerspective = 1;
						cam.my_zFarPerspective = 100;
					}
					ImGui::SliderFloat("Dolly Zoom", &cam.dolly, -0.04f, 0.04f);
					ImGui::Checkbox("Do you want to Dolly?", &cam.my_isDolly);
					if (cam.my_isDolly)
					{
						cam.my_fovy += cam.dolly * 2.5f;
						cam.translationWorld.z += cam.dolly / 5;

					}
					ImGui::Text("Camera controls");//Look at
					ImGui::SliderFloat("eye X", &cam.my_eye.x, -10.0f, 10.0f);
					ImGui::SliderFloat("eye Y", &cam.my_eye.y, -10.0f, 10.0f);
					ImGui::SliderFloat("eye Z", &cam.my_eye.z, -10.0f, 10.0f);

					ImGui::SliderFloat("at X", &cam.my_at.x, -10.0f, 10.0f);
					ImGui::SliderFloat("at Y", &cam.my_at.y, -10.0f, 10.0f);
					ImGui::SliderFloat("at Z", &cam.my_at.z, -10.0f, 10.0f);
					if (ImGui::Button("auto"))
					{
						cam.my_eye = { 0.0f,0.0f,3.0f };
						cam.my_at = { 1.0,1.0,1.0 };
					}
					cam.Perspective(glm::radians(cam.my_fovy), aspect, cam.my_zNearPerspective, cam.my_zFarPerspective);

				}

				ImGui::End();


			}


		}
	}
	{
	
		

		//float pos_x = light_test.position.x;
		//float pos_y = light_test.position.y;
		//float pos_z = light_test.position.z;

		std::shared_ptr<PointLight> light_test = scene->GetLight(0);
		glm::vec3 position = light_test->position;
		glm::vec3 translate = light_test->translation;
		float ambient_strength = light_test->ambient_strength;
		float diffuse_strength = light_test->diffuse_strength;
		float specular_strength = light_test->specular_strength;

		glm::vec3 ambient = light_test->ambient;
		glm::vec3 diffuse = light_test->diffuse;
		glm::vec3 specular = light_test->specular;



		bool flat = light_test->flat_shading;
		bool gouraud = light_test->gouraud_shading;
		bool phong = light_test->phong_shading;
		int currentLightIndex = 0;
		if (currentLightIndex != scene->GetActiveLightIndex())
		{
			scene->SetActiveLightIndex(currentLightIndex);
		}

		if (light_controllers)
		{
			ImGui::Begin("light modification");

			ImGui::SliderFloat("move light-x", &translate.x, -3.0f, 3.0f);
			ImGui::SliderFloat("move light-y", &translate.y, -3.0f, 3.0f);
			ImGui::SliderFloat("move light-z", &translate.z, -10.0f, 10.0f);
			light_test->translation = translate;
			light_test->set_transform(light_test->translation);


			ImGui::SliderFloat("ambient strength", &ambient_strength, 0.0f, 1.0f);
			light_test->ambient_strength = ambient_strength;
			ImGui::ColorEdit3("ambient color", (float*)&ambient);
			light_test->ambient = ambient;
			ImGui::SliderFloat("diffuse strength", &diffuse_strength, 0.0f, 1.0f);
			light_test->diffuse_strength = diffuse_strength;
			ImGui::ColorEdit3("diffuse color", (float*)&diffuse);
			light_test->diffuse = diffuse;

			ImGui::SliderFloat("specular strength", &specular_strength, 0.0f, 1.0f);
			light_test->specular_strength = specular_strength;
			ImGui::ColorEdit3("specular color", (float*)&specular);
			light_test->specular = specular;
			ImGui::End();
		}
	}
}
//void DrawImguiMenus(ImGuiIO& io, Scene& scene)
//{
//
//	/**
//	 * MeshViewer menu
//	 */
//
//	static bool model_transformation = false, model_controllers = false, camera_transformation = false, camera_controllers = false, addCamera = false, light_controllers = false;
//	ImGui::Begin("MeshViewer Menu");
//
//
//	// Menu Bar
//	if (ImGui::BeginMainMenuBar())
//	{
//		if (ImGui::BeginMenu("File"))
//		{
//			if (ImGui::MenuItem("Open", "CTRL+O"))
//			{
//				nfdchar_t* outPath = NULL;
//				nfdresult_t result = NFD_OpenDialog("obj;", NULL, &outPath);
//				if (result == NFD_OKAY)
//				{
//					std::shared_ptr<MeshModel>& myFile = Utils::LoadMeshModel(outPath);
//					scene.AddModel(myFile);
//					scene.SetActiveModelIndex(scene.GetModelCount() - 1);
//					free(outPath);
//				}
//				else if (result == NFD_CANCEL)
//				{
//				}
//				else
//				{
//				}
//
//			}
//			ImGui::EndMenu();
//		}
//		if (ImGui::MenuItem("model transformation"))
//		{
//			model_transformation = true;
//		}
//
//		if (ImGui::MenuItem("model controllers"))
//		{
//			model_controllers = true;
//		}
//
//		if (ImGui::MenuItem("camera transformation"))
//		{
//			camera_transformation = true;
//		}
//
//		if (ImGui::MenuItem("camera controllers"))
//		{
//			camera_controllers = true;
//		}
//		if (ImGui::MenuItem("Add a camera"))
//		{
//			addCamera = true;
//		}
//		if (ImGui::MenuItem("Light"))
//		{
//			light_controllers = true;
//		}
//
//		// TODO: Add more menubar items (if you want to)
//		ImGui::EndMainMenuBar();
//	}
//
//	// Controls
//	/*ImGui::ColorEdit3("Clear Color", (float*)&clear_color);*/
//	// TODO: Add more controls as needed
//
//	ImGui::End();
//
//
//
//	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
//	/*if (show_demo_window)
//		ImGui::ShowDemoWindow(&show_demo_window);*/
//
//		//Method #2
//	if (scene.GetModelCount() > 0)
//	{
//		MeshModel& curr = scene.GetActiveModel2();
//
//		static bool worldFlag = false, localFlag = false;
//
//		if (model_controllers)
//		{
//			ImGui::Begin("Keyboard controls");
//			ImGui::Text("Chose World or Local transformation to control from keyboard");
//			if (ImGui::Checkbox("World", &worldFlag))
//				localFlag = false;
//			if (ImGui::Checkbox("Local", &localFlag))
//				worldFlag = false;
//			ImGui::Text("W to move model up");
//			ImGui::Text("S to move model down");
//			ImGui::Text("A to move model left");
//			ImGui::Text("D to move model right");
//			ImGui::Text("UP Arrow to upscale model");
//			ImGui::Text("DOWN Arrow to downscale model");
//			ImGui::Text("X to rotate x axis");
//			ImGui::Text("Y to rotate y axis");
//			ImGui::Text("Z to rotate z axis");
//			ImGui::End();
//		}
//
//		{
//
//		}
//
//		if (!io.WantCaptureKeyboard) // Second method of transforming the model using the keyboard
//		{
//			if (localFlag)
//			{
//				if (io.KeysDown['W'] || io.KeysDown['w'])
//				{
//					curr.translationObject.y += 5;
//				}
//				if (io.KeysDown['S'] || io.KeysDown['s'])
//				{
//					curr.translationObject.y -= 5;
//				}
//				if (io.KeysDown['A'] || io.KeysDown['a'])
//				{
//					curr.translationObject.x -= 5;
//				}
//				if (io.KeysDown['D'] || io.KeysDown['d'])
//				{
//					curr.translationObject.x += 5;
//				}
//				if (io.KeysDown['X'] || io.KeysDown['x'])
//				{
//					curr.rotationObject.x >= 360 ? curr.rotationObject.x -= 360 : curr.rotationObject.x;
//					curr.rotationObject.x += 5;
//				}
//				if (io.KeysDown['Y'] || io.KeysDown['y'])
//				{
//					curr.rotationObject.y >= 360 ? curr.rotationObject.y -= 360 : curr.rotationObject.y;
//					curr.rotationObject.y += 5;
//				}
//				if (io.KeysDown['Z'] || io.KeysDown['z'])
//				{
//					curr.rotationObject.z >= 360 ? curr.rotationObject.z -= 360 : curr.rotationObject.z;
//					curr.rotationObject.z += 5;
//				}
//				if (io.KeysDown[264]) // DOWN Arrow
//				{
//					curr.scaleObject.x -= 5;
//					curr.scaleObject.y -= 5;
//					curr.scaleObject.z -= 5;
//				}
//				if (io.KeysDown[265]) // UP Arrow
//				{
//					curr.scaleObject.x += 5;
//					curr.scaleObject.y += 5;
//					curr.scaleObject.z += 5;
//				}
//			}
//			else if (worldFlag)
//			{
//				if (io.KeysDown['W'] || io.KeysDown['w'])
//				{
//					curr.translationWorld.y += 5;
//				}
//				if (io.KeysDown['S'] || io.KeysDown['s'])
//				{
//					curr.translationWorld.y -= 5;
//				}
//				if (io.KeysDown['A'] || io.KeysDown['a'])
//				{
//					curr.translationWorld.x -= 5;
//				}
//				if (io.KeysDown['D'] || io.KeysDown['d'])
//				{
//					curr.translationWorld.x += 5;
//				}
//				if (io.KeysDown['X'] || io.KeysDown['x'])
//				{
//					curr.rotationWorld.x >= 360 ? curr.rotationWorld.x -= 360 : curr.rotationWorld.x;
//					curr.rotationWorld.x += 5;
//				}
//				if (io.KeysDown['Y'] || io.KeysDown['y'])
//				{
//					curr.rotationWorld.y >= 360 ? curr.rotationWorld.y -= 360 : curr.rotationWorld.y;
//					curr.rotationWorld.y += 5;
//				}
//				if (io.KeysDown['Z'] || io.KeysDown['z'])
//				{
//					curr.rotationWorld.z >= 360 ? curr.rotationWorld.z -= 360 : curr.rotationWorld.z;
//					curr.rotationWorld.z += 5;
//				}
//				if (io.KeysDown[264]) // DOWN Arrow
//				{
//					curr.scaleWorld.x -= 0.1;
//					curr.scaleWorld.y -= 0.1;
//					curr.scaleWorld.z -= 0.1;
//				}
//				if (io.KeysDown[265]) // UP Arrow
//				{
//					curr.scaleWorld.x += 0.1;
//					curr.scaleWorld.y += 0.1;
//					curr.scaleWorld.z += 0.1;
//				}
//			}
//		}
//	}
//
//
//	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
//
//	{
//		static int counter = 0;
//		const int modelCount = scene.GetModelCount();
//		const int cameraCount = scene.GetCameraCount();
//		static char* items[] = { "0","1","2","3","4","5","6","7" };
//		static char* items2[] = { "0","1","2","3","4","5","6","7" };
//		static int selectedItemModel = modelCount;
//		static int selectedItemCamera = 0;
//
//		//Model controls
//		if (modelCount > 0)
//		{
//			{
//				if (model_transformation)
//
//				{
//					ImGui::Begin("Change Model Position");
//					ImGui::Combo("modelPicker", &selectedItemModel, items, modelCount);
//					scene.SetActiveModelIndex(selectedItemModel);
//					int index = scene.GetActiveModelIndex();
//					MeshModel& myModel = scene.GetModel2(index);
//					glm::vec3 translationObject(myModel.GetTranslationObject());
//					glm::vec3 rotationObject(myModel.GetRotationObject());
//					glm::vec3 scaleObject(myModel.GetScaleObject());
//
//					glm::vec3 translationWorld(myModel.GetTranslationWorld());
//					glm::vec3 rotationWorld(myModel.GetRotationWorld());
//					glm::vec3 scaleWorld(myModel.GetScaleWorld());
//
//					bool axisLocal = myModel.axisLocal;
//					bool axisWorld = myModel.axisWorld;
//					bool bBoxLocal = myModel.bounding_box_local;
//					bool bBoxWorld = myModel.bounding_box_world;
//					bool verticeNormals = myModel.drawVerticeNormals;
//					bool faceNormals = myModel.drawFaceNormals;
//					bool rectangles = myModel.drawRectangle;
//
//					ImGui::Text("Local Transformation");
//					ImGui::SliderFloat3("Translate-Local", &translationObject.x, -3.0f, 3.0f);
//					ImGui::SliderFloat3("Rotate-Local", &rotationObject.x, 0.0f, 360.0f);
//					ImGui::SliderFloat("Scale-Local", &scaleObject.x, 0.0f, 4.0f);
//					scaleObject.y = scaleObject.x;
//					scaleObject.z = scaleObject.x;
//					myModel.SetTranslationObject(translationObject);
//					myModel.SetRotationObject(rotationObject);
//					myModel.SetScaleObject(scaleObject);
//					myModel.SetObjectTransform();
//
//					ImGui::Text("World Transformation");
//
//					ImGui::SliderFloat3("Translate-World", &translationWorld.x, -3.0f, 3.0f);
//					ImGui::SliderFloat3("Rotate-World", &rotationWorld.x, 0.0f, 360.0f);
//					ImGui::SliderFloat("Scale-World", &scaleWorld.x, 0.0f, 4.0f);
//					scaleWorld.y = scaleWorld.x;
//					scaleWorld.z = scaleWorld.x;
//					myModel.SetTranslationWorld(translationWorld);
//					myModel.SetRotationWorld(rotationWorld);
//					myModel.SetScaleWorld(scaleWorld);
//					myModel.SetWorldTransform();
//
//					if (ImGui::Button("Center"))
//					{
//						myModel.SetTranslationObject(glm::vec3(0.0f));
//						myModel.SetRotationObject(glm::vec3(0.0f));
//						myModel.SetTranslationWorld(glm::vec3(0.0f));
//						myModel.SetRotationWorld(glm::vec3(0.0f));
//					}
//
//					if (ImGui::Checkbox("Axis Local", &axisLocal))
//					{
//						myModel.axisLocal = !myModel.axisLocal;
//					}
//
//					if (ImGui::Checkbox("Axis World", &axisWorld))
//					{
//						myModel.axisWorld = !myModel.axisWorld;
//					}
//
//					if (ImGui::Checkbox("B_Box Local", &bBoxLocal))
//					{
//						myModel.bounding_box_local = !myModel.bounding_box_local;
//					}
//
//					if (ImGui::Checkbox("B_Box World", &bBoxWorld))
//					{
//						myModel.bounding_box_world = !myModel.bounding_box_world;
//					}
//					if (ImGui::Checkbox("vertice normals", &verticeNormals))
//					{
//						myModel.drawVerticeNormals = !myModel.drawVerticeNormals;
//					}
//					if (ImGui::Checkbox("face normals", &faceNormals))
//					{
//						myModel.drawFaceNormals = !myModel.drawFaceNormals;
//					}
//					if (ImGui::Checkbox("ractangles", &rectangles))
//					{
//						myModel.drawRectangle = !myModel.drawRectangle;
//					}
//					ImGui::End();
//				}
//			}
//
//
//			//Camera controls
//
//
//
//			if (cameraCount > 0)
//			{
//				Camera& cam = scene.GetActiveCamera();
//				static float aspect = float(windowWidth) / float(windowHeight);
//				if (camera_controllers)
//				{
//					ImGui::Begin("Camera Controller");
//					ImGui::Combo("Pick a Camera", &selectedItemCamera, items2, cameraCount);
//					scene.SetActiveCameraIndex(selectedItemCamera);
//					Camera& cam = scene.GetActiveCamera();
//					if (ImGui::RadioButton("Ortho", cam.my_ortho_or_perspective)) { cam.my_ortho_or_perspective = true; }ImGui::SameLine();
//					if (ImGui::RadioButton("Perspective", !cam.my_ortho_or_perspective)) { cam.my_ortho_or_perspective = false; }
//					if (cam.my_ortho_or_perspective) // Ortho
//					{
//						ImGui::SliderFloat("Up", &cam.my_up, -10.0f, 10.0f);
//						ImGui::SliderFloat("Down", &cam.my_down, -10.0f, 10.0f);
//						ImGui::SliderFloat("Left", &cam.my_left, -10.0f, 10.0f);
//						ImGui::SliderFloat("right", &cam.my_right, -10.0f, 10.0f);
//						ImGui::SliderFloat("Near", &cam.my_zNearOrtho, -10.0f, 10.0f);
//						ImGui::SliderFloat("Far", &cam.my_zFarOrtho, -10.0f, 10.0f);
//
//						if (ImGui::Button("auto"))
//						{
//							cam.my_up = 1;
//							cam.my_down = -1;
//							cam.my_left = -1;
//							cam.my_right = 1;
//							cam.my_zNearOrtho = 1;
//							cam.my_zFarOrtho = -1;
//						}
//						cam.Ortho(cam.my_left, cam.my_right, cam.my_down, cam.my_up, cam.my_zNearOrtho, cam.my_zFarOrtho);
//					}
//					else // Perspective
//					{
//
//						ImGui::SliderFloat("Fovy in degrees", &cam.my_fovy, 1.0f, 360.0f);
//						ImGui::SliderFloat("Near", &cam.my_zNearPerspective, -10.0f, 10.0f);
//						ImGui::SliderFloat("Far", &cam.my_zFarPerspective, -100.0f, 100.0f);
//
//						if (ImGui::Button("reset"))
//						{
//
//							cam.my_fovy = 45.0f;
//							cam.my_zNearPerspective = 1;
//							cam.my_zFarPerspective = 100;
//						}
//						ImGui::SliderFloat("Dolly Zoom", &cam.dolly, -0.04f, 0.04f);
//						ImGui::Checkbox("Do you want to Dolly?", &cam.my_isDolly);
//						if (cam.my_isDolly)
//						{
//							cam.my_fovy += cam.dolly * 2.5f;
//							cam.translationWorld.z += cam.dolly / 5;
//
//						}
//						ImGui::Text("Camera controls");//Look at
//						ImGui::SliderFloat("eye X", &cam.my_eye.x, -10.0f, 10.0f);
//						ImGui::SliderFloat("eye Y", &cam.my_eye.y, -10.0f, 10.0f);
//						ImGui::SliderFloat("eye Z", &cam.my_eye.z, -10.0f, 10.0f);
//
//						ImGui::SliderFloat("at X", &cam.my_at.x, -10.0f, 10.0f);
//						ImGui::SliderFloat("at Y", &cam.my_at.y, -10.0f, 10.0f);
//						ImGui::SliderFloat("at Z", &cam.my_at.z, -10.0f, 10.0f);
//						if (ImGui::Button("auto"))
//						{
//							cam.my_eye = { 0.0f,0.0f,3.0f };
//							cam.my_at = { 1.0,1.0,1.0 };
//						}
//						cam.Perspective(glm::radians(cam.my_fovy), aspect, cam.my_zNearPerspective, cam.my_zFarPerspective);
//
//					}
//
//					ImGui::End();
//				}
//
//				//Camera transformations
//				if (addCamera)
//				{
//					ImGui::Begin("Add a Camera");
//					static glm::vec3 tempEye(0.0f, 0.0f, 3.0f);
//					static glm::vec3 tempAt(1.0f, 1.0f, 1.0f);
//					static float tempUp = 1.0f;
//					ImGui::SliderFloat3("Eye ?", &tempEye.x, -10.f, 10.f);
//					ImGui::SliderFloat3("At ?", &tempAt.x, -10.f, 10.f);
//					ImGui::SliderFloat("Up ?", &tempUp, -1.0f, 1.0f);
//
//					//if (ImGui::Button("Add !"))
//					//{
//					//	shared_ptr<Camera>& addedCam = std::make_shared<Camera>();
//					//	addedCam->Ortho(0.0f, 400.0f, 0.0f, 400.0f, -1.0f, 1.0f);
//					//	addedCam->SetCameraLookAt(glm::vec3(tempEye.x, tempEye.y, tempEye.z), glm::vec3(tempAt.x, tempAt.x, tempAt.x), glm::vec3(0, tempUp, 0));
//					//	addedCam->Perspective(glm::radians(45.0f), float(windowWidth) / float(windowHeight), 1.0f, 100.0f);
//					//	addedCam->my_eye = tempEye;
//					//	addedCam->my_at = tempAt;
//					//	addedCam->my_upper = tempUp;
//					//	scene.AddCamera(addedCam);
//					//	//scene.SetActiveCameraIndex(scene.GetCameraCount()-1);
//					//	selectedItemCamera = scene.GetCameraCount() - 1;
//					//	tempEye = glm::vec3(0.0f, 0.0f, 3.0f);
//					//	tempAt = glm::vec3(1.0f, 1.0f, 1.0f);
//					//	tempUp = 1.0f;
//					//	addCamera = false;
//					//}
//					ImGui::End();
//				}
//
//				if (camera_transformation)
//				{
//
//
//					ImGui::Begin("Change Camera Position");
//					//ImGui::Combo("Camera Picker", &selectedItemCamera, items, cameraCount);
//					//scene.SetActiveCameraIndex(selectedItemCamera);
//					//
//					//int index = scene.GetActiveCameraIndex();
//					glm::vec3 camtranslationObject(cam.GetTranslationObject());
//					glm::vec3 camrotationObject(cam.GetRotationObject());
//					glm::vec3 camscaleObject(cam.GetScaleObject());
//
//					glm::vec3 camtranslationWorld(cam.GetTranslationWorld());
//					glm::vec3 camrotationWorld(cam.GetRotationWorld());
//					glm::vec3 camscaleWorld(cam.GetScaleWorld());
//
//					ImGui::Text("Local Transformation");
//					ImGui::SliderFloat3("Translate-Local", &camtranslationObject.x, -10.0f, 10.0f);
//					ImGui::SliderFloat3("Rotate-Local", &camrotationObject.x, 0.0f, 360.0f);
//					ImGui::SliderFloat("Scale-Local", &camscaleObject.x, -5.0f, 5.0f);
//					camscaleObject.y = camscaleObject.x;
//					camscaleObject.z = camscaleObject.x;
//					cam.SetTranslationObject(camtranslationObject);
//					cam.SetRotationObject(camrotationObject);
//					cam.SetScaleObject(camscaleObject);
//					cam.SetObjectTransform();
//
//					ImGui::Text("World Transformation");
//
//					ImGui::SliderFloat3("Translate-World", &camtranslationWorld.x, -10.0f, 10.0f);
//					ImGui::SliderFloat3("Rotate-World", &camrotationWorld.x, 0.0f, 360.0f);
//					ImGui::SliderFloat("Scale-World", &camscaleWorld.x, 0.0f, 2.0f);
//					camscaleWorld.y = camscaleWorld.x;
//					camscaleWorld.z = camscaleWorld.x;
//					cam.SetTranslationWorld(camtranslationWorld);
//					cam.SetRotationWorld(camrotationWorld);
//					cam.SetScaleWorld(camscaleWorld);
//					cam.SetWorldTransform();
//					if (ImGui::Button("Center"))
//					{
//						cam.SetTranslationObject(glm::vec3(0.0f));
//						cam.SetRotationObject(glm::vec3(0.0f));
//						cam.SetTranslationWorld(glm::vec3(0.0f));
//						cam.SetRotationWorld(glm::vec3(0.0f));
//					}
//					ImGui::End();
//				}
//				cam.SetCameraLookAt(cam.my_eye, cam.my_at, glm::vec3(0, cam.my_upper, 0));
//
//
//
//			}
//			//light
//			MeshModel& model = scene.GetActiveModel2();
//			Light& light_test = scene.GetActiveLight();
//
//			//float pos_x = light_test.position.x;
//			//float pos_y = light_test.position.y;
//			//float pos_z = light_test.position.z;
//
//
//			glm::vec3 position = light_test.position;
//			glm::vec3 translate = light_test.translation;
//			float ambient_strength = light_test.ambient_strength;
//			float diffuse_strength = light_test.diffuse_strength;
//			float specular_strength = light_test.specular_strength;
//
//			glm::vec3 ambient = light_test.ambient;
//			glm::vec3 diffuse = light_test.diffuse;
//			glm::vec3 specular = light_test.specular;
//
//			static bool emerald = true;
//			static bool gold = false;
//			static bool ruby = false;
//
//			bool flat = light_test.flat_shading;
//			bool gouraud = light_test.gouraud_shading;
//			bool phong = light_test.phong_shading;
//
//
//			if (light_controllers)
//			{
//				ImGui::Begin("light modification");
//
//				ImGui::SliderFloat("move light-x", &translate.x, -3.0f, 3.0f);
//				ImGui::SliderFloat("move light-y", &translate.y, -3.0f, 3.0f);
//				ImGui::SliderFloat("move light-z", &translate.z, -10.0f, 10.0f);
//				light_test.translation = translate;
//				light_test.set_transform(light_test.translation);
//
//
//				ImGui::SliderFloat("ambient strength", &ambient_strength, 0.0f, 1.0f);
//				light_test.ambient_strength = ambient_strength;
//				ImGui::ColorEdit3("ambient color", (float*)&ambient);
//				light_test.ambient = ambient;
//				ImGui::SliderFloat("diffuse strength", &diffuse_strength, 0.0f, 1.0f);
//				light_test.diffuse_strength = diffuse_strength;
//				ImGui::ColorEdit3("diffuse color", (float*)&diffuse);
//				light_test.diffuse = diffuse;
//
//				ImGui::SliderFloat("specular strength", &specular_strength, 0.0f, 1.0f);
//				light_test.specular_strength = specular_strength;
//				ImGui::ColorEdit3("specular color", (float*)&specular);
//				light_test.specular = specular;
//
//				if (ImGui::RadioButton("emerald", emerald)) { emerald = true; gold = false; ruby = false; }
//				if (ImGui::RadioButton("gold", gold)) { emerald = false; gold = true; ruby = false; }
//				if (ImGui::RadioButton("ruby", ruby)) { emerald = false; gold = false; ruby = true; }
//				if (emerald)
//				{
//					model.ambient = glm::vec3(0.0215, 0.1745, 0.0215);
//					model.diffuse = glm::vec3(0.07568, 0.61424, 0.07568);
//					model.specular = glm::vec3(0.633, 0.727811, 0.633);
//				}
//				if (gold)
//				{
//					model.ambient = glm::vec3(0.24725, 0.1995, 0.0745);
//					model.diffuse = glm::vec3(0.75164, 0.60648, 0.22648);
//					model.specular = glm::vec3(0.628281, 0.555802, 0.366065);
//				}
//				if (ruby)
//				{
//					model.ambient = glm::vec3(0.1745, 0.01175, 0.01175);
//					model.diffuse = glm::vec3(0.61424, 0.04136, 0.04136);
//					model.specular = glm::vec3(0.727811, 0.626959, 0.626959);
//				}
//
//				if (ImGui::RadioButton("Flat shading", flat)) { flat = true; gouraud = false; phong = false; }
//				if (ImGui::RadioButton("Gouraud shading", gouraud)) { flat = false; gouraud = true; phong = false; }
//				if (ImGui::RadioButton("Phong shading", phong)) { flat = false; gouraud = false; phong = true; }
//
//				if (flat) { light_test.flat_shading = true; light_test.gouraud_shading = false; light_test.phong_shading = false; }
//				if (gouraud) { light_test.flat_shading = false; light_test.gouraud_shading = true; light_test.phong_shading = false; }
//				if (phong) { light_test.flat_shading = false; light_test.gouraud_shading = false; light_test.phong_shading = true; }
//
//
//			}
//		}
//	}
//}



//#define _USE_MATH_DEFINES
//#include <cmath>
//#include <imgui/imgui.h>
//#include <stdio.h>
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <nfd.h>
//#include <glm/glm.hpp>
//#include <glm/gtx/transform.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include "imgui_impl_glfw.h"
//#include "imgui_impl_opengl3.h"
//
//#include "Renderer.h"
//#include "Scene.h"
//#include "Utils.h"
//#include <cmath>
//#include <iostream>
//#include <string>
///**
// * Fields
// */
//bool show_demo_window = false;
//bool show_another_window = false;
//glm::vec4 clear_color = glm::vec4(0.2f, 0.6f, 0.5f, 0.5f);
//
///**
// * Function declarations
// */
//static void GlfwErrorCallback(int error, const char* description);
//GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name);
//ImGuiIO& SetupDearImgui(GLFWwindow* window);
//void StartFrame();
//void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io);
//void Cleanup(GLFWwindow* window);
//void DrawImguiMenus(ImGuiIO& io, Scene& scene);
//
///**
// * Function implementation
// */
//void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
//{
//	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
//	// TODO: Handle mouse scroll here
//}
//int windowWidth = 1920, windowHeight = 1080;
//
//int main(int argc, char **argv)
//{
//	GLFWwindow* window = SetupGlfwWindow(windowWidth, windowHeight, "Mesh Viewer");
//	if (!window)
//		return 1;
//
//	int frameBufferWidth, frameBufferHeight;
//	glfwMakeContextCurrent(window);
//	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
//
//	Renderer renderer = Renderer(frameBufferWidth, frameBufferHeight);
//	Scene scene = Scene();
//
//	//
//	shared_ptr<Camera> cam = std::make_shared<Camera>();
//	cam->Ortho(0.0f, 400.0f, 0.0f, 400.0f, -1.0f, 1.0f);
//	cam->SetCameraLookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(1, 1, 1), glm::vec3(0, 1, 0));
//	cam->Perspective(glm::radians(45.0f), float(windowWidth)/float(windowHeight), 1.0f, 100.0f);
//	scene.AddCamera(cam);
//	scene.SetActiveCameraIndex(0);
//
//	shared_ptr<light> light_test = std::make_shared<light>();
//	light_test->ambient = glm::vec3(0.3f, 0.2f, 0.4f);
//	light_test->position = glm::vec3(1.0f, 1.0f, 1.0f);
//	light_test->diffuse = glm::vec3(0.2f, 1.0f, 1.0f);
//	light_test->specular = glm::vec3(8.0f, 1.0f, 1.0f);
//	light_test->specular_strength = 0.5f;
//	light_test->ambient_strength = 0.1f;
//	light_test->diffuse_strength = 0.3f;
//	scene.AddLight(light_test);
//	scene.SetActiveLightIndex(0);
//
//
//	//
//	ImGuiIO& io = SetupDearImgui(window);
//	glfwSetScrollCallback(window, ScrollCallback);
//    while (!glfwWindowShouldClose(window))
//    {
//        glfwPollEvents();
//		StartFrame();
//		DrawImguiMenus(io, scene);
//		RenderFrame(window, scene, renderer, io);
//    }
//	Cleanup(window);
//    return 0;
//}
//
//static void GlfwErrorCallback(int error, const char* description)
//{
//	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
//}
//
//GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name)
//{
//	glfwSetErrorCallback(GlfwErrorCallback);
//	if (!glfwInit())
//		return NULL;
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//	
//	#if __APPLE__
//		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//	#endif
//	
//	GLFWwindow* window = glfwCreateWindow(w, h, window_name, NULL, NULL);
//	glfwMakeContextCurrent(window);
//	glfwSwapInterval(1); // Enable vsync
//						 // very importent!! initialization of glad
//						 // https://stackoverflow.com/questions/48582444/imgui-with-the-glad-opengl-loader-throws-segmentation-fault-core-dumped
//
//	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
//	return window;
//}
//
//ImGuiIO& SetupDearImgui(GLFWwindow* window)
//{
//	IMGUI_CHECKVERSION();
//	ImGui::CreateContext();
//	ImGuiIO& io = ImGui::GetIO();
//	ImGui_ImplGlfw_InitForOpenGL(window, true);
//	ImGui_ImplOpenGL3_Init();
//	ImGui::StyleColorsDark();
//	return io;
//}
//
//void StartFrame()
//{
//	ImGui_ImplOpenGL3_NewFrame();
//	ImGui_ImplGlfw_NewFrame();
//	ImGui::NewFrame();
//}
//
//void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io)
//{
//	ImGui::Render();
//	int frameBufferWidth, frameBufferHeight;
//	glfwMakeContextCurrent(window);
//	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
//	if (scene.GetCameraCount() == 0)
//	{
//
//	}
//	if (scene.GetModelCount() > 0)
//	{
//		MeshModel& curr = scene.GetActiveModel();
//
//
//
//		if (frameBufferWidth != renderer.GetViewportWidth() || frameBufferHeight != renderer.GetViewportHeight())
//		{
//			renderer.SetNewViewport(frameBufferWidth, frameBufferHeight);
//		}
//
//		if (!io.WantCaptureKeyboard)
//		{
//			// TODO: Handle keyboard events here
//			if (io.KeysDown[65])
//			{
//				// A key is down
//				// Use the ASCII table for more key codes (https://www.asciitable.com/)
//			}
//		}
//
//		if (!io.WantCaptureMouse)
//		{
//			//Method #1
//
//			if (io.MouseDown[0])
//			{
//				curr.translationWorld.x = -curr.translationObject.x + io.MousePos.x;
//				curr.translationWorld.y = -curr.translationObject.y + (renderer.GetViewportHeight() - io.MousePos.y);
//			}
//		}
//	}
//
//	renderer.ClearColorBuffer(clear_color);
//	renderer.clearZBuffer();
//	renderer.Render(scene);
//	renderer.SwapBuffers();
//
//	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
//	glfwMakeContextCurrent(window);
//	glfwSwapBuffers(window);
//}
//
//void Cleanup(GLFWwindow* window)
//{
//	ImGui_ImplOpenGL3_Shutdown();
//	ImGui_ImplGlfw_Shutdown();
//	ImGui::DestroyContext();
//
//	glfwDestroyWindow(window);
//	glfwTerminate();
//}
//
//void DrawImguiMenus(ImGuiIO& io, Scene& scene)
//{
//
//	/**
//	 * MeshViewer menu
//	 */
//
//	static bool model_transformation = false, model_controllers = false, camera_transformation = false, camera_controllers = false, addCamera = false, light_controllers=false;
//	ImGui::Begin("MeshViewer Menu");
//
//
//	// Menu Bar
//	if (ImGui::BeginMainMenuBar())
//	{
//		if (ImGui::BeginMenu("File"))
//		{
//			if (ImGui::MenuItem("Open", "CTRL+O"))
//			{
//				nfdchar_t* outPath = NULL;
//				nfdresult_t result = NFD_OpenDialog("obj;", NULL, &outPath);
//				if (result == NFD_OKAY)
//				{
//					shared_ptr<MeshModel>& myFile = Utils::LoadMeshModel(outPath);
//					scene.AddModel(myFile);
//					scene.SetActiveModelIndex(scene.GetModelCount() - 1);
//					free(outPath);
//				}
//				else if (result == NFD_CANCEL)
//				{
//				}
//				else
//				{
//				}
//
//			}
//			ImGui::EndMenu();
//		}
//		if (ImGui::MenuItem("model transformation"))
//		{
//			model_transformation = true;
//		}
//
//		if (ImGui::MenuItem("model controllers"))
//		{
//			model_controllers = true;
//		}
//
//		if (ImGui::MenuItem("camera transformation"))
//		{
//			camera_transformation = true;
//		}
//
//		if (ImGui::MenuItem("camera controllers"))
//		{
//			camera_controllers = true;
//		}
//		if (ImGui::MenuItem("Add a camera"))
//		{
//			addCamera = true;
//		}
//		if (ImGui::MenuItem("Light"))
//		{
//			light_controllers = true;
//		}
//
//		// TODO: Add more menubar items (if you want to)
//		ImGui::EndMainMenuBar();
//	}
//
//	// Controls
//	ImGui::ColorEdit3("Clear Color", (float*)&clear_color);
//	// TODO: Add more controls as needed
//
//	ImGui::End();
//
//
//
//	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
//	if (show_demo_window)
//		ImGui::ShowDemoWindow(&show_demo_window);
//
//	//Method #2
//	if (scene.GetModelCount() > 0)
//	{
//		MeshModel& curr = scene.GetActiveModel();
//
//		static bool worldFlag = false, localFlag = false;
//
//		if (model_controllers)
//		{
//			ImGui::Begin("Keyboard controls");
//			ImGui::Text("Chose World or Local transformation to control from keyboard");
//			if (ImGui::Checkbox("World", &worldFlag))
//				localFlag = false;
//			if (ImGui::Checkbox("Local", &localFlag))
//				worldFlag = false;
//			ImGui::Text("W to move model up");
//			ImGui::Text("S to move model down");
//			ImGui::Text("A to move model left");
//			ImGui::Text("D to move model right");
//			ImGui::Text("UP Arrow to upscale model");
//			ImGui::Text("DOWN Arrow to downscale model");
//			ImGui::Text("X to rotate x axis");
//			ImGui::Text("Y to rotate y axis");
//			ImGui::Text("Z to rotate z axis");
//			ImGui::End();
//		}
//
//		{
//
//		}
//
//		if (!io.WantCaptureKeyboard) // Second method of transforming the model using the keyboard
//		{
//			if (localFlag)
//			{
//				if (io.KeysDown['W'] || io.KeysDown['w'])
//				{
//					curr.translationObject.y += 5;
//				}
//				if (io.KeysDown['S'] || io.KeysDown['s'])
//				{
//					curr.translationObject.y -= 5;
//				}
//				if (io.KeysDown['A'] || io.KeysDown['a'])
//				{
//					curr.translationObject.x -= 5;
//				}
//				if (io.KeysDown['D'] || io.KeysDown['d'])
//				{
//					curr.translationObject.x += 5;
//				}
//				if (io.KeysDown['X'] || io.KeysDown['x'])
//				{
//					curr.rotationObject.x >= 360 ? curr.rotationObject.x -= 360 : curr.rotationObject.x;
//					curr.rotationObject.x += 5;
//				}
//				if (io.KeysDown['Y'] || io.KeysDown['y'])
//				{
//					curr.rotationObject.y >= 360 ? curr.rotationObject.y -= 360 : curr.rotationObject.y;
//					curr.rotationObject.y += 5;
//				}
//				if (io.KeysDown['Z'] || io.KeysDown['z'])
//				{
//					curr.rotationObject.z >= 360 ? curr.rotationObject.z -= 360 : curr.rotationObject.z;
//					curr.rotationObject.z += 5;
//				}
//				if (io.KeysDown[264]) // DOWN Arrow
//				{
//					curr.scaleObject.x -= 5;
//					curr.scaleObject.y -= 5;
//					curr.scaleObject.z -= 5;
//				}
//				if (io.KeysDown[265]) // UP Arrow
//				{
//					curr.scaleObject.x += 5;
//					curr.scaleObject.y += 5;
//					curr.scaleObject.z += 5;
//				}
//			}
//			else if (worldFlag)
//			{
//				if (io.KeysDown['W'] || io.KeysDown['w'])
//				{
//					curr.translationWorld.y += 5;
//				}
//				if (io.KeysDown['S'] || io.KeysDown['s'])
//				{
//					curr.translationWorld.y -= 5;
//				}
//				if (io.KeysDown['A'] || io.KeysDown['a'])
//				{
//					curr.translationWorld.x -= 5;
//				}
//				if (io.KeysDown['D'] || io.KeysDown['d'])
//				{
//					curr.translationWorld.x += 5;
//				}
//				if (io.KeysDown['X'] || io.KeysDown['x'])
//				{
//					curr.rotationWorld.x >= 360 ? curr.rotationWorld.x -= 360 : curr.rotationWorld.x;
//					curr.rotationWorld.x += 5;
//				}
//				if (io.KeysDown['Y'] || io.KeysDown['y'])
//				{
//					curr.rotationWorld.y >= 360 ? curr.rotationWorld.y -= 360 : curr.rotationWorld.y;
//					curr.rotationWorld.y += 5;
//				}
//				if (io.KeysDown['Z'] || io.KeysDown['z'])
//				{
//					curr.rotationWorld.z >= 360 ? curr.rotationWorld.z -= 360 : curr.rotationWorld.z;
//					curr.rotationWorld.z += 5;
//				}
//				if (io.KeysDown[264]) // DOWN Arrow
//				{
//					curr.scaleWorld.x -= 0.1;
//					curr.scaleWorld.y -= 0.1;
//					curr.scaleWorld.z -= 0.1;
//				}
//				if (io.KeysDown[265]) // UP Arrow
//				{
//					curr.scaleWorld.x += 0.1;
//					curr.scaleWorld.y += 0.1;
//					curr.scaleWorld.z += 0.1;
//				}
//			}
//		}
//	}
//
//
//	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
//
//			{
//				static int counter = 0;
//				const int modelCount = scene.GetModelCount();
//				const int cameraCount = scene.GetCameraCount();
//				static char* items[] = { "0","1","2","3","4","5","6","7" };
//				static char* items2[] = { "0","1","2","3","4","5","6","7" };
//				static int selectedItemModel = modelCount;
//				static int selectedItemCamera = 0;
//
//		//Model controls
//		if (modelCount > 0)
//		{
//			{
//				if (model_transformation)
//
//				{
//					ImGui::Begin("Change Model Position");
//					ImGui::Combo("modelPicker", &selectedItemModel, items, modelCount);
//					scene.SetActiveModelIndex(selectedItemModel);
//					int index = scene.GetActiveModelIndex();
//					MeshModel& myModel = scene.GetModel(index);
//					glm::vec3 translationObject(myModel.GetTranslationObject());
//					glm::vec3 rotationObject(myModel.GetRotationObject());
//					glm::vec3 scaleObject(myModel.GetScaleObject());
//
//					glm::vec3 translationWorld(myModel.GetTranslationWorld());
//					glm::vec3 rotationWorld(myModel.GetRotationWorld());
//					glm::vec3 scaleWorld(myModel.GetScaleWorld());
//
//					bool axisLocal = myModel.axisLocal;
//					bool axisWorld = myModel.axisWorld;
//					bool bBoxLocal = myModel.bounding_box_local;
//					bool bBoxWorld = myModel.bounding_box_world;
//					bool verticeNormals = myModel.drawVerticeNormals;
//					bool faceNormals = myModel.drawFaceNormals;
//					bool rectangles = myModel.drawRectangle;
//
//					ImGui::Text("Local Transformation");
//					ImGui::SliderFloat3("Translate-Local", &translationObject.x, -3.0f, 3.0f);
//					ImGui::SliderFloat3("Rotate-Local", &rotationObject.x, 0.0f, 360.0f);
//					ImGui::SliderFloat("Scale-Local", &scaleObject.x, 0.0f, 4.0f);
//					scaleObject.y = scaleObject.x;
//					scaleObject.z = scaleObject.x;
//					myModel.SetTranslationObject(translationObject);
//					myModel.SetRotationObject(rotationObject);
//					myModel.SetScaleObject(scaleObject);
//					myModel.SetObjectTransform();
//
//					ImGui::Text("World Transformation");
//
//					ImGui::SliderFloat3("Translate-World", &translationWorld.x, -3.0f, 3.0f);
//					ImGui::SliderFloat3("Rotate-World", &rotationWorld.x, 0.0f, 360.0f);
//					ImGui::SliderFloat("Scale-World", &scaleWorld.x, 0.0f, 4.0f);
//					scaleWorld.y = scaleWorld.x;
//					scaleWorld.z = scaleWorld.x;
//					myModel.SetTranslationWorld(translationWorld);
//					myModel.SetRotationWorld(rotationWorld);
//					myModel.SetScaleWorld(scaleWorld);
//					myModel.SetWorldTransform();
//
//					if (ImGui::Button("Center"))
//					{
//						myModel.SetTranslationObject(glm::vec3(0.0f));
//						myModel.SetRotationObject(glm::vec3(0.0f));
//						myModel.SetTranslationWorld(glm::vec3(0.0f));
//						myModel.SetRotationWorld(glm::vec3(0.0f));
//					}
//
//					if (ImGui::Checkbox("Axis Local", &axisLocal))
//					{
//						myModel.axisLocal = !myModel.axisLocal;
//					}
//
//					if (ImGui::Checkbox("Axis World", &axisWorld))
//					{
//						myModel.axisWorld = !myModel.axisWorld;
//					}
//
//					if (ImGui::Checkbox("B_Box Local", &bBoxLocal))
//					{
//						myModel.bounding_box_local = !myModel.bounding_box_local;
//					}
//
//					if (ImGui::Checkbox("B_Box World", &bBoxWorld))
//					{
//						myModel.bounding_box_world = !myModel.bounding_box_world;
//					}
//					if (ImGui::Checkbox("vertice normals", &verticeNormals))
//					{
//						myModel.drawVerticeNormals = !myModel.drawVerticeNormals;
//					}
//					if (ImGui::Checkbox("face normals", &faceNormals))
//					{
//						myModel.drawFaceNormals = !myModel.drawFaceNormals;
//					}
//					if (ImGui::Checkbox("ractangles", &rectangles))
//					{
//						myModel.drawRectangle = !myModel.drawRectangle;
//					}
//					ImGui::End();
//				}
//			}
//
//
//			//Camera controls
//
//
//
//			if (cameraCount > 0)
//			{
//				Camera& cam = scene.GetActiveCamera();
//				static float aspect = float(windowWidth)/float(windowHeight);
//				if (camera_controllers)
//				{
//					ImGui::Begin("Camera Controller");
//					ImGui::Combo("Pick a Camera", &selectedItemCamera, items2, cameraCount);
//					scene.SetActiveCameraIndex(selectedItemCamera);
//					Camera &cam =  scene.GetActiveCamera();
//					if(ImGui::RadioButton("Ortho", cam.my_ortho_or_perspective)){cam.my_ortho_or_perspective=true;}ImGui::SameLine();
//					if (ImGui::RadioButton("Perspective", !cam.my_ortho_or_perspective)) { cam.my_ortho_or_perspective = false; }
//					if (cam.my_ortho_or_perspective) // Ortho
//					{
//						ImGui::SliderFloat("Up", &cam.my_up, -10.0f, 10.0f);
//						ImGui::SliderFloat("Down", &cam.my_down, -10.0f, 10.0f);
//						ImGui::SliderFloat("Left", &cam.my_left, -10.0f, 10.0f);
//						ImGui::SliderFloat("right", &cam.my_right, -10.0f, 10.0f);
//						ImGui::SliderFloat("Near", &cam.my_zNearOrtho, -10.0f, 10.0f);
//						ImGui::SliderFloat("Far", &cam.my_zFarOrtho, -10.0f, 10.0f);
//
//						if (ImGui::Button("auto"))
//						{
//							cam.my_up = 1;
//							cam.my_down = -1;
//							cam.my_left = -1;
//							cam.my_right = 1;
//							cam.my_zNearOrtho = 1;
//							cam.my_zFarOrtho = -1;
//						}
//						cam.Ortho(cam.my_left, cam.my_right, cam.my_down, cam.my_up, cam.my_zNearOrtho, cam.my_zFarOrtho);
//					}
//					else // Perspective
//					{
//
//						ImGui::SliderFloat("Fovy in degrees", &cam.my_fovy, 1.0f, 360.0f);
//						ImGui::SliderFloat("Near", &cam.my_zNearPerspective, -10.0f, 10.0f);
//						ImGui::SliderFloat("Far", &cam.my_zFarPerspective, -100.0f, 100.0f);
//
//						if (ImGui::Button("reset"))
//						{
//
//							cam.my_fovy = 45.0f;
//							cam.my_zNearPerspective = 1;
//							cam.my_zFarPerspective = 100;
//						}
//						ImGui::SliderFloat("Dolly Zoom", &cam.dolly, -0.04f, 0.04f);
//						ImGui::Checkbox("Do you want to Dolly?", &cam.my_isDolly);
//						if (cam.my_isDolly)
//						{
//							cam.my_fovy += cam.dolly * 2.5f;
//							cam.translationWorld.z += cam.dolly / 5;
//
//						}
//						ImGui::Text("Camera controls");//Look at
//						ImGui::SliderFloat("eye X", &cam.my_eye.x, -10.0f, 10.0f);
//						ImGui::SliderFloat("eye Y", &cam.my_eye.y, -10.0f, 10.0f);
//						ImGui::SliderFloat("eye Z", &cam.my_eye.z, -10.0f, 10.0f);
//
//						ImGui::SliderFloat("at X", &cam.my_at.x, -10.0f, 10.0f);
//						ImGui::SliderFloat("at Y", &cam.my_at.y, -10.0f, 10.0f);
//						ImGui::SliderFloat("at Z", &cam.my_at.z, -10.0f, 10.0f);
//						if (ImGui::Button("auto"))
//						{
//							cam.my_eye = { 0.0f,0.0f,3.0f };
//							cam.my_at = { 1.0,1.0,1.0 };
//						}
//						cam.Perspective(glm::radians(cam.my_fovy), aspect, cam.my_zNearPerspective, cam.my_zFarPerspective);
//
//					}
//							
//					ImGui::End();
//				}
//
//				//Camera transformations
//				if (addCamera)
//				{
//					ImGui::Begin("Add a Camera");
//					static glm::vec3 tempEye(0.0f,0.0f,3.0f);
//					static glm::vec3 tempAt(1.0f, 1.0f, 1.0f);
//					static float tempUp = 1.0f;
//					ImGui::SliderFloat3("Eye ?", &tempEye.x, -10.f, 10.f);
//					ImGui::SliderFloat3("At ?", &tempAt.x, -10.f, 10.f);
//					ImGui::SliderFloat("Up ?", &tempUp, -1.0f, 1.0f);
//							
//					if (ImGui::Button("Add !"))
//					{
//						shared_ptr<Camera>& addedCam = std::make_shared<Camera>();
//						addedCam->Ortho(0.0f, 400.0f, 0.0f, 400.0f, -1.0f, 1.0f);
//						addedCam->SetCameraLookAt(glm::vec3(tempEye.x, tempEye.y, tempEye.z), glm::vec3(tempAt.x, tempAt.x, tempAt.x), glm::vec3(0, tempUp, 0));
//						addedCam->Perspective(glm::radians(45.0f), float(windowWidth) / float(windowHeight), 1.0f, 100.0f);
//						addedCam->my_eye = tempEye;
//						addedCam->my_at = tempAt;
//						addedCam->my_upper = tempUp;
//						scene.AddCamera(addedCam);
//						//scene.SetActiveCameraIndex(scene.GetCameraCount()-1);
//						selectedItemCamera = scene.GetCameraCount() - 1;
//						tempEye = glm::vec3(0.0f, 0.0f, 3.0f);
//						tempAt = glm::vec3(1.0f, 1.0f, 1.0f);
//						tempUp = 1.0f;
//						addCamera = false;
//					}
//					ImGui::End();
//				}
//
//				if (camera_transformation)
//				{
//
//
//					ImGui::Begin("Change Camera Position");
//					//ImGui::Combo("Camera Picker", &selectedItemCamera, items, cameraCount);
//					//scene.SetActiveCameraIndex(selectedItemCamera);
//					//
//					//int index = scene.GetActiveCameraIndex();
//					glm::vec3 camtranslationObject(cam.GetTranslationObject());
//					glm::vec3 camrotationObject(cam.GetRotationObject());
//					glm::vec3 camscaleObject(cam.GetScaleObject());
//
//				glm::vec3 camtranslationWorld(cam.GetTranslationWorld());
//				glm::vec3 camrotationWorld(cam.GetRotationWorld());
//				glm::vec3 camscaleWorld(cam.GetScaleWorld());
//
//				ImGui::Text("Local Transformation");
//				ImGui::SliderFloat3("Translate-Local", &camtranslationObject.x, -10.0f, 10.0f);
//				ImGui::SliderFloat3("Rotate-Local", &camrotationObject.x, 0.0f, 360.0f);
//				ImGui::SliderFloat("Scale-Local", &camscaleObject.x, -5.0f, 5.0f);
//				camscaleObject.y = camscaleObject.x;
//				camscaleObject.z = camscaleObject.x;
//				cam.SetTranslationObject(camtranslationObject);
//				cam.SetRotationObject(camrotationObject);
//				cam.SetScaleObject(camscaleObject);
//				cam.SetObjectTransform();
//
//					ImGui::Text("World Transformation");
//
//					ImGui::SliderFloat3("Translate-World", &camtranslationWorld.x, -10.0f, 10.0f);
//					ImGui::SliderFloat3("Rotate-World", &camrotationWorld.x, 0.0f, 360.0f);
//					ImGui::SliderFloat("Scale-World", &camscaleWorld.x, 0.0f, 2.0f);
//					camscaleWorld.y = camscaleWorld.x;
//					camscaleWorld.z = camscaleWorld.x;
//					cam.SetTranslationWorld(camtranslationWorld);
//					cam.SetRotationWorld(camrotationWorld);
//					cam.SetScaleWorld(camscaleWorld);
//					cam.SetWorldTransform();
//					if (ImGui::Button("Center"))
//					{
//						cam.SetTranslationObject(glm::vec3(0.0f));
//						cam.SetRotationObject(glm::vec3(0.0f));
//						cam.SetTranslationWorld(glm::vec3(0.0f));
//						cam.SetRotationWorld(glm::vec3(0.0f));
//					}
//					ImGui::End();
//				}
//				cam.SetCameraLookAt(cam.my_eye, cam.my_at, glm::vec3(0, cam.my_upper, 0));
//
//
//
//	}
//			//light
//			MeshModel& model = scene.GetActiveModel();
//			light& light_test = scene.GetActiveLight();
//
//			//float pos_x = light_test.position.x;
//			//float pos_y = light_test.position.y;
//			//float pos_z = light_test.position.z;
//			
//
//			glm::vec3 position = light_test.position;
//			glm::vec3 translate = light_test.translation;
//			float ambient_strength = light_test.ambient_strength;
//			float diffuse_strength = light_test.diffuse_strength;
//			float specular_strength = light_test.specular_strength;
//
//			glm::vec3 ambient = light_test.ambient;
//			glm::vec3 diffuse = light_test.diffuse;
//			glm::vec3 specular = light_test.specular;
//
//			static bool emerald = true;
//			static bool gold = false;
//			static bool ruby = false;
//
//			 bool flat = light_test.flat_shading;
//			 bool gouraud = light_test.gouraud_shading;
//			 bool phong = light_test.phong_shading;
//
//
//			if (light_controllers)
//			{
//				ImGui::Begin("light modification");
//
//				ImGui::SliderFloat("move light-x", &translate.x, -3.0f, 3.0f);
//				ImGui::SliderFloat("move light-y", &translate.y, -3.0f, 3.0f);
//				ImGui::SliderFloat("move light-z", &translate.z, -10.0f, 10.0f);
//				light_test.translation = translate;
//				light_test.set_transform(light_test.translation);
//
//
//				ImGui::SliderFloat("ambient strength",&ambient_strength, 0.0f, 1.0f);
//				light_test.ambient_strength = ambient_strength;
//				ImGui::ColorEdit3("ambient color",(float*)&ambient);
//				light_test.ambient = ambient;
//				ImGui::SliderFloat("diffuse strength", &diffuse_strength, 0.0f, 1.0f);
//				light_test.diffuse_strength = diffuse_strength;
//				ImGui::ColorEdit3("diffuse color", (float*)&diffuse);
//				light_test.diffuse = diffuse;
//
//				ImGui::SliderFloat("specular strength", &specular_strength, 0.0f, 1.0f);
//				light_test.specular_strength = specular_strength;
//				ImGui::ColorEdit3("specular color", (float*)&specular);
//				light_test.specular = specular;
//
//				if (ImGui::RadioButton("emerald", emerald)) { emerald = true;gold = false;ruby = false; }
//				if (ImGui::RadioButton("gold", gold)) { emerald = false;gold = true;ruby = false; }
//				if (ImGui::RadioButton("ruby", ruby)) { emerald = false;gold = false;ruby = true; }
//				if (emerald) 
//				{
//					model.ambient=glm::vec3(0.0215, 0.1745, 0.0215);
//					model.diffuse=glm::vec3(0.07568, 0.61424, 0.07568);
//					model.specular=glm::vec3(0.633, 0.727811, 0.633);
//				}
//				if (gold)
//				{
//					model.ambient = glm::vec3(0.24725, 0.1995, 0.0745);
//					model.diffuse = glm::vec3(0.75164, 0.60648, 0.22648);
//					model.specular = glm::vec3(0.628281, 0.555802, 0.366065);
//				}
//				if (ruby)
//				{
//					model.ambient = glm::vec3(0.1745, 0.01175, 0.01175);
//					model.diffuse = glm::vec3(0.61424, 0.04136, 0.04136);
//					model.specular = glm::vec3(0.727811, 0.626959, 0.626959);
//				}
//
//				if (ImGui::RadioButton("Flat shading", flat)) { flat = true;gouraud = false;phong = false; }
//				if (ImGui::RadioButton("Gouraud shading", gouraud)) { flat = false;gouraud = true;phong = false; }
//				if (ImGui::RadioButton("Phong shading", phong)) { flat = false;gouraud = false;phong = true; }
//
//				if (flat) { light_test.flat_shading = true;light_test.gouraud_shading = false;light_test.phong_shading = false; }
//				if (gouraud) { light_test.flat_shading = false;light_test.gouraud_shading = true;light_test.phong_shading = false; }
//				if (phong) { light_test.flat_shading = false;light_test.gouraud_shading = false;light_test.phong_shading = true; }
//
//
//			}
//		}
//	}
//		}
