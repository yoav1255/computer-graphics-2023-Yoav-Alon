#define _USE_MATH_DEFINES
#include <cmath>
#include <imgui/imgui.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nfd.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Renderer.h"
#include "Scene.h"
#include "Utils.h"
#include <cmath>
#include <iostream>
#include <string>
/**
 * Fields
 */
bool show_demo_window = false;
bool show_another_window = false;
glm::vec4 clear_color = glm::vec4(0.2f, 0.6f, 0.5f, 0.5f);

/**
 * Function declarations
 */
static void GlfwErrorCallback(int error, const char* description);
GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name);
ImGuiIO& SetupDearImgui(GLFWwindow* window);
void StartFrame();
void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io);
void Cleanup(GLFWwindow* window);
void DrawImguiMenus(ImGuiIO& io, Scene& scene);

/**
 * Function implementation
 */
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	// TODO: Handle mouse scroll here
}

int main(int argc, char **argv)
{
	int windowWidth = 1920, windowHeight = 1080;
	GLFWwindow* window = SetupGlfwWindow(windowWidth, windowHeight, "Mesh Viewer");
	if (!window)
		return 1;

	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

	Renderer renderer = Renderer(frameBufferWidth, frameBufferHeight);
	Scene scene = Scene();

	//
	shared_ptr<Camera> cam = std::make_shared<Camera>();
	cam->Ortho(0.0f, 400.0f, 0.0f, 400.0f, -1.0f, 1.0f);
	cam->SetCameraLookAt(glm::vec3(1, 1, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	scene.AddCamera(cam);
	scene.SetActiveCameraIndex(0);

	//
	ImGuiIO& io = SetupDearImgui(window);
	glfwSetScrollCallback(window, ScrollCallback);
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
		StartFrame();
		DrawImguiMenus(io, scene);
		RenderFrame(window, scene, renderer, io);
    }
	Cleanup(window);
    return 0;
}

static void GlfwErrorCallback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

GLFWwindow* SetupGlfwWindow(int w, int h, const char* window_name)
{
	glfwSetErrorCallback(GlfwErrorCallback);
	if (!glfwInit())
		return NULL;
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	#if __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif
	
	GLFWwindow* window = glfwCreateWindow(w, h, window_name, NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync
						 // very importent!! initialization of glad
						 // https://stackoverflow.com/questions/48582444/imgui-with-the-glad-opengl-loader-throws-segmentation-fault-core-dumped

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	return window;
}

ImGuiIO& SetupDearImgui(GLFWwindow* window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();
	ImGui::StyleColorsDark();
	return io;
}

void StartFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void RenderFrame(GLFWwindow* window, Scene& scene, Renderer& renderer, ImGuiIO& io)
{
	ImGui::Render();
	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);
	if (scene.GetCameraCount() == 0)
	{

	}
	if (scene.GetModelCount() > 0)
	{
		MeshModel& curr = scene.GetActiveModel();



		if (frameBufferWidth != renderer.GetViewportWidth() || frameBufferHeight != renderer.GetViewportHeight())
		{
			// TODO: Set new aspect ratio
		}

		if (!io.WantCaptureKeyboard)
		{
			// TODO: Handle keyboard events here
			if (io.KeysDown[65])
			{
				// A key is down
				// Use the ASCII table for more key codes (https://www.asciitable.com/)
			}
		}

		if (!io.WantCaptureMouse)
		{
			//Method #1

			if (io.MouseDown[0])
			{
				curr.translationWorld.x = -curr.translationObject.x + io.MousePos.x;
				curr.translationWorld.y = -curr.translationObject.y + (renderer.GetViewportHeight() - io.MousePos.y);
			}
		}
	}

	renderer.ClearColorBuffer(clear_color);
	renderer.Render(scene);
	renderer.SwapBuffers();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwMakeContextCurrent(window);
	glfwSwapBuffers(window);
}

void Cleanup(GLFWwindow* window)
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();
}

