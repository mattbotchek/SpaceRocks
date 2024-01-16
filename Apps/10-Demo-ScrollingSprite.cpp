// 10-Demo-ScrollingSprite.cpp

#include <glad.h>
#include <GLFW/glfw3.h>
#include <time.h>
#include "GLXtras.h"
#include "IO.h"
#include "Sprite.h"

// Application Variables

int		winWidth = 1000, winHeight = 1000;

Sprite	background;
string	scrollImage = "C:/Assets/Images/SeamlessPine.png";
bool	scrolling = true, vertically = true;

float	loopDuration = 2, accumulatedVTime = 0, accumulatedHTime = 0;
time_t	scrollTime = clock();

// Display

void Scroll() {
	time_t now = clock();
	if (scrolling) {
		float dt = (float)(now-scrollTime)/CLOCKS_PER_SEC;
		(vertically? accumulatedVTime : accumulatedHTime) += dt;
	}
	scrollTime = now;
	float v = accumulatedVTime/loopDuration, u = accumulatedHTime/loopDuration;
	background.uvTransform = Translate(u, v, 0);
}

void Display() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	background.Display();
	glFlush();
}

// Application

void Keyboard(int key, bool press, bool shift, bool control) {
	if (press) {
		if (key == 'S') scrolling = !scrolling;
		if (key == 'V') vertically = true;
		if (key == 'H') vertically = false;
	}
}

void Resize(int width, int height) {
	glViewport(0, 0, winWidth = width, winHeight = height);
}

const char* usage = R"(
	S: vertical-scrolling on/off
	V: scroll vertically
	H: scroll horizontally
)";

int main(int ac, char **av) {
	// init app window and GL context
	GLFWwindow *w = InitGLFW(100, 100, winWidth, winHeight, "Sprite Stepback");
	background.Initialize(scrollImage, .7f);
	// callbacks
	RegisterKeyboard(Keyboard);
	RegisterResize(Resize);
	printf("Usage: %s\n", usage);
	// event loop
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(w)) {
		Scroll();
		Display();
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
	// terminate
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glfwDestroyWindow(w);
	glfwTerminate();
}
