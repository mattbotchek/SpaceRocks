// MeshAndSpritesApp.cpp -  display of 2D sprites and 3D meshes

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <time.h>
#include "Camera.h"
#include "Draw.h"
#include "GLXtras.h"
#include "Mesh.h"
#include "Misc.h"
#include "Sprite.h"
#include "VecMat.h"

using std::string;

// sprites
Sprite ground, billboard;

// display
int winW = 600, winH = 600;
Camera camera(0, 0, winW, winH, vec3(0,0,0), vec3(0,0,-5));

// Mouse

void MouseButton(float x, float y, bool left, bool down) {
	if (left && down)
		camera.Down(x, y, Shift());
	if (!down)
		camera.Up();
}

void MouseMove(float x, float y, bool leftDown, bool rightDown) {
	if (leftDown)
		camera.Drag(x, y);
}

void MouseWheel(float spin) {
	camera.Wheel(spin, Shift());
}

// Display

void Display() {
	glClearColor(1, 1, 1, 1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	ground.Display(&camera.fullview);
	billboard.Display(&camera.fullview);
	glFlush();
}

// Application

string dirImages = "C:/Assets/Images/";
string dirMeshes = "C:/Assets/Models/";

void ReadSprites() {
	ground.Initialize(dirImages+"Earth.tga");
	ground.ptTransform = RotateX(90);
	billboard.Initialize(dirImages+"Parrots.tga");
	billboard.ptTransform = Scale(vec3(.5f));
}

void Resize(int width, int height) {
	camera.Resize(width, height);
	glViewport(0, 0, winW = width, winH = height);
}

int main(int ac, char **av) {
	// init app window and GL context
	GLFWwindow *w = InitGLFW(100, 100, winW, winH, "Mesh And Sprites");
	// init characters
	ReadSprites();
	// callbacks
	RegisterMouseButton(MouseButton);
	RegisterMouseMove(MouseMove);
	RegisterMouseWheel(MouseWheel);
	RegisterResize(Resize);
	// event loop
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(w)) {
		Display();
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
	// terminate
	glfwDestroyWindow(w);
	glfwTerminate();
}
