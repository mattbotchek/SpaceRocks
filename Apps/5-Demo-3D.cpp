// Demo-5-3D-Arcball.cpp: same as Demo-4-Matrix.cpp, but a cube, not a square

#include <glad.h>                                         
#include <glfw3.h>                                        
#include "Camera.h"													// mouse-controlled camera
#include "GLXtras.h"  
#include "IO.h"														// LoadTexture, Shift

GLuint program = 0, textureName = 0, textureUnit = 0, vBuffer = 0;	// OpenGL identifiers
const char *file = "C:/Assets/Images/Parrots.jpg";	// bmp, jpg, png, tga supported

// window and camera
int winWidth = 700, winHeight = 700;
Camera camera(0, 0, winWidth, winHeight, vec3(0,0,0), vec3(0,0,-15));
	//        X, Y, screenW,  screenH,   rotation,    translation

// Display

const char *vertexShader = R"(
	#version 130
	uniform mat4 m;													// from CPU
	in vec3 point;													// from GPU
	in vec2 uv;														// from GPU
	out vec2 vuv;           
	void main() {
		gl_Position = m*vec4(point, 1);								// transform
		vuv = uv;													// send to pixel shader
	}
)";

const char *pixelShader = R"(
	#version 130
	in vec2 vuv;													// from vertex shader
	uniform sampler2D textureImage;									// from CPU
	out vec4 pColor;                                       
	void main() { pColor = texture(textureImage, vuv); }
)";

vec3 qpts[24];														// 24 points (6 quad faces)
vec2 quvs[24];														// corresponding uvs

void Display() {
	glClearColor(0, 0, 1, 1);										// set background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// clear background and z-buffer
	glEnable(GL_DEPTH_TEST);										// view only nearest surface
	glUseProgram(program);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glActiveTexture(GL_TEXTURE0+textureUnit);						// make texture active
	glBindTexture(GL_TEXTURE_2D, textureName);
	// shaded rendering
	VertexAttribPointer(program, "point", 3, 0, (void*) 0);			// set feed for qpoints
	VertexAttribPointer(program, "uv", 2, 0, (void*) sizeof(qpts));	// set feed for quvs
	SetUniform(program, "m", camera.fullview*Scale(2.5f));			// set vertex shader transform
	SetUniform(program, "textureImage", (int) textureUnit);			// set pixel shader texture map
	glDrawArrays(GL_QUADS, 0, 24);									// draw cube                     
	// line-drawing
	// glLineWidth(5);
	// for (int i = 0; i < 6; i++)
	//	glDrawArrays(GL_LINE_LOOP, 4*i, 4);							// outline each face
	if (camera.down && !camera.shift)
		camera.arcball.Draw(camera.control);
	glFlush();														// finish scene
}

// Mouse Handlers

void MouseButton(float x, float y, bool left, bool down) {
	if (left && down) camera.Down(x, y, Shift(), Control());
	if (!down) camera.Up();
}

void MouseMove(float x, float y, bool leftDown, bool rightDown) { if (leftDown) camera.Drag(x, y); }

void MouseWheel(float spin) { camera.Wheel(spin); }

// Cube Vertices

float l = -1, r = 1, b = -1, t = 1, n = -1, f = 1;					// left, right, bottom, top, near, far
vec3 points[] = { {l, b, n}, {l, b, f}, {l, t, n}, {l, t, f},		// 8 cube corners
				  {r, b, n}, {r, b, f}, {r, t, n}, {r, t, f} };

void SetQuad(int quad, int p0, int p1, int p2, int p3) {
	int i = 4*quad;
	qpts[i+0] = points[p0]; quvs[i+0] = vec2(0,0); 
	qpts[i+1] = points[p1]; quvs[i+1] = vec2(0,1); 
	qpts[i+2] = points[p2]; quvs[i+2] = vec2(1,1); 
	qpts[i+3] = points[p3]; quvs[i+3] = vec2(1,0);
}

void InitVertexBuffer() {
//	glUseProgram(program);
	enum { lbn=0, lbf, ltn, ltf, rbn, rbf, rtn, rtf };
	SetQuad(0, lbf, ltf, ltn, lbn);									// left face
	SetQuad(1, rtn, rtf, rbf, rbn);									// right
	SetQuad(2, rbn, rbf, lbf, lbn);									// bottom
	SetQuad(3, ltf, rtf, rtn, ltn);									// top
	SetQuad(4, ltn, rtn, rbn, lbn);									// near
	SetQuad(5, rbf, rtf, ltf, lbf);									// far
	int spts = sizeof(qpts), suvs = sizeof(quvs);
	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, spts+suvs, NULL, GL_STATIC_DRAW);	// allocate GPU buffer
	glBufferSubData(GL_ARRAY_BUFFER, 0, spts, qpts);				// store qpoints
	glBufferSubData(GL_ARRAY_BUFFER, spts, suvs, quvs);				// store quvs after qpoints
}

// Application

void Resize(int width, int height) {
	camera.Resize(winWidth = width, winHeight = height);
	glViewport(0, 0, width, height);
}

int main() {
	// init app window, GL context, anti-alias
	GLFWwindow *w = InitGLFW(200, 200, winWidth, winHeight, "3D");
	program = LinkProgramViaCode(&vertexShader, &pixelShader);
	InitVertexBuffer();
	textureName = ReadTexture(file);								// store texture image
	printf("mouse-drag:  rotate\nwith shift:  translate xy\nmouse-wheel: translate z");
	// callbacks
	RegisterMouseButton(MouseButton);
	RegisterMouseMove(MouseMove);
	RegisterMouseWheel(MouseWheel);
	RegisterResize(Resize);
	// event loop
	while (!glfwWindowShouldClose(w)) {
		Display();
		glfwSwapBuffers(w);                          
		glfwPollEvents();
	}
}
