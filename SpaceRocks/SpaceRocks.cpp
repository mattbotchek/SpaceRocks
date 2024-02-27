// This is the main entrypoint for SpaceRocks, it'll handle the setup of the "world" (or galaxy) setting up of gravity, and overall
// handling of sprite interactions. All sprites, actions, and intersections will either pass through or be handled by this file. 
// requires glad.h, glfw3.h, Draw.h, GLXtras.h, Sprite.h, and cmath (should already be present on system).

// File Owners: Matthew Botchek, Maria Milkowski

#include "resources.h"


const string BASE_PATH = "C:/repos/SpaceRocks/SpaceRocks/Assets/";

Sprite background, actor, logo, endScreen;
vector<Sprite *> sprites;
WorldGenerator gen; 
WorldFrame** frames;
WorldFrame currentFrame;
int actorFrameX, actorFrameY, frameMax;
vector<Planet> planets;
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
	if (actor.position[0] > 1 && actorFrameX < frameMax)
	{
		actorFrameX++;
		actor.SetPosition(vec2(-float(actor.position[0] - 0.1), actor.position[1]));
	}
	if (actor.position[0] < -1 && actorFrameX > 0)
	{
		actorFrameX--;
		actor.SetPosition(vec2(-float(actor.position[0] + 0.1), actor.position[1]));
	}
	if (actor.position[1] > 1 && actorFrameY < frameMax)
	{
		actorFrameY++;
		actor.SetPosition(vec2(actor.position[0], -float(actor.position[1] - 0.1)));
	}
	if (actor.position[1] < -1 && actorFrameY > 0)
	{
		actorFrameY--;
		actor.SetPosition(vec2(actor.position[0], -float(actor.position[1] + 0.1)));
	}
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

void gameDisplay() {
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	background.Display();
	actor.Display();
	//death.Display();
	currentFrame = frames[actorFrameX][actorFrameY];
	planets = currentFrame.GetPlanets();
	DisplayPlanets();
	glFlush();
}

void EndScreen()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	endScreen.Display();
	glFlush();
}

void StartScreen()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	logo.Display();
	glFlush();
}

void SetupGameWorld()
{
	logo.Initialize(BASE_PATH + "/Images/startScreen.tga");
	endScreen.Initialize(BASE_PATH + "/Images/gameover.tga");
	background.Initialize(BASE_PATH + "/Images/background.jpg");
	actor.Initialize(BASE_PATH + "/Images/shuttle.png");
	actor.SetScale(vec2(.05f, .05f));
	actor.SetPosition(vec2(0.0, 0.0));
}

int main(int ac, char** av) {
	bool gameStart = false;

	GLFWwindow* mainGame = InitGLFW(100, 100, 1000, 1000, "SpaceRocks");
	glfwMakeContextCurrent(mainGame);

	sprites.push_back(&actor);
	
	cout << "Starting up world generator" << endl;
	frames = gen.getData();
	actorFrameX = 1;
	actorFrameY = 1;
	frameMax = 2;
	cout << "Finished World Gen" << endl;
	SetupGameWorld();
	
	RegisterKeyboard(Keyboard);

	// event loop
	while (!glfwWindowShouldClose(mainGame)) {
		while (gameStart == false) {
			StartScreen();
			glfwSwapBuffers(mainGame);
			if (GetAsyncKeyState(VK_SPACE) & 0x8001) {
				gameStart = true;
			}
		}
		
		CheckUser();
		gameDisplay();
		//StartGravity();
		glfwSwapBuffers(mainGame);
		glfwPollEvents();
	}
}
