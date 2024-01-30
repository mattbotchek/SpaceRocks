#include <glad.h>
#include <GLFW/glfw3.h>
#include "Draw.h"
#include "GLXtras.h"
#include "Sprite.h"
#include <cmath>

Sprite background, actor;
bool hovering = false;

// interaction
int		key = 0;
time_t	keydownTime = 0;

// Mouse

void MouseWheel(float spin) {
	actor.Wheel(spin);
}

void MouseButton(float x, float y, bool left, bool down) {
	if (left && down) {
		if (actor.Hit(x, y))
			printf("I've been hit!\n");
		actor.Down(x, y);
	}
}

void MouseMove(float x, float y, bool leftDown, bool rightDown) {
	hovering = actor.Hit(x, y);
	if (leftDown)
		actor.Drag(x, y);
}

// Application

void ApplyGravity(float gravity)
{
	actor.SetPosition(actor.position + vec2((float)0.0, gravity));
}

void MoveActor(float speed) {
	// apply rotation for "forward" movement not square movement

	// Wonky AF and needs to be fixed, looking into the math LOL
	float dx, dy;
	dx = speed * cos(actor.rotation);
	dy = speed * sin(actor.rotation);

	actor.SetPosition(actor.position + vec2(dx, dy));
}

void RotateActor(float angle)
{
	cout << actor.rotation << endl;
	actor.SetRotation(actor.rotation + angle);
}

// Movement

void TestKey()
{
	float d = .005f;
	float roationalSpeed = 0.5;
	if (key == GLFW_KEY_LEFT) RotateActor(roationalSpeed);
	if (key == GLFW_KEY_RIGHT) RotateActor(-roationalSpeed);
	if (key == GLFW_KEY_DOWN) MoveActor(-d);
	if (key == GLFW_KEY_UP) MoveActor(d);
}

void CheckUser() {
	if (KeyDown(key) && (float)(clock() - keydownTime) / CLOCKS_PER_SEC > .01f)
		TestKey();
}

void Keyboard(int k, bool press, bool shift, bool control) {
	if (press) 
	{
		key = k;
		keydownTime = clock();
		TestKey();
	}
}

void StartGravity()
{
	float gravity = -0.0005;
	ApplyGravity(gravity);
}

void Resize(int width, int height) {
	glViewport(0, 0, width, height);
}

void Display() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	background.Display();
	actor.Display();
	if (hovering)
		actor.Outline(vec3(1, 1, 0));
	glFlush();
}

int main(int ac, char** av) {
	GLFWwindow* w = InitGLFW(100, 100, 600, 600, "Sprite Demo");
	// read background, foreground
	// Temp Image of earth I ripped from some site, meaning to find again but google history aint helpful. -Matt
	background.Initialize("C:/repos/SpaceRocks/SpaceRocks/Assets/Images/earth.tga");

	// Shuttle image from same site, temporary until we can make our own
	actor.Initialize("C:/repos/SpaceRocks/SpaceRocks/Assets/Images/shuttle.tga");
	actor.SetScale(vec2(.4f, .4f));
	// callbacks
	RegisterMouseButton(MouseButton);
	RegisterMouseMove(MouseMove);
	RegisterMouseWheel(MouseWheel);
	RegisterResize(Resize);
	RegisterKeyboard(Keyboard);
	printf("drag mouse to move sprite; mouse wheel changes size\n");
	// event loop
	while (!glfwWindowShouldClose(w)) {
		CheckUser();
		Display();
		StartGravity();
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
}