void DrawImguiMenus(ImGuiIO& io, Scene& scene)
{

	/**
	 * MeshViewer menu
	 */

	static bool model_transformation=false, model_controllers=false, camera_transformation=false, camera_controllers=false;
	ImGui::Begin("MeshViewer Menu");


	// Menu Bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open", "CTRL+O"))
			{
				nfdchar_t* outPath = NULL;
				nfdresult_t result = NFD_OpenDialog("obj;", NULL, &outPath);
				if (result == NFD_OKAY)
				{
					shared_ptr<MeshModel>& myFile = Utils::LoadMeshModel(outPath);
					scene.AddModel(myFile);
					scene.SetActiveModelIndex(scene.GetModelCount() - 1);
					free(outPath);
					//std::cout << myFile;
				}
				else if (result == NFD_CANCEL)
				{
				}
				else
				{
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

		// TODO: Add more menubar items (if you want to)
		ImGui::EndMainMenuBar();
	}

	// Controls
	ImGui::ColorEdit3("Clear Color", (float*)&clear_color);
	// TODO: Add more controls as needed

	ImGui::End();


	
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

		//Method #2
	if (scene.GetModelCount() > 0)
	{
		MeshModel& curr = scene.GetActiveModel();

		static bool worldFlag = false, localFlag = false;

		if(model_controllers)
		{
			ImGui::Begin("Keyboard controls");
			ImGui::Text("Chose World or Local transformation to control from keyboard");
			if (ImGui::Checkbox("World", &worldFlag))
				localFlag = false;
			if (ImGui::Checkbox("Local", &localFlag))
				worldFlag = false;
			ImGui::Text("W to move model up");
			ImGui::Text("S to move model down");
			ImGui::Text("A to move model left");
			ImGui::Text("D to move model right");
			ImGui::Text("UP Arrow to upscale model");
			ImGui::Text("DOWN Arrow to downscale model");
			ImGui::Text("X to rotate x axis");
			ImGui::Text("Y to rotate y axis");
			ImGui::Text("Z to rotate z axis");
			ImGui::End();
		}

		{

		}

		if (!io.WantCaptureKeyboard) // Second method of transforming the model using the keyboard
		{
			if (localFlag)
			{
				if (io.KeysDown['W'] || io.KeysDown['w'])
				{
					curr.translationObject.y += 5;
				}
				if (io.KeysDown['S'] || io.KeysDown['s'])
				{
					curr.translationObject.y -= 5;
				}
				if (io.KeysDown['A'] || io.KeysDown['a'])
				{
					curr.translationObject.x -= 5;
				}
				if (io.KeysDown['D'] || io.KeysDown['d'])
				{
					curr.translationObject.x += 5;
				}
				if (io.KeysDown['X'] || io.KeysDown['x'])
				{
					curr.rotationObject.x >= 360 ? curr.rotationObject.x -= 360 : curr.rotationObject.x;
					curr.rotationObject.x += 5;
				}
				if (io.KeysDown['Y'] || io.KeysDown['y'])
				{
					curr.rotationObject.y >= 360 ? curr.rotationObject.y -= 360 : curr.rotationObject.y;
					curr.rotationObject.y += 5;
				}
				if (io.KeysDown['Z'] || io.KeysDown['z'])
				{
					curr.rotationObject.z >= 360 ? curr.rotationObject.z -= 360 : curr.rotationObject.z;
					curr.rotationObject.z += 5;
				}
				if (io.KeysDown[264]) // DOWN Arrow
				{
					curr.scaleObject.x -= 5;
					curr.scaleObject.y -= 5;
					curr.scaleObject.z -= 5;
				}
				if (io.KeysDown[265]) // UP Arrow
				{
					curr.scaleObject.x += 5;
					curr.scaleObject.y += 5;
					curr.scaleObject.z += 5;
				}
			}
			else if (worldFlag)
			{
				if (io.KeysDown['W'] || io.KeysDown['w'])
				{
					curr.translationWorld.y += 5;
				}
				if (io.KeysDown['S'] || io.KeysDown['s'])
				{
					curr.translationWorld.y -= 5;
				}
				if (io.KeysDown['A'] || io.KeysDown['a'])
				{
					curr.translationWorld.x -= 5;
				}
				if (io.KeysDown['D'] || io.KeysDown['d'])
				{
					curr.translationWorld.x += 5;
				}
				if (io.KeysDown['X'] || io.KeysDown['x'])
				{
					curr.rotationWorld.x >= 360 ? curr.rotationWorld.x -= 360 : curr.rotationWorld.x;
					curr.rotationWorld.x += 5;
				}
				if (io.KeysDown['Y'] || io.KeysDown['y'])
				{
					curr.rotationWorld.y >= 360 ? curr.rotationWorld.y -= 360 : curr.rotationWorld.y;
					curr.rotationWorld.y += 5;
				}
				if (io.KeysDown['Z'] || io.KeysDown['z'])
				{
					curr.rotationWorld.z >= 360 ? curr.rotationWorld.z -= 360 : curr.rotationWorld.z;
					curr.rotationWorld.z += 5;
				}
				if (io.KeysDown[264]) // DOWN Arrow
				{
					curr.scaleWorld.x -= 0.1;
					curr.scaleWorld.y -= 0.1;
					curr.scaleWorld.z -= 0.1;
				}
				if (io.KeysDown[265]) // UP Arrow
				{
					curr.scaleWorld.x += 0.1;
					curr.scaleWorld.y += 0.1;
					curr.scaleWorld.z += 0.1;
				}
			}
		}
	}

		
			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.

			{
				static int counter = 0;
				const int modelCount = scene.GetModelCount();
				const int cameraCount = scene.GetCameraCount();
				static char* items[] = { "0","1","2","3","4","5","6","7" };
				static int selectedItemModel = modelCount;

				//Model controls
				if (modelCount > 0)
				{
					{
						if (model_transformation)

						{
							ImGui::Begin("Change Model Position");
							ImGui::Combo("modelPicker", &selectedItemModel, items, modelCount);
							scene.SetActiveModelIndex(selectedItemModel);
							int index = scene.GetActiveModelIndex();
							MeshModel& myModel = scene.GetModel(index);
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
							 bool verticeNormals = myModel.drawNormals;

							ImGui::Text("Local Transformation");
							ImGui::SliderFloat3("Translate-Local", &translationObject.x, -1000.0f, 1000.0f);
							ImGui::SliderFloat3("Rotate-Local", &rotationObject.x, 0.0f, 360.0f);
							ImGui::SliderFloat("Scale-Local", &scaleObject.x, 0.0f, 2000.0f);
							scaleObject.y = scaleObject.x;
							scaleObject.z = scaleObject.x;
							myModel.SetTranslationObject(translationObject);
							myModel.SetRotationObject(rotationObject);
							myModel.SetScaleObject(scaleObject);
							myModel.SetObjectTransform();

							ImGui::Text("World Transformation");

							ImGui::SliderFloat3("Translate-World", &translationWorld.x, -1000.0f, 1000.0f);
							ImGui::SliderFloat3("Rotate-World", &rotationWorld.x, 0.0f, 360.0f);
							ImGui::SliderFloat("Scale-World", &scaleWorld.x, 0.0f, 2.0f);
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
								myModel.drawNormals = !myModel.drawNormals;
							}


							ImGui::End();
						}

					}
					//Camera controls

					if (cameraCount > 0)
					{
						Camera& cam = scene.GetActiveCamera();
						static float up = 1.0f;
						static float down = -1.0f;
						static float left = -1.0f;
						static float right = 1.0f;
						static float zNear = 1.0f;
						static float zFar = -1.0f;
						cam.Ortho(left, right, down, up, zNear, zFar);
						static glm::vec3 eye = glm::vec3(0.0f, 0.0f, 0.1f);
						static glm::vec3 at = glm::vec3(0.0f);
						static float upper = 1.0f;
						cam.SetCameraLookAt(eye, at, glm::vec3(0.0f, upper, 0.0f));
						if (camera_controllers)
						{
							ImGui::Begin("Camera Controller");
							ImGui::Text("View Volume");
							ImGui::SliderFloat("Up", &up, -10.0f, 10.0f);
							ImGui::SliderFloat("Down", &down, -10.0f, 10.0f);
							ImGui::SliderFloat("Left", &left, -10.0f, 10.0f);
							ImGui::SliderFloat("right", &right, -10.0f, 10.0f);
							ImGui::SliderFloat("Near", &zNear, -10.0f, 10.0f);
							ImGui::SliderFloat("Far", &zFar, -10.0f, 10.0f);

							if (ImGui::Button("auto"))
							{
								up = 1;
								down = -1;
								left = -1;
								right = 1;
								zNear = 1;
								zFar = -1;
							}

							ImGui::Text("Camera controls");
							ImGui::SliderFloat("eye X", &eye.x, 0.0f, 10.0f);
							ImGui::SliderFloat("eye Y", &eye.y, 0.0f, 10.0f);
							ImGui::SliderFloat("eye Z", &eye.z, 0.0f, 10.0f);

							ImGui::SliderFloat("at X", &at.x, 0.0f, 10.0f);
							ImGui::SliderFloat("at Y", &at.y, 0.0f, 10.0f);
							ImGui::SliderFloat("at Z", &at.z, 0.0f, 10.0f);


							//std::vector<const char*> camera_names;
							//for (int i = 0; i < scene.GetCameraCount(); i++)
							//{
							//	const std::string& name = scene.GetCamera(i).GetName();
							//	camera_names.push_back(name.c_str());
							//}
							ImGui::End();
						}

						if (camera_transformation)
						{

							static int selectedItemCamera = cameraCount;

							ImGui::Begin("Change Camera Position");
							//ImGui::Combo("Camera Picker", &selectedItemCamera, items, cameraCount);
							//scene.SetActiveCameraIndex(selectedItemCamera);
							//
							//int index = scene.GetActiveCameraIndex();
							Camera& cam = scene.GetActiveCamera();

							glm::vec3 camtranslationObject(cam.GetTranslationObject());
							glm::vec3 camrotationObject(cam.GetRotationObject());
							glm::vec3 camscaleObject(cam.GetScaleObject());

							glm::vec3 camtranslationWorld(cam.GetTranslationWorld());
							glm::vec3 camrotationWorld(cam.GetRotationWorld());
							glm::vec3 camscaleWorld(cam.GetScaleWorld());

							ImGui::Text("Local Transformation");
							ImGui::SliderFloat3("Translate-Local", &camtranslationObject.x, -1000.0f, 1000.0f);
							ImGui::SliderFloat3("Rotate-Local", &camrotationObject.x, 0.0f, 360.0f);
							ImGui::SliderFloat("Scale-Local", &camscaleObject.x, 0.0f, 2000.0f);
							camscaleObject.y = camscaleObject.x;
							camscaleObject.z = camscaleObject.x;
							cam.SetTranslationObject(camtranslationObject);
							cam.SetRotationObject(camrotationObject);
							cam.SetScaleObject(camscaleObject);
							cam.SetObjectTransform();

							ImGui::Text("World Transformation");

							ImGui::SliderFloat3("Translate-World", &camtranslationWorld.x, -1000.0f, 1000.0f);
							ImGui::SliderFloat3("Rotate-World", &camrotationWorld.x, 0.0f, 360.0f);
							ImGui::SliderFloat("Scale-World", &camscaleWorld.x, 0.0f, 2.0f);
							camscaleWorld.y = camscaleWorld.x;
							camscaleWorld.z = camscaleWorld.x;
							cam.SetTranslationWorld(camtranslationWorld);
							cam.SetRotationWorld(camrotationWorld);
							cam.SetScaleWorld(camscaleWorld);
							cam.SetWorldTransform();
							if (ImGui::Button("Center"))
							{
								cam.SetTranslationObject(glm::vec3(0.0f));
								cam.SetRotationObject(glm::vec3(0.0f));
								cam.SetTranslationWorld(glm::vec3(0.0f));
								cam.SetRotationWorld(glm::vec3(0.0f));
							}
							ImGui::End();
						}
					}
				}
			}

			// 3. Show another simple window.
			if (show_another_window)
			{
				ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
				ImGui::Text("Hello from another window!");
				if (ImGui::Button("Close Me"))
					show_another_window = false;
				ImGui::End();
			}
		}
