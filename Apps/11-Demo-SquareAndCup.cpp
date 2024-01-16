// SquareAndCup.cpp

#include <glad.h>
#include <glfw3.h>
#include "Camera.h"
#include "Draw.h"
#include "GLXtras.h"
#include "IO.h"
#include "Mesh.h"
#include "Text.h"

// application display parameters
int			winWidth = 700, winHeight = 700;
Camera		camera(0, 0, winWidth, winHeight, vec3(20,7,0), vec3(0,0,-6));

// meshes
Mesh		square, cup;

// Display
vec3		lights[] = { {1, 1, .2f}, {.5f, .2f, 1} };
const int	nLights = sizeof(lights)/sizeof(vec3);
bool		annotate = true;

void Display(GLFWwindow *w) {
	glClearColor(.5f, .5f, .5f, 1);						// set background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// clear background and z-buffer
	glEnable(GL_DEPTH_TEST);							// see only nearest surface
	int shader = UseMeshShader();
	vec3 xLights[nLights];
	for (int i = 0; i < nLights; i++)
		xLights[i] = Vec3(camera.modelview*vec4(lights[i]));
	SetUniform(shader, "nLights", nLights);
	SetUniform3v(shader, "lights", nLights, (float *) xLights);
	// draw meshes
	square.Display(camera, 0);
	cup.Display(camera, 1);
	if (annotate) {
		// draw lights
		UseDrawShader(camera.fullview);
		for (int i = 0; i < nLights; i++)
			Disk(lights[i], 12, vec3(1, 1, 1));
		// draw grid
		float z = -.01f;
		mat4 m = camera.fullview*square.toWorld;
		UseDrawShader(m);
		// lines
		int nLines = 10;
		for (int i = 0; i <= nLines; i++) {
			float t = (float) i/nLines;
			float a = -1+2*t;
			Line(vec3(a, -1.f, z), vec3(a, 1.f, z), 2, vec3(0, 0, 1));
			Line(vec3(-1.f, a, z), vec3(1.f, a, z), 2, vec3(0, 1, 0));
		}
		// indices
		for (int i = 0; i < nLines; i++) {
			float fi = (float) (i+.3f)/(nLines);
			for (int j = 0; j < nLines; j++) {
				float fj = (float) (j+.3f)/(nLines);
				vec3 p(-1+fi*2, -1+fj*2, z);
				Text(p, m, vec3(1, 1, 0), 10, "%i,%i", i, j);
			}
		}
	}
	glFlush();
}

// Scene

void MakeScene() {
	string modelsDir = "C:/Assets/Models/";
	string imagesDir = "C:/Assets/Images/";
	square.Read(modelsDir+"Square.obj", imagesDir+"Lily.tga");
	square.toWorld = RotateX(90);
	cup.Read(modelsDir+"Teacup.obj", imagesDir+"Parrots.tga");
	cup.toWorld = RotateY(30)*Scale(.3f)*Translate(0, .51f, 0);
}

// Callbacks

void MouseButton(float x, float y, bool left, bool down) {
	if (down)
		camera.Down(x, y, Shift()); else camera.Up();
}

void MouseMove(float x, float y, bool leftDown, bool rightDown) {
	if (leftDown)
		camera.Drag(x, y);
}

void MouseWheel(float spin) {
	camera.Wheel(spin, Shift());
}

void Resize(int width, int height) {
	camera.Resize(width, height);
	glViewport(0, 0, width, height);
}

// Application

void Keyboard(int key, bool press, bool shift, bool control) {
	if (press && key == 'A')
		annotate = !annotate;
}

const char *usage = R"(
	mouse-drag:  rotate
	with shift:  translate xy
	mouse-wheel: translate z
)";

int main(int ac, char **av) {
	// create app window, read mesh
	GLFWwindow *w = InitGLFW(100, 100, winWidth, winHeight, "Square And Cup");
	MakeScene();
	// callbacks
	RegisterMouseMove(MouseMove);
	RegisterMouseButton(MouseButton);
	RegisterMouseWheel(MouseWheel);
	RegisterKeyboard(Keyboard);

	RegisterResize(Resize);
	printf("Usage: %s", usage);
	// event loop
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(w)) {
		Display(w);
		glfwPollEvents();
		glfwSwapBuffers(w);
	}
}
