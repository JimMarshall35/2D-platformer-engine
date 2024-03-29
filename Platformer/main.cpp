
#include <iostream>
#include "Engine.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "ErrorHandling.h"
#include "JSONLevelSerializer.h"
#include "LuaLevelSerializer.h"
#include "LuaVMService.h"
#include "EditorUI.h"
#include "Renderer2D.h"
#include "PortAudioPlayer.h"
#include "BetterRenderer.h"

static bool wantMouseInput = false;
static bool wantKeyboardInput = false;


void framebuffer_size_callback(GLFWwindow* window, int newwidth, int newheight);
void imguiInit(GLFWwindow* window);
static void processInput(GLFWwindow* window, float delta);
static void inputInit(GLFWwindow* window);
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

Engine* engine_ptr; 
int main(int argc, char* argv[])
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(800, 600, "Marching cubes Demo", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600);
	//glEnable(GL_DEPTH_TEST);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glClearColor(0.5294, 0.8078, 0.9216, 1.0); // sky blue
	double last = glfwGetTime();
	
	
	// Note: these two lines are NOT the same one accidentally repeated!! READ CAREFULLY
	inputInit(window);
	imguiInit(window);
	
	
	const ImGuiIO& io = ImGui::GetIO(); (void)io;

	LuaVMService* vmService = new LuaVMService();
	Engine engine(
		new EditorUserInterface(vmService),
		new LuaLevelSerializer(vmService),
		new Renderer2D(),
		new PortAudioPlayer()
	);
	engine_ptr = &engine;

	//BetterTileset bt;
	
	//bt.LoadTilesetFile(48, 48, "C:\\Users\\james.marshall\\source\\repos\\Platformer\\Platformer\\batch1.png");

	
	//bt.BuildAtlas();
	//BetterRenderer br;
	//br.WindowH = 600;
	//br.WindowW = 800;
	std::chrono::duration<double> frameTime(0.0);
	std::chrono::duration<double> sleepAdjust(0.0);
	while (!glfwWindowShouldClose(window))
	{
#ifndef SEPARATE_PHYSICS_THREAD


		std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
		engine.Box2dContext.Step();
#endif // SEPARATE_PHYSICS_THREAD

		

		double now = glfwGetTime();
		double delta = now - last;
		last = now;
		wantMouseInput = io.WantCaptureMouse;
		wantKeyboardInput = io.WantCaptureKeyboard;

		glfwPollEvents();

		engine.Update(delta);
		glClear(GL_COLOR_BUFFER_BIT);
		engine.Draw();
		//br.DrawAtlasTexture(glm::vec2(0), glm::vec2(16, 16), 0, 0, engine._GameCam);
		//br.DrawAtlasTexture(glm::vec2(0,0), glm::vec2(128,128), 0, 30, engine._GameCam);
		//br.DrawAtlasTexture(glm::vec2(-30,-30), glm::vec2(16, 16), 0, 2, engine._GameCam);
		
		glfwSwapBuffers(window);
#ifndef SEPARATE_PHYSICS_THREAD

		// Throttle to cap at 60Hz. This adaptive using a sleep adjustment. This could be improved by
		// using mm_pause or equivalent for the last millisecond.
		std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
		std::chrono::duration<double> target(1.0 / 60.0);
		std::chrono::duration<double> timeUsed = t2 - t1;
		std::chrono::duration<double> sleepTime = target - timeUsed + sleepAdjust;
		if (sleepTime > std::chrono::duration<double>(0))
		{
			std::this_thread::sleep_for(sleepTime);
		}

		std::chrono::steady_clock::time_point t3 = std::chrono::steady_clock::now();
		frameTime = t3 - t1;

		// Compute the sleep adjustment using a low pass filter
		sleepAdjust = 0.9 * sleepAdjust + 0.1 * (target - frameTime);
#endif // SEPARATE_PHYSICS_THREAD

	}

	glfwDestroyWindow(window);
	glfwTerminate();
	delete vmService;
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int newwidth, int newheight)
{
	engine_ptr->FrameBufferSizeCallbackHandler(window, newwidth, newheight);
}

void imguiInit(GLFWwindow* window)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");
}




static void processInput(GLFWwindow* window, float delta)
{
	if (!wantKeyboardInput) {

	}
}
static void inputInit(GLFWwindow* window) {
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetKeyCallback(window, key_callback);
}
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	engine_ptr->CursorPosCallbackHandler(xpos, ypos, wantMouseInput);//CursorPositionCallbackHandler(xpos, ypos, wantMouseInput, camera);

}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	engine_ptr->ScrollCallbackHandler(window, xoffset, yoffset, wantMouseInput);//ui_ptr->scrollCallbackHandler(window, xoffset, yoffset, wantMouseInput,camera);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	engine_ptr->MouseButtonCallbackHandler(button, action, mods, wantMouseInput);//ui_ptr->mouseButtonCallbackHandler(button, action, mods, wantMouseInput, camera);
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
	engine_ptr->KeyBoardButtonCallbackHandler(window, key, scancode, action, mods, wantKeyboardInput);//ui_ptr->keyboardButtonCallbackHandler(window, key, scancode, action, mods, wantKeyboardInput);
}