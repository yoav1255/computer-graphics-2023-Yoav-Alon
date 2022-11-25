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
	int windowWidth = 1280, windowHeight = 720;
	GLFWwindow* window = SetupGlfwWindow(windowWidth, windowHeight, "Mesh Viewer");
	if (!window)
		return 1;

	int frameBufferWidth, frameBufferHeight;
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &frameBufferWidth, &frameBufferHeight);

	Renderer renderer = Renderer(frameBufferWidth, frameBufferHeight);
	Scene scene = Scene();
	//load Model
		//std::string url = "C:/Users/YoavS/Documents/GitHub/computer-graphics-2023-yoavalon/Data/banana.obj";
		//shared_ptr<MeshModel>& myFile = Utils::LoadMeshModel(url);
	//std::cout << myFile;

	//Initial vertices to the center of the screen
	
	//myFile->SetScaleObject(glm::vec3(2000.0f, 2000.0f, 2000.0f));
	//myFile->SetTranslationObject( glm::vec3(500.0f, 400.0f, 100.0f));
	//myFile->SetRotationObject(glm::vec3(0, 0, 0));
	//myFile->SetObjectTransform();

	//glm::mat4 translateMat = glm::translate(glm::mat4(1.0f),glm::vec3(500.0f,500.0f,0.0f ));
	//glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(2000.0f, 2000.0f, 2000.0f));
	//glm::mat4 scaleAndTranslate =  translateMat*scaleMat;
	//myFile->setVertices(scaleAndTranslate);

		//scene.AddModel(myFile);
	
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
		// TODO: Handle mouse events here
		if (io.MouseDown[0])
		{


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
					scene.AddModel(Utils::LoadMeshModel(outPath));
					scene.SetActiveModelIndex(scene.GetModelCount()-1);
					free(outPath);
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

		// TODO: Add more menubar items (if you want to)
		ImGui::EndMainMenuBar();
	}

	// Controls
	ImGui::ColorEdit3("Clear Color", (float*)&clear_color);
	// TODO: Add more controls as needed
	
	ImGui::End();

	/**
	 * Imgui demo - you can remove it once you are familiar with imgui
	 */
	
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.

	{
		static int counter = 0;
		const int modelCount = scene.GetModelCount();
		static char* items[] = { "0","1","2","3","4","5","6","7"};
		static int selectedItem = modelCount;
		if (modelCount > 0)
		{
			ImGui::Begin("Change Model Position");
			ImGui::Combo("modelPicker", &selectedItem, items, modelCount);
			scene.SetActiveModelIndex(selectedItem);

			int index = scene.GetActiveModelIndex();
			MeshModel& myModel = scene.GetModel(index);

			 glm::vec3 translationObject(myModel.GetTranslationObject());
			 glm::vec3 rotationObject(myModel.GetRotationObject());
			 glm::vec3 scaleObject(myModel.GetScaleObject());

			 glm::vec3 translationWorld(myModel.GetTranslationWorld());
			 glm::vec3 rotationWorld(myModel.GetRotationWorld());
			 glm::vec3 scaleWorld(myModel.GetScaleWorld());


			ImGui::Text("Local Transformation");
			ImGui::SliderFloat3("Translate-Local", &translationObject.x, 0.0f, 1000.0f);
			ImGui::SliderFloat3("Rotate-Local", &rotationObject.x, 0.0f, 360.0f);
			ImGui::SliderFloat("Scale-Local", &scaleObject.x, 0.0f, 2000.0f);
			scaleObject.y = scaleObject.x;
			scaleObject.z = scaleObject.x;
			myModel.SetTranslationObject(translationObject);
			myModel.SetRotationObject(rotationObject);
			myModel.SetScaleObject(scaleObject);
			myModel.SetObjectTransform();

			ImGui::Text("World Transformation");

			ImGui::SliderFloat3("Translate-World", &translationWorld.x, 0.0f, 1000.0f);
			ImGui::SliderFloat3("Rotate-World", &rotationWorld.x, 0.0f, 360.0f);
			ImGui::SliderFloat("Scale-World", &scaleWorld.x, 0.0f, 2.0f);
			scaleWorld.y = scaleWorld.x;
			scaleWorld.z = scaleWorld.x;
			myModel.SetTranslationWorld(translationWorld);
			myModel.SetRotationWorld(rotationWorld);
			myModel.SetScaleWorld(scaleWorld);
			myModel.SetWorldTransform();
		}



		ImGui::End();

	
		//ImGui::Begin("something else");
		//ImGui::Checkbox("Another Window", &show_another_window);

		//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		//	counter++;
		//ImGui::SameLine();
		//ImGui::Text("counter = %d", counter);

		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		//ImGui::End();
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