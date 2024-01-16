// SpriteGIF.cpp

#include <glad.h>
#include <GLFW/glfw3.h>
#include "GLXtras.h"
#include "Sprite.h"

Sprite background;

void Display() {
	background.Display();
	glFlush();
}

void Resize(int width, int height) {
	glViewport(0, 0, width, height);
}

int main(int ac, char **av) {
	GLFWwindow *w = InitGLFW(100, 100, 1000, 450, "Sprite GIF");
	background.InitializeGIF("C:/Assets/Images/FlamingBackground.gif");
//	background.frameDuration = .125f;
	RegisterResize(Resize);
	while (!glfwWindowShouldClose(w)) {
		Display();
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
}
