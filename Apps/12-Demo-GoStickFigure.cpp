// GoStickFigure.cpp: 2D stick-figure animation and display

#include <glad.h>
#include <time.h>
#include <vector>
#include <GLFW/glfw3.h>
#include "Draw.h"
#include "GLXtras.h"

// colors
vec3	wht(1,1,1), blk(0,0,0), red(1,0,0), grn(0,1,0), blu(0,0,1), mag(1,0,1);

// animation
time_t	start = clock();
bool	animate = true;
float	PI = 3.1415f, loopTime = 2, loopTimeNeck = 3, loopTimeLeg = 1;

// keyboard
int		key = 0;
time_t	keyDownTime = 0;

class Segment {
public:
	int maxNChildren = 6;
	float angle = 0, length = 0;
	std::vector<Segment> children;
	vec3 color;
	mat4 m;
	vec2 p1, p2;
	void Draw(float lineWidth) {
		Line(p1, p2, lineWidth, color);
		Disk(p1, lineWidth, color);
		Disk(p2, lineWidth, color);
		for (size_t i = 0; i < children.size(); i++)
			children[i].Draw(lineWidth);
	}
	void Draw(mat4 parent, float lineWidth) {
		SetEnds(parent);
		Draw(lineWidth);
	}
	void SetEnds(mat4 parent) {
		vec4 origin(0, 0, 0, 1);
		mat4 me = parent*m;
		p1 = Vec2(parent*origin);
		p2 = Vec2(me*origin);
		for (size_t i = 0; i < children.size(); i++)
			children[i].SetEnds(me);
	}
	Segment *AddChild(float angle, float length, vec3 color) {
		int size = children.size();
		if (size < maxNChildren) {
			children.resize(size+1);
			children[size] = Segment(angle, length, color);
		}
		return &children[size];
	}
	void SetM(float a, float l) {
		angle = a;
		length = l;
		m = RotateZ(angle)*Translate(vec3(0, length, 0));
	}
	void SetLength(float l) { SetM(angle, l); }
	void SetAngle(float a) { SetM(a, length); }
	Segment(float angle = 0, float length = 0, vec3 color = vec3(0,0,0)) : color(color) {
		// angle: rotation ccw (in degrees) with respect to parent
		// length: geometric length of this segment
		children.reserve(maxNChildren); // thus, no reallocations
		SetM(angle, length);
	}
};

// figure 1
vec3 fig1Location(-.5f, 0, 0);
Segment fig1(-90, .5f);
Segment *s1 = fig1.AddChild(30, .4f, red);
Segment *s2 = fig1.AddChild(-30, .4f, grn);
Segment *s3 = s2->AddChild(20, .2f, blu);
Segment *s4 = s3->AddChild(90, .3f, mag);

// figure 2
vec3 fig2Location(.5f, 0, 0);
Segment hip(0, 0, blk);
Segment *lLeg = hip.AddChild(150, .5f, grn);
Segment *rLeg = hip.AddChild(-150, .5f, blu);
Segment *torso = hip.AddChild(0, .5f, mag);
Segment *neck = torso->AddChild(0, .2f, red);

// Move

void MoveFigure2(int k) {
	fig2Location.x += k == 'A'? -.1f : k == 'D'? .1f : 0;
}

void Update() {
	float keyTime = (float) (clock()-keyDownTime)/CLOCKS_PER_SEC;
	if (KeyDown(key) && keyTime > .2f)
		MoveFigure2(key);
}

// Animation

void Animate() {
	if (animate) {
		time_t now = clock();
		float elapsedTime = (float) (now-start)/CLOCKS_PER_SEC;
		// animate figure 1
		float t = (1+sin(2*PI*elapsedTime/loopTime))/2;
		float angle = 120+t*45;
		s1->SetAngle(angle);
		s2->SetLength(.4f+.4f*t);
		s2->SetAngle(-angle);
		s3->SetAngle(elapsedTime*180);
		s4->SetAngle(elapsedTime*540);
		// animate figure 2
		float tNeck = (1+sin(2*PI*elapsedTime/loopTimeNeck))/2;
		float tLeg = (1+sin(2*PI*elapsedTime/loopTimeLeg))/2;
		neck->SetAngle(-tNeck*25);
		lLeg->SetAngle(120+tLeg*30);
	}
}

// Display

void Display() {
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	UseDrawShader();
	// figure 1
	mat4 fig1View = Translate(fig1Location)*Scale(.8f);
	fig1.Draw(fig1View, 10);
	// figure 2
	mat4 fig2View = Translate(fig2Location)*Scale(.8f);
	hip.Draw(fig2View, 10);
	Disk(neck->p2, 40, wht);
	Disk(lLeg->p2, 20, lLeg->color);
	Disk(rLeg->p2, 20, rLeg->color);
}

// Application

void Keyboard(int k, bool press, bool shift, bool control) {
	if (press) {
		key = k;
		keyDownTime = clock();
		MoveFigure2(k);
	}
}

void Resize(int width, int height) {
	glViewport(0, 0, width, height);
}

int main(int ac, char **av) {
	GLFWwindow *w = InitGLFW(100, 100, 600, 600, "Go Stick-Figure");
	// callbacks
	RegisterResize(Resize);
	RegisterKeyboard(Keyboard);
	// event loop
	while (!glfwWindowShouldClose(w)) {
		Update();
		Animate();
		Display();
		glFlush();
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
}
