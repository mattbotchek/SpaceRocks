// MovingSprite.cpp - interpolate between two sprites

#include <glad.h>
#include <GLFW/glfw3.h>
#include <time.h>
#include "GLXtras.h"
#include "Sprite.h"

Sprite background, sprites[2], *picked = NULL;
GLuint lerpProgram = 0;
time_t start = clock();

GLuint BuildLerpProgram() {
	const char *vShader = R"(
		#version 330
		uniform mat4 view;
		out vec2 uv;
		void main() {
			const vec2 pts[6] = vec2[6](vec2(-1,-1), vec2(1,-1), vec2(1,1), vec2(-1,1), vec2(-1,-1), vec2(1,1));
			uv = (vec2(1,1)+pts[gl_VertexID])/2;
			gl_Position = view*vec4(pts[gl_VertexID], 0, 1);
		}
	)";
	const char *pShader = R"(
		#version 330
		in vec2 uv;
		out vec4 pColor;
		uniform sampler2D textureImage1, textureImage2;
		uniform float t;
		void main() {
			vec4 pColor1 = texture(textureImage1, uv);
			vec4 pColor2 = texture(textureImage2, uv);
			pColor = pColor1+t*(pColor2-pColor1);
			if (pColor.a < .02) discard;		// don't tag z-buffer if nearly transparent
		}
	)";
	return LinkProgramViaCode(&vShader, &pShader);
}

class LerpingSprite : public Sprite {
public:
	Sprite *s1 = NULL, *s2 = NULL;
	float duration = 1;						// in seconds
	int textureUnit1 = 5, textureUnit2 = 6;	// arbitrary
	void Display(float t) {					// t=0: texture1, t=1: texture2
		glUseProgram(lerpProgram);
		// interpolate position & scale, send transform to vertex shader
		SetPosition((1-t)*s1->GetPosition()+t*s2->GetPosition());
		SetScale((1-t)*s1->GetScale()+t*s2->GetScale());
		SetUniform(lerpProgram, "view", ptTransform);
		// set texture for s1, send to pixel shader
		glActiveTexture(GL_TEXTURE0+textureUnit1);
		glBindTexture(GL_TEXTURE_2D, s1->textureName);
		SetUniform(lerpProgram, "textureImage1", (int) textureUnit1);
		// set texture for s2, send to pixel shader
		glActiveTexture(GL_TEXTURE0+textureUnit2);
		glBindTexture(GL_TEXTURE_2D, s2->textureName);
		SetUniform(lerpProgram, "textureImage2", (int) textureUnit2);
		// set t to interpolate textureImage1, textureImage2 in pixel shader
		SetUniform(lerpProgram, "t", t);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	LerpingSprite(Sprite *s1, Sprite *s2) : s1(s1), s2(s2) { }
};

LerpingSprite lerper(&sprites[0], &sprites[1]);

// Display

void Display() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	background.Display();
	for (Sprite s : sprites)
		s.Display();
	float elapsed = (float)(clock()-start)/CLOCKS_PER_SEC;
	float t = (float)(1+sin(3.1415*elapsed/lerper.duration))/2; // sine wave of given duration
	lerper.Display(t);
	glFlush();
}

// Mouse

void MouseWheel(float spin) {
	if (picked)
		picked->Wheel(spin);
}

void MouseButton(float x, float y, bool left, bool down) {
	if (left && down) {
		picked = NULL;
		for (Sprite &s : sprites)
			if (s.Hit(x, y)) {
				picked = &s;
				s.Down(x, y);
			}
	}
}

void MouseMove(float x, float y, bool leftDown, bool rightDown) {
	if (leftDown && picked)
		picked->Drag(x, y);
}

// Application

void Resize(int width, int height) {
	int s = width < height? width : height;
	glViewport(0, 0, s, s);
}

int main(int ac, char **av) {
	GLFWwindow *w = InitGLFW(100, 50, 1000, 1000, "Lerping Sprite Demo");
	// initialize background, foreground sprites
	string dir("C:/Assets/Images/");
	background.Initialize(dir+"waterBackground.png");
	const char *names[] = { "reptile32.png", "ichthyosteg32.png" };
	vec2 positions[] = { {-.5f, -.5f}, {.5f, .5f} };
	for (int i = 0; i < 2; i++) {
		sprites[i].Initialize(dir+names[i]);
		sprites[i].SetPosition(positions[i]);
		sprites[i].SetScale(vec2(.3f));
	}
	// initialize lerper sprite
	lerpProgram = BuildLerpProgram();
	lerper.SetScale(vec2(.3f));
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
