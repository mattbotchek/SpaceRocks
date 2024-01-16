// Sprite.cpp - 2D display and manipulate texture-mapped quad

#include <glad.h>
#include <GLFW/glfw3.h>
#include "Draw.h"
#include "GLXtras.h"
#include "Sprite.h"
#include "mmsystem.h"

Sprite background, actor1, actor2;
bool collision = false;

// Mouse

void MouseWheel(float spin) { actor1.Wheel(spin); }

void MouseButton(float x, float y, bool left, bool down) { if (left && down) actor1.Down(x, y); }

void MouseMove(float x, float y, bool leftDown, bool rightDown) {
	if (leftDown) {
		actor1.Drag(x, y);
		collision = actor1.Intersect(actor2);
		if (collision)
			PlaySoundA("C:/Assets/Sounds/Mario7.wav", NULL, SND_ASYNC | SND_NODEFAULT | SND_NOSTOP);
	}
}

// Display

void Outline(Sprite s, float width = 2, vec3 color = vec3(1,1,0)) {
	UseDrawShader(mat4());
	vec2 pts[] = { s.PtTransform({-1,-1}), s.PtTransform({-1,1}), s.PtTransform({1,1}), s.PtTransform({1,-1}) };
	for (int i = 0; i < 4; i++)
		Line(pts[i], pts[(i+1)%4], width, color);
}

void Display() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	background.Display();
	actor1.Display();
	actor2.Display();
	if (collision) {
		Outline(actor1);
		Outline(actor2);
	}
	glFlush();
}

// Application

void Resize(int width, int height) { glViewport(0, 0, width, height); }

int main(int ac, char **av) {
	GLFWwindow *w = InitGLFW(100, 100, 600, 600, "Sprite Demo");
	// read background and two foreground sprites
	background.Initialize("C:/Assets/Images/Earth.tga");
	actor1.Initialize("C:/Assets/Images/DNA-32bit.png", 0);
	actor1.SetScale(.3f);
	actor1.SetPosition(vec2(-.3f, 0.f));
	actor2.Initialize("C:/Assets/Images/Mandrill32bit.tga", 1);
	actor2.SetScale(.3f);
	actor2.SetPosition(vec2(.3f, 0.f));
	// callbacks
	RegisterMouseButton(MouseButton);
	RegisterMouseMove(MouseMove);
	RegisterMouseWheel(MouseWheel);
	RegisterResize(Resize);
	printf("mouse drag to move sprite\n");
	// event loop
	while (!glfwWindowShouldClose(w)) {
		Display();
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
}
