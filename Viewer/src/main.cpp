#define _USE_MATH_DEFINES
#include <cmath>
#include <imgui/imgui.h>
#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <nfd.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Renderer.h"
#include "Scene.h"
#include "Utils.h"
#include <iostream>
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


int main(int argc, char** argv)
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

	std::string filePath = "C:/Haifa_University/ComputerGraphics/Data/banana.obj";
	std::cout << filePath << std::endl;
	std::shared_ptr<MeshModel> banana = Utils::LoadMeshModel(filePath);
	//banana->scale(3000);
	//banana->move(500, 300);
	std::cout << *banana << std::endl;
	scene.AddModel(banana);


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
	MeshModel& curr = scene.GetModel(0);
	if (frameBufferWidth != renderer.GetViewportWidth() || frameBufferHeight != renderer.GetViewportHeight())
	{
		// TODO: Set new aspect ratio
	}
	if (!io.WantCaptureMouse)
	{
		if (io.MouseDown[0]) // First method of transforming the object - by mouse drag and mouse clicks
		{
			curr.translateWorld.x = -curr.translateLocal.x  + io.MousePos.x;
			curr.translateWorld.y = -curr.translateLocal.y + (renderer.GetViewportHeight() - io.MousePos.y);
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
	MeshModel& curr = scene.GetModel(0);
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);
	{
		ImGui::Begin("World controls");
		ImGui::Text("WORLD");
		ImGui::SliderFloat("Scale X by", &curr.scaleWorld.x, 0.0f, 2.0f);
		ImGui::SliderFloat("Scale Y by", &curr.scaleWorld.y, 0.0f, 2.0f);
		ImGui::SliderFloat("Scale Z by", &curr.scaleWorld.z, 0.0f, 2.0f);
		ImGui::SliderFloat("Rotate X by", &curr.rotateWorld.x, 0.0f, 360.0f);
		ImGui::SliderFloat("Rotate Y by", &curr.rotateWorld.y, 0.0f, 360.0f);
		ImGui::SliderFloat("Rotate Z by", &curr.rotateWorld.z, 0.0f, 360.0f);
		ImGui::SliderFloat("Translate X by", &curr.translateWorld.x, 0.0f, 2000.0f);
		ImGui::SliderFloat("Translate Y by", &curr.translateWorld.y, 0.0f, 2000.0f);
		ImGui::SliderFloat("Translate Z by", &curr.translateWorld.z, 0.0f, 2000.0f);
		ImGui::End();
	}
	static bool worldFlag = false, localFlag = false;
	{
		ImGui::Begin("Keyboard controls");
		ImGui::Text("Chose World or Local transformation to control from keyboard");
		if(ImGui::Checkbox("World", &worldFlag))
			localFlag = false;
		if(ImGui::Checkbox("Local", &localFlag))
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

		if (!io.WantCaptureKeyboard) // Second method of transforming the model using the keyboard
		{
			if (localFlag)
			{
				if (io.KeysDown['W'] || io.KeysDown['w'])
				{
					curr.translateLocal.y += 5;
				}
				if (io.KeysDown['S'] || io.KeysDown['s'])
				{
					curr.translateLocal.y -= 5;
				}
				if (io.KeysDown['A'] || io.KeysDown['a'])
				{
					curr.translateLocal.x -= 5;
				}
				if (io.KeysDown['D'] || io.KeysDown['d'])
				{
					curr.translateLocal.x+= 5;
				}
				if (io.KeysDown['X'] || io.KeysDown['x'])
				{
					curr.rotateLocal.x >= 360 ? curr.rotateLocal.x -= 360 : curr.rotateLocal.x;
					curr.rotateLocal.x += 5;
				}
				if (io.KeysDown['Y'] || io.KeysDown['y'])
				{
					curr.rotateLocal.y >= 360 ? curr.rotateLocal.y -= 360 : curr.rotateLocal.y;
					curr.rotateLocal.y += 5;
				}
				if (io.KeysDown['Z'] || io.KeysDown['z'])
				{
					curr.rotateLocal.z >= 360 ? curr.rotateLocal.z -= 360 : curr.rotateLocal.z;
					curr.rotateLocal.z += 5;
				}
				if (io.KeysDown[264]) // DOWN Arrow
				{
					curr.scaleLocal.x -= 5;
					curr.scaleLocal.y -= 5;
					curr.scaleLocal.z -= 5;
				}
				if (io.KeysDown[265]) // UP Arrow
				{
					curr.scaleLocal.x += 5;
					curr.scaleLocal.y += 5;
					curr.scaleLocal.z += 5;
				}
			}
			else if (worldFlag)
			{
				if (io.KeysDown['W'] || io.KeysDown['w'])
				{
					curr.translateWorld.y += 5;
				}
				if (io.KeysDown['S'] || io.KeysDown['s'])
				{
					curr.translateWorld.y -= 5;
				}
				if (io.KeysDown['A'] || io.KeysDown['a'])
				{
					curr.translateWorld.x -= 5;
				}
				if (io.KeysDown['D'] || io.KeysDown['d'])
				{
					curr.translateWorld.x += 5;
				}
				if (io.KeysDown['X'] || io.KeysDown['x'])
				{
					curr.rotateWorld.x >= 360 ? curr.rotateWorld.x -= 360 : curr.rotateWorld.x;
					curr.rotateWorld.x += 5;
				}
				if (io.KeysDown['Y'] || io.KeysDown['y'])
				{
					curr.rotateWorld.y >= 360 ? curr.rotateWorld.y -= 360 : curr.rotateWorld.y;
					curr.rotateWorld.y += 5;
				}
				if (io.KeysDown['Z'] || io.KeysDown['z'])
				{
					curr.rotateWorld.z >= 360 ? curr.rotateWorld.z -= 360 : curr.rotateWorld.z;
					curr.rotateWorld.z += 5;
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
		static float f = 0.0f;
		static int counter = 0;
		ImGui::Begin("Local controls");
		ImGui::Text("LOCAL");
		ImGui::SliderFloat("Scale X by", &curr.scaleLocal.x, 0.0f, 5000.0f);
		ImGui::SliderFloat("Scale Y by", &curr.scaleLocal.y, 0.0f, 5000.0f);
		ImGui::SliderFloat("Scale Z by", &curr.scaleLocal.z, 0.0f, 5000.0f);
		ImGui::SliderFloat("Rotate X by", &curr.rotateLocal.x, 0.0f, 360.0f);
		ImGui::SliderFloat("Rotate Y by", &curr.rotateLocal.y, 0.0f, 360.0f);
		ImGui::SliderFloat("Rotate Z by", &curr.rotateLocal.z, 0.0f, 360.0f);
		ImGui::SliderFloat("Translate X by",&curr.translateLocal.x, 0.0f, 2000.0f);
		ImGui::SliderFloat("Translate Y by", &curr.translateLocal.y, 0.0f, 2000.0f);
		ImGui::SliderFloat("Translate Z by", &curr.translateLocal.z, 0.0f, 2000.0f);
		ImGui::End();
		
		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
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