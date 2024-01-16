// WavySquareAndCup.cpp

#include <glad.h>
#include <glfw3.h>
#include <time.h>
#include "Camera.h"
#include "Draw.h"
#include "GLXtras.h"
#include "IO.h"
#include "Mesh.h"

// application display parameters
int			winWidth = 700, winHeight = 700;
Camera		camera(0, 0, winWidth, winHeight, vec3(20,7,0), vec3(0,0,-6));

// meshes
Mesh		cup;
int			cupTextureUnit = 1;

// tesselation
GLuint		tessProgram = 0;
int			tessRes = 64;
int			tessTextureUnit = 7, tessTextureName = 0;


// Display
vec3		lights[] = { {1, 1, .2f}, {.5f, .2f, 1} };
const int	nLights = sizeof(lights)/sizeof(vec3);

void SetLights(int program) {
	vec3 xLights[nLights];
	for (int i = 0; i < nLights; i++)
		xLights[i] = Vec3(camera.modelview*vec4(lights[i]));
	SetUniform(program, "nLights", nLights);
	SetUniform3v(program, "lights", nLights, (float *) xLights);
}

// ripple
float		rippleFreq = 16, rippleHeight = .05f;

// animation
time_t		was = clock();
float		waveTime = 0, waveSpeed = 10;

void Animate() {
	// animate wave
	float dt = (float) (clock()-was)/CLOCKS_PER_SEC;
	waveTime += dt;
	was = clock();
	// bob the cup
	float phaseOffset = -waveTime*waveSpeed;
	float z = 1.5f*rippleHeight*cos(phaseOffset);
	cup.toWorld = RotateY(30)*Scale(.3f)*Translate(0, .51f+z, 0);
}

void Display() {
	glClearColor(.5f, .5f, .5f, 1);						// set background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// clear background and z-buffer
	glEnable(GL_DEPTH_TEST);							// see only nearest surface
	// rippling board
	glUseProgram(tessProgram);
	SetUniform(tessProgram, "modelview", camera.modelview*RotateX(90));
	SetUniform(tessProgram, "persp", camera.persp);
	SetLights(tessProgram);
	SetUniform(tessProgram, "phaseOffset", -waveTime*waveSpeed);
	SetUniform(tessProgram, "height", rippleHeight);
	SetUniform(tessProgram, "freq", rippleFreq);
	SetUniform(tessProgram, "textureImage", tessTextureUnit);
	glActiveTexture(GL_TEXTURE0+tessTextureUnit);
	glBindTexture(GL_TEXTURE_2D, tessTextureName);
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	float res = (float) tessRes;
	float outerLevels[] = { res, res, res, res }, innerLevels[] = { res, res };
	glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, outerLevels);
	glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, innerLevels);
	glDrawArrays(GL_PATCHES, 0, 4);
	// cup on board
	int meshProgram = UseMeshShader();
	SetLights(meshProgram);
	cup.Display(camera, cupTextureUnit);
	glFlush();
}

// Scene

void MakeScene() {
	string dir = "C:/Assets/", modelsDir = dir+"Models/", imagesDir = dir+"Images/";
	cup.Read(modelsDir+"Teacup.obj", imagesDir+"Parrots.tga");
	cup.toWorld = RotateY(30)*Scale(.3f)*Translate(0, .51f, 0);
	tessTextureName = ReadTexture((char *) (imagesDir+"Lily.tga").c_str());
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

// Tessellation Program

const char *tVertexShader = R"(
	#version 410 core
	void main() { }
)";

const char *tTessellationShader = R"(
	#version 410 core
	layout (quads, equal_spacing, ccw) in;
	out vec3 vPoint, vNormal;
	out vec2 vUv;
	uniform mat4 modelview, persp;
	uniform float freq = 3, height = .2, phaseOffset = 0;
	void Ripple(float s, float t, out vec3 p, out vec3 n) {
		vec2 xy = vec2(2*s-1, 2*t-1);			// origin is center of patch sized +/-1
		float a = phaseOffset+length(xy)*freq;	// angle
		p = vec3(xy, height*cos(a));			// p.z is function of cos, so peak is over origin
		float f = freq*sin(a)*height;
		n = normalize(vec3(f*xy, 1));
			// derivative of cos is -sin, so velocity at (a, cos(x)) is (1, -sin(a))
			// non-unit-length normal is (sin(a), 1)
	}
	void main() {
		vUv = gl_TessCoord.st;
		vec3 p, n;
		Ripple(vUv.s, vUv.t, p, n);
		vPoint = (modelview*vec4(p, 1)).xyz;				// transform point by view
		vNormal = normalize((modelview*vec4(n, 0)).xyz);	// transform normal by view
		gl_Position = persp*vec4(vPoint, 1);				// apply perspective
	}
)";

// Application

const char *usage = R"(
	mouse-drag:  rotate
	with shift:  translate xy
	mouse-wheel: translate z
)";

int main(int ac, char **av) {
	// create app window, read mesh
	GLFWwindow *w = InitGLFW(100, 100, winWidth, winHeight, "Wavy Square And Cup");
	MakeScene();
	const char *tPixelShader = GetMeshPixelShaderNoLines();
	tessProgram = LinkProgramViaCode(&tVertexShader, NULL, &tTessellationShader, NULL, &tPixelShader);
	// callbacks
	RegisterMouseMove(MouseMove);
	RegisterMouseButton(MouseButton);
	RegisterMouseWheel(MouseWheel);
	RegisterResize(Resize);
	printf("Usage: %s", usage);
	// event loop
	while (!glfwWindowShouldClose(w)) {
		Animate();
		Display();
		glfwPollEvents();
		glfwSwapBuffers(w);
	}
}
