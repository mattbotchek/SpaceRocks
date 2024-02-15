// This is the main entrypoint for SpaceRocks, it'll handle the setup of the "world" (or galaxy) setting up of gravity, and overall
// handling of sprite interactions. All sprites, actions, and intersections will either pass through or be handled by this file. 
// requires glad.h, glfw3.h, Draw.h, GLXtras.h, Sprite.h, and cmath (should already be present on system).

// File Owners: Matthew Botchek, Maria Milkowski

#include "resources.h"



Sprite background, startbackground, actor, logo, playtext;
vector <Planet> planets;
Planet planet, planet2;
bool hovering = false;

// interaction
int		key = 0;
time_t	keydownTime = 0;


// Application

double distance(double x1, double y1, double x2, double y2) {
	return std::sqrt(std::pow((x2 - x1), 2) + std::pow((y2 - y1), 2));
}

void ApplyGravity(Planet lplanet)
{
	float actorY, actorX, dx, dy, gx, gy, planetX, planetY;
	actorX = actor.GetPosition()[0];
	actorY = actor.GetPosition()[1];
	planetX = lplanet.GetPosition()[0];
	planetY = lplanet.GetPosition()[1];

	dx = planetX - actorX;
	dy = planetY - actorY;

	double distanceToPlanet = distance(actorX, actorY, planetX, planetY);
	double angleToPlanet = atan2(dy, dx);

	float gStrength = lplanet.GetGravitySpeed(distanceToPlanet);

	gx = gStrength * cos(angleToPlanet);
	gy = gStrength * sin(angleToPlanet);

	actor.SetPosition(actor.position + vec2(gx, gy));
}

void MoveActor(float speed) {
	// apply rotation for "forward" movement not square movement
	float dx, dy;
	// Convert actor rotation from degrees to radians
	double actorRotation = actor.rotation * 3.14159 / 180;
	dx = speed * cos(actorRotation);
	dy = speed * sin(actorRotation);

	actor.SetPosition(actor.position + vec2(dx, dy));
}

void RotateActor(float angle)
{
	actor.SetRotation(actor.rotation + angle);
}

// Movement

void TestKey()
{
	float d = 0.0015;
	float roationalSpeed = 0.5;
	if (key == GLFW_KEY_LEFT) RotateActor(roationalSpeed);
	if (key == GLFW_KEY_RIGHT) RotateActor(-roationalSpeed);
	if (key == GLFW_KEY_DOWN) MoveActor(d);
	if (key == GLFW_KEY_UP) MoveActor(-d);
}

void CheckUser() {
	if (KeyDown(key) && (float)(clock() - keydownTime) / CLOCKS_PER_SEC > .01f)
		TestKey();
}

void Keyboard(int k, bool press, bool shift, bool control) {
	if (press) 
	{
		key = k;
		keydownTime = clock();
		TestKey();
	}
}

void StartGravity()
{
	for (Planet x : planets)
	{
		ApplyGravity(x);
	}
}

void Resize(int width, int height) {
	glViewport(0, 0, width, height);
}

void DisplayPlanets()
{
	for (Planet x : planets)
	{
		x.Display();
	}
}

void Display() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	background.Display();
	actor.Display();
	//death.Display();
	glFlush();
}

void StartScreen()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	startbackground.Display();
	logo.Display();
	playtext.Display();
	glFlush();
}

void SetupGameWorld()
{
	background.Initialize("C:/repos/SpaceRocks/SpaceRocks/Assets/Images/background.jpg");
	actor.Initialize("C:/repos/SpaceRocks/SpaceRocks/Assets/Images/shuttle.png");
	//death.InitializeGIF("C:/repos/SpaceRocks/SpaceRocks/Assets/Images/DeathExplosion.gif", 10000000.0f);
	//death.SetScale(vec2(0.15f, 0.15f));
	//death.SetPosition(vec2(0, 0));
	actor.SetScale(vec2(.05f, .05f));
	actor.SetPosition(vec2(0.4, 0.4));

	planet.initialize(vec2(-0.4,0.4), vec2(0.2, 0.2));
	planets.push_back(planet);
	planet2.initialize(vec2(0.4, -0.4), vec2(0.2, 0.2));
	planets.push_back(planet2);
}

int main(int ac, char** av) {
	//// Start Screen
	//GLFWwindow* startScreen = InitGLFW(100, 100, 600, 600, "Start Game");
	//startbackground.Initialize("C:/repos/SpaceRocks/SpaceRocks/Assets/Images/background.jpg");
	//logo.Initialize("C:/repos/SpaceRocks/SpaceRocks/Assets/Images/spaceRocksLogo.tga");
	//playtext.Initialize("C:/repos/SpaceRocks/SpaceRocks/Assets/Images/toPlayText.tga");
	////start page loop
	//while (!glfwWindowShouldClose(startScreen) && !(GetAsyncKeyState(VK_SPACE) & 0x80000000)) {
	//	StartScreen();
	//	glfwSwapBuffers(startScreen);
	//	glfwPollEvents();
	//}
	//glfwTerminate();	
	// Main Game

	GLFWwindow* mainGame = InitGLFW(100, 100, 1000, 1000, "SpaceRocks");
	cout << "Created New Window" << endl;
	glfwMakeContextCurrent(mainGame);
	cout << "Made Current Context" << endl;
	SetupGameWorld();

	RegisterKeyboard(Keyboard);
	
	cout << "Entering Loop" << endl;
	// event loop
	while (!glfwWindowShouldClose(mainGame)) {
		CheckUser();
		Display();
		DisplayPlanets();
		StartGravity();
		glfwSwapBuffers(mainGame);
		glfwPollEvents();
	}
}
