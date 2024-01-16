// RoomWithLights.cpp

#include <glad.h>
#include <glfw3.h>
#include "Camera.h"
#include "Draw.h"
#include "GLXtras.h"
#include "IO.h"
#include "Mesh.h"

// application display parameters
int		winWidth = 700, winHeight = 700;
Camera	camera(0, 0, winWidth, winHeight, vec3(0,7,0), vec3(0,0,-6));
vec3	light(0, 0, 0);

// meshes
Mesh	room, bench;

// Display

void Display(GLFWwindow *w) {
	glClearColor(.5f, .5f, .5f, 1);						// set background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// clear background and z-buffer
	glEnable(GL_DEPTH_TEST);							// see only nearest surface
	int shader = UseMeshShader();
	SetUniform(shader, "defaultLight", light);
	SetUniform(shader, "twoSidedShading", false);
	room.Display(camera);
	SetUniform(shader, "twoSidedShading", true);
	bench.Display(camera, 0);
	UseDrawShader();
	if (IsVisible(light, camera.fullview))
		Disk(light, 12, vec3(1, 1, 1));
	/* test front-facing
	glDisable(GL_DEPTH_TEST);
	mat4 vp = Viewport(), m = vp*camera.fullview*room.toWorld;
	int4 q = room.quads[1];
	vec3 p1(room.points[q.i1]), p2(room.points[q.i2]), p3(room.points[q.i3]), p4(room.points[q.i4]);
	vec3 n1(room.normals[q.i1]), n2(room.normals[q.i2]), n3(room.normals[q.i3]), n4(room.normals[q.i4]);
	vec3 c((p1+p2+p3+p4)/4), N = normalize(n1+n2+n3+n4);
	vec4 xN = m*vec4(N.x, N.y, N.z, 0);
	vec4 x1 = m*vec4(p1), x2 = m*vec4(p2), x3 = m*vec4(p3);
	vec2 xx1(x1.x/x1.w, x1.y/x1.w), xx2(x2.x/x2.w, x2.y/x2.w), xx3(x3.x/x3.w, x3.y/x3.w);
	float x = cross(xx2-xx1, xx3-xx2);
	bool primitiveFrontFacing = x > 0, normalFrontFacing = xN.z < 0;
	UseDrawShader(camera.fullview);
	// Disk(c, 24, primitiveFrontFacing? vec3(0,1,0) : vec3(1,0,0));
	// test sidedness
	vec3 L = normalize(light-c);
	float dd = dot(L, N);
	bool lightFrontFacing = dd > 0;
	ArrowV(c, .3f*N, camera.modelview, camera.persp, primitiveFrontFacing? vec3(1,0,1) : vec3(0,1,1));
	ArrowV(c, .3f*L, camera.modelview, camera.persp, lightFrontFacing? vec3(1,1,0) : vec3(0,0,0));
	bool sideViewer = primitiveFrontFacing && normalFrontFacing;
	bool sideLight = dd > 0;
	Disk(c, 20, sideViewer == sideLight? vec3(0,1,0) : vec3(1,0,0)); */
	glFlush();											// finish
}

// Scene

void MakeScene() {
	// read circle for target display
	string boxFile = "C:/Assets/Models/OpenBox.obj";
	string benchFile = "C:/Assets/Models/Bench.obj";
	string benchTexture = "C:/Assets/Images/Bench.tga";
	room.Read(boxFile);
	room.toWorld = Scale(1, .8f, 1);
	bench.Read(benchFile, benchTexture);
	bench.toWorld = Scale(.4f)*RotateY(30)*Translate(-.2f, -.99f, 0);
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

const char *usage = R"(
	mouse-drag:  rotate
	with shift:  translate xy
	mouse-wheel: translate z
)";

int main(int ac, char **av) {
	// create app window, read mesh
	GLFWwindow *w = InitGLFW(100, 100, winWidth, winHeight, "Room with a VR View");
	MakeScene();
	// callbacks
	RegisterMouseMove(MouseMove);
	RegisterMouseButton(MouseButton);
	RegisterMouseWheel(MouseWheel);
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
