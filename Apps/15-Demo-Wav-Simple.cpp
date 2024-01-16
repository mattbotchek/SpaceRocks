// Demo-Wav-Simple.cpp

#include <glad.h>
#include <GLFW/glfw3.h>
#include "GLXtras.h"
#include "Wav.h"
#include "Widgets.h"
	
// audio
Wav wav("C:/Users/Jules/Code/GG-Projects/Mario8.wav");
float volume = 1;

// colors
vec3 wht(1, 1, 1), grn(0, .7f, 0), blu(.5f, .5f, .9f), blk(0, 0, 0);

// buttons
Button stopB("STOP", 15, 15, 60, 60, wht), pauseB("PAUSE", 80, 15, 60, 60, wht), playB("PLAY", 145, 15, 60, 60, wht);
Button *buttons[] = { &stopB, &pauseB, &playB };

// display

void Display() {
	glClearColor(blu.x, blu.y, blu.z, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	for (Button *b : buttons) {
		b->Draw(NULL, 11, NULL, grn);
		b->Outline(blk);
	}
	glFlush();
}

void MouseButton(float x, float y, bool left, bool down) {
	if (down) {
		if (stopB.Hit(x, y)) wav.Stop();
		if (playB.Hit(x, y)) wav.Play(volume);
		if (pauseB.Hit(x, y)) wav.Pause();
	}
}

void Keyboard(int key, bool press, bool shift, bool control) {
	if (press) {
		if (key == GLFW_KEY_UP && volume < 1) volume += .1f;
		if (key == GLFW_KEY_DOWN && volume > 0) volume -= .1f;
		wav.SetVolume(volume);
		printf("volume now %2.1f\n", volume);
	}
}

int main(int ac, char **av) {
	// app initialization
	GLFWwindow *w = InitGLFW(100, 100, 220, 90, "WAV");
	RegisterMouseButton(MouseButton);
	RegisterKeyboard(Keyboard);
	// initialize audio device to reduce lag
	wav.OpenDevice();
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
