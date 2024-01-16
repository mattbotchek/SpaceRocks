//JuliaSet.cpp


#include <glad.h>
#include <glfw3.h>
#include "GLXtras.h"
#include <time.h>
#include <stdio.h>

GLuint program = 0;
int winWidth = 800, winHeight = 800;
float cReal = -.8f, cImag = .156f;

const char* vertexShader = R"(
	#version 130
	void main() {
		vec2 points[] = vec2[4](vec2(-1, -1), vec2(-1, 1), vec2(1, 1), vec2(1, -1));
		gl_Position = vec4(points[gl_VertexID], 0, 1);
	}
)";

const char* pixelShader = R"(
	#version 130
	out vec4 pColor;
	uniform float cReal = -.8, cImag = .156;
	struct Complex { float r, i; };
	Complex Multiply(Complex a, Complex b) { return Complex(a.r*b.r-a.i*b.i, a.i*b.r+a.r*b.i); }
	Complex Add(Complex a, Complex b) { return Complex(a.r+b.r, a.i+b.i); }
	float Magnitude(Complex a) { return sqrt(a.r*a.r+a.i*a.i); }
	void main() {
		int kLimit = 1000;
		float x = 2*gl_FragCoord.x/800-1, y = 2*gl_FragCoord.y/800-1;
		Complex a = Complex(x, y);
		Complex c = Complex(cReal, cImag);
		
		for (int k = 0; k < kLimit; k++) {
			a = Add(Multiply(a, a), c);
			if (Magnitude(a) > 500)  {
			float n = float(k)/kLimit; // should be between 0 and 1
			pColor = vec4(n, n, n, 1);
			return;
			}
			}
		pColor = vec4(1, 1, 1, 1);
	}
)";

void MouseMove(GLFWwindow* w, double x, double y) {
	if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		cReal = 2 * (float)x / winWidth - 1;
		cImag = 2 * (float)y / winHeight - 1;
	}
}

void Display() {
	glUseProgram(program);
	SetUniform(program, "cReal", cReal);
	SetUniform(program, "cImag", cImag);
	glDrawArrays(GL_QUADS, 0, 4);
	glFlush();
	
}

int main() {
	// init window, build shader
	glfwInit();
	GLFWwindow* w = glfwCreateWindow(winWidth, winHeight, "Julia Set", NULL, NULL);
	glfwSetWindowPos(w, 100, 100);
	glfwMakeContextCurrent(w);
	glfwSetCursorPosCallback(w, MouseMove);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	program = LinkProgramViaCode(&vertexShader, &pixelShader);
	// event loop
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(w)) {
		Display();
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
	glfwDestroyWindow(w);
	glfwTerminate();
}


