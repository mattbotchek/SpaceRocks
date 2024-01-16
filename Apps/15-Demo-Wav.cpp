// Demo-Wav.cpp

#include <glad.h>
#include <GLFW/glfw3.h>
#include "Draw.h"
#include "GLXtras.h"
#include "Wav.h"
#include "Widgets.h"
	
// audio
string dir("C:/Users/Jules/Code/GG-Projects/");
Wav bSym3(dir+"BSym3Tiny.wav"), mario7(dir+"Mario8.wav"), *picked = &bSym3;
float volume = 1;

// app
int winW = 1000, winH = 400;

// buttons
Button rewindB(295, 10, 45, 45), pauseB(365, 10, 45, 45), playB(435, 10, 45, 45);
Button marioB(530, 10, 45, 45), beethB(600, 10, 45, 45);
Button *buttons[] = { &playB, &pauseB, &rewindB, &marioB, &beethB };

// display
WavView leftView(20, 70, winW/2-30, 310, &bSym3, C_Left);
WavView rightView(winW/2+10, 70, winW/2-30, 310, &bSym3, C_Right);
WavView monoView(winW/4, 70, winW/2-30, 310, &mario7, C_Mono);

void Display() {
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	if (picked == &bSym3) {
		leftView.Display();
		rightView.Display();
	}
	if (picked == &mario7)
		monoView.Display();
	for (int i = 0; i < sizeof(buttons)/sizeof(Button *); i++)
		buttons[i]->Draw(5);
	(picked == &mario7? marioB : beethB).Outline(vec3(0, .7f, 0));
	glFlush();
}

void MouseButton(float x, float y, bool left, bool down) {
	if (down) {
		if (rewindB.Hit(x, y)) picked->Stop();
		if (playB.Hit(x, y)) picked->Play(volume);
		if (pauseB.Hit(x, y)) picked->Pause();
		if (marioB.Hit(x, y)) picked = &mario7;
		if (beethB.Hit(x, y)) picked = &bSym3;
	}
}

void Keyboard(int key, bool press, bool shift, bool control) {
	if (press) {
		if (key == GLFW_KEY_UP && volume < 1) volume += .1f;
		if (key == GLFW_KEY_DOWN && volume > 0) volume -= .1f;
		picked->SetVolume(volume);
		printf("volume now %2.1f\n", volume);
	}
}

int main(int ac, char **av) {
	// app initialization
	GLFWwindow *w = InitGLFW(100, 100, winW, winH, "WAV");
	RegisterMouseButton(MouseButton);
	RegisterKeyboard(Keyboard);
	// buttons
	playB.Initialize(dir+"Play.png");
	pauseB.Initialize(dir+"Pause.png");
	rewindB.Initialize(dir+"Rewind.png");
	marioB.Initialize(dir+"Mario.png");
	beethB.Initialize(dir+"Beeth.png");
	// initialize audio device to reduce lag
	bSym3.OpenDevice();
	mario7.OpenDevice();
	// usage and event loop
	printf("use up/down arrows to increase/decrease volume\n");
	while (!glfwWindowShouldClose(w)) {
		Display();
		glfwPollEvents();
		glfwSwapBuffers(w);
	}
	glfwDestroyWindow(w);
	glfwTerminate();
}
