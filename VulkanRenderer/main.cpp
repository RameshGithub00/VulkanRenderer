#include "Renderer.h"

#include <GLFW/glfw3.h>

#pragma comment(lib, "glfw3.lib")

GLFWwindow* window;

void initWIndow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
	Renderer::getPtr()->init();
}

void initVulkan()
{

}

void initInstance()
{

}

void mainLoop()
{

}

void cleanUp()
{

}


int main()
{
	Renderer::getPtr();
	initWIndow();


	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
}