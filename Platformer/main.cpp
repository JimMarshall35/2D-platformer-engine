
#include <iostream>
#include "Engine.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "ErrorHandling.h"
#include "JSONLevelSerializer.h"
#include "LuaLevelSerializer.h"
#include "EditorUI.h"

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

	glClearColor(0.5294, 0.8078, 0.9216, 1.0);
	double last = glfwGetTime();
	Engine engine(new EditorUserInterface(), new LuaLevelSerializer());
	engine_ptr = &engine;
	inputInit(window);
	imguiInit(window);

	
	
	const ImGuiIO& io = ImGui::GetIO(); (void)io;
	

	while (!glfwWindowShouldClose(window))
	{
		double now = glfwGetTime();
		double delta = now - last;
		last = now;

		engine.Update(delta);

		wantMouseInput = io.WantCaptureMouse;
		wantKeyboardInput = io.WantCaptureKeyboard;

		glfwPollEvents();

		

		glClear(GL_COLOR_BUFFER_BIT);
		engine.Draw();
		
		
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();
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