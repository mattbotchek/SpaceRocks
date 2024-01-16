// PumpItUp.cpp: animated flow

#include <glad.h>
#include <GLFW/glfw3.h>
#include <time.h>
#include "Camera.h"
#include "Draw.h"
#include "GLXtras.h"
#include "Mesh.h"
#include "Text.h"

// Window, Camera, Lights, Object, Colors
int		winW = 800, winH = 800;
Camera	camera(0, 0, winW, winH, vec3(0, 0, 0), vec3(0, 0, -.3f));
vec3	lights[] = { {1.8f, -.4f, .45f}, {-2.2f, -.4f, .45f}, {.02f, 2.0f, .85f} };
Mesh	sphere;

// Options
bool	animate = true;			// move spheres along path?
bool	fixCorner = true;		// don't stretch if causes overshoot at corner
bool	stretch = true;			// otherwise, symmetrical sphere
float	stretchFactor = 3;		// elongate in direction of motion
float	multiple = 1;			// multiply number of spheres

// Spheres
float	sphereRadius = .0015f;
float	sphereTravelTime = 3;											// seconds for a sphere to travel path
int		nSpheres = 0, maxNSpheres = 100;								// # spheres currently in path

// Timing
time_t	prevTime = 0;
float	accumTime = 0;

// Path

float Length(vec3 *pts, int nPts) {
	float len = 0;
	for (int i = 1; i < nPts; i++)
		len += length(pts[i]-pts[i-1]);
	return len;
}

vec3	points[] = { {0, .5, .5},  {.5, .5, .5},  {.5, -.5, .5}, {.5, -.5, -.5},
					 {.5, .5, -.5}, {0, .5, -.5}, {0, -.5, -.5}, {0, -.5, .5} };
int		nPoints = sizeof(points)/sizeof(vec3);
float	pathLength = Length(points, nPoints);

vec3 Point(float t, vec3 &p1, vec3 &p2) {
	float d = t*pathLength, accumD = 0;
	for (int i = 1; i < nPoints; i++) {
		p1 = points[i-1];
		p2 = points[i];
		float len = length(p2-p1);
		if (accumD+len > d)
			return p1+((d-accumD)/len)*(p2-p1);
		accumD += len;
	}
	return vec3();
}

// Display

void Animate() {
	accumTime += animate? (float)(clock()-prevTime)/CLOCKS_PER_SEC : 0;
	prevTime = clock();
	// set # spheres on path proportional to accumulated time
	float f = accumTime/sphereTravelTime, ff = f > 1? 1 : f;
	nSpheres = (int) (multiple*maxNSpheres*ff);
}

void DisplaySpheres(float accumTime) {
	GLuint program = UseMeshShader();
	float dt = sphereTravelTime/(multiple*maxNSpheres);
	SetUniform(program, "color", vec3(1, 0, 0));
	for (int i = 0; i < nSpheres; i++) {
		float time = accumTime-i*dt;
		if (time >= 0) {
			float t = (float) fmod(time/sphereTravelTime, 1);
			vec3 p1, p2, p = Point(t, p1, p2);							// p is center of sphere, p1p2 contains p
			vec3 scale(sphereRadius);
			if (stretch && stretchFactor > 1) {
				float f = stretchFactor*sphereRadius;
				float dp1 = length(p-p1), dp2 = length(p-p2);			// should be in native "tube" space (ie, +/-1)
				if (!fixCorner || (dp1 > 5*f && dp2 > 5*f)) {		// stretch each sphere in direction of travel
					vec3 d(p2-p1), a(abs(d.x), abs(d.y), abs(d.z));		// (only principal axes) *** why 5?
					float *s = a.x>a.y? (a.x>a.z? &scale.x : &scale.z)
							   : a.y>a.z? &scale.y : &scale.z;
					*s = f;
				}
			}
			sphere.toWorld = Translate(.1f*p)*Scale(scale);				// move sphere to p, scale with possible stretch
			sphere.Display(camera);
		}
	}
}

void Display() {
	glClearColor(.4f, .8f, .8f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	GLuint program = UseMeshShader();
	const int nLights = sizeof(lights)/sizeof(vec3);
	vec3 xLights[nLights];
	for (int i = 0; i < nLights; i++)
		xLights[i] = Vec3(camera.modelview*vec4(lights[i], 1));
	SetUniform(program, "nLights", nLights);
	SetUniform3v(program, "lights", nLights, (float *) xLights);
	DisplaySpheres(accumTime);
	glDisable(GL_DEPTH_TEST);
	UseDrawShader(camera.fullview);
	if (camera.down && !camera.shift)
		camera.arcball.Draw(camera.control);
	Text(50, 50, vec3(0, 0, 0), 12, "stretchFactor: %3.2f", stretchFactor);
	Text(50, 80, vec3(0, 0, 0), 12, "multiple: %3.2f", multiple);
	glFlush();
}

// Mouse Handlers

void MouseButton(float x, float y, bool left, bool down) {
	if (left && down)
		camera.Down(x, y, Shift(), Control());
	if (!down)
		camera.Up();
}

void MouseMove(float x, float y, bool leftDown, bool rightDown) {
	if (leftDown)
		camera.Drag(x, y);
}

void MouseWheel(float spin) {
	camera.Wheel(spin);
}

// Application

void Keyboard(int key, bool press, bool shift, bool control) {
	if (press) {
		if (key == 'A') animate = !animate;
		if (key == 'C') fixCorner = !fixCorner;
		if (key == 'S') stretch = !stretch;
		if (key == 'F')	stretchFactor *= shift? .8f : 1.3f;
		if (key == 'T') {
			multiple += shift? -1 : 1;
			multiple = multiple < 1? 1 : multiple > 6? 6 : multiple;
		}
	}
}

void Resize(int width, int height) {
	camera.Resize(width, height);
	glViewport(0, 0, width, height);
}

const char *usage = R"(
	A: toggle animate
	C: toggle corner correct
	S: toggle stretch
	F: inc/dec stretch factor
	T: inc/dec multiple
)";

int main(int argc, char** argv) {
	GLFWwindow* w = InitGLFW(100, 100, winW, winH, "Pump It Up");
	sphere.Read("C:/Assets/Models/Sphere.obj");
	RegisterMouseButton(MouseButton);
	RegisterMouseMove(MouseMove);
	RegisterMouseWheel(MouseWheel);
	RegisterResize(Resize);
	RegisterKeyboard(Keyboard);
	printf("usage:%s", usage);
	while (!glfwWindowShouldClose(w)) {
		Animate();
		Display();
		glfwPollEvents();
		glfwSwapBuffers(w);
	}
	glfwDestroyWindow(w);
	glfwTerminate();
}
