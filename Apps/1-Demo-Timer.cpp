// HealthBar.cpp

#include <glad.h>
#include <GLFW/glfw3.h>
#include <time.h>
#include "Draw.h"
#include "GLXtras.h"
#include "Sprite.h"
#include "Widgets.h"

Sprite background;

class Circle {
public:
	Timer timer;
	vec2 center;
	float radius = .5f, radiansPerSec = 3.1415f/2;
	void Display() {
		float radians = timer.Elapsed()*radiansPerSec;
		vec2 p = center+radius*vec2(cos(radians), sin(radians));
		UseDrawShader(mat4(1));
		Line(center, p, 2, vec3(1, 0, 1));
		Disk(center, 6, vec3(1, 0, 1));
		Disk(p, 16, vec3(1, 0, 1));
	}
	Circle(float x, float y, float r) : center(x, y), radius(r) { }
};

class Bar {
public:
	Timer timer;
	int x = 0, y = 0, w = 0;		// location and size, in pixels
	int hStart = 0, hStop = 0;		// range of height of animated bar, in pixels
	void Display(vec3 color = vec3(1, 0, 0)) {
		float t = timer.Progress();
		int h = (int) ((1-t)*hStart+t*hStop);
		UseDrawShader(ScreenMode());
		Quad(x, y, x+w, y, x+w, y+h, x, y+h, true, color);
	}
	Bar(int x, int y, int w, int start, int stop) : x(x), y(y), w(w), hStart(start), hStop(stop) { timer.duration = .5f; }
};

Bar bar(100, 100, 50, 0, 200);
Circle circle(.2f, 0, .5f);

void Display() {
	background.Display();
	bar.Display();
	circle.Display();
	glFlush();
}

void Resize(int width, int height) {
	glViewport(0, 0, width, height);
}

void Keyboard(int key, bool press, bool shift, bool control) {
	if (press) {
		if (key == 'A')
			bar.timer.Reset();
		if (key == 'P')
			circle.timer.Pause();
		if (key == 'R')
			circle.timer.Resume();
	}
}

const char *usage = R"(
	A: animate health bar
	P: pause disk
	R: resume disk
)";

int main() {
	GLFWwindow *w = InitGLFW(100, 100, 600, 600, "Health Bar Demo");
	background.Initialize("C:/Assets/Images/Earth.tga");
	RegisterResize(Resize);
	RegisterKeyboard(Keyboard);
	printf("Usage:%s\n", usage);
	while (!glfwWindowShouldClose(w)) {
		Display();
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
}
