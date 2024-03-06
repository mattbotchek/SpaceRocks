// This is the main entrypoint for SpaceRocks, it'll handle the setup of the "world" (or galaxy) setting up of gravity, and overall
// handling of sprite interactions. All sprites, actions, and intersections will either pass through or be handled by this file. 
// requires glad.h, glfw3.h, Draw.h, GLXtras.h, Sprite.h, and cmath (should already be present on system).

// File Owners: Matthew Botchek, Maria Milkowski

#include "resources.h"
#include <unordered_map>
#include <chrono>
#include "Text.h"

const string BASE_PATH = "C:/repos/SpaceRocks/SpaceRocks/Assets/";

// Sprites
Sprite background, actor, death, logo, endScreen;
bool playerDead = false;
Planet planet;
Asteroid asteroid1, asteroid2, asteroid3, asteroid4, asteroid5, asteroid6;
vector<Asteroid> asts;
vector<Asteroid *> asteroids;
int deathFrames = 0;

// WorldFrame and Generation
WorldGenerator gen; 
WorldFrame** frames;
WorldFrame currentFrame;
int actorFrameX, actorFrameY, frameMax;
vector<Planet> planets;
bool frameChanged = true;

// Hit detection
vec2 shuttleSensors[] = { {0.0f,-1.0f}, { 0.0f, 1.0f}, { -1.0f, 0.0f }, { 1.0f, 0.0f} };

// Key Input
extern std::unordered_map<int, time_t> kb = {};
std::chrono::steady_clock::time_point lastUpdate;
int		key = 0;
time_t	keydownTime = 0;

bool gravityEnabled = true;
bool gameRunning = true;

// Application

vec3 Probe(vec2 ndc) {
	// ndc (normalized device coords) lower left (-1,-1) to upper right (1,1)
	// return screen-space s, with s.z depth value at pixel (s.x, s.y)
	int4 vp = VP();
	vec3 s(vp[0] + (ndc.x + 1) * vp[2] / 2, vp[1] + (ndc.y + 1) * vp[3] / 2, 0);
	DepthXY((int)s.x, (int)s.y, s.z);
	return s;
}

vec3 Probe(vec2 v, mat4 m) {
	return Probe(Vec2(m * vec4(v, 0, 1)));
}

double distance(double x1, double y1, double x2, double y2) {
	return std::sqrt(std::pow((x2 - x1), 2) + std::pow((y2 - y1), 2));
}

void ApplyGravity(Planet lplanet)
{
	float actorY, actorX, dx, dy, gx, gy, planetX, planetY;
	actorX = actor.position[0];
	actorY = actor.position[1];
	planetX = lplanet.position[0];
	planetY = lplanet.position[1];

	dx = planetX - actorX;
	dy = planetY - actorY;

	double distanceToPlanet = distance(actorX, actorY, planetX, planetY);
	double angleToPlanet = atan2(dy, dx);

	float gStrength = lplanet.GetGravitySpeed(distanceToPlanet);

	gx = gStrength * cos(angleToPlanet);
	gy = gStrength * sin(angleToPlanet);

	actor.SetPosition(actor.position + vec2(gx, gy));
}

void SendAstroids()
{
	for (Asteroid *x : asteroids)
	{
		if (x->IsAlive())
		{
			if (x->position[0] <= 1.0 && x->position[1] <= 1.0 && x->position[0] >= -1.0 && x->position[1] >= -1.0)
			{
				x->Display();
				x->Move();
			}
			else
			{
				x->SetAlive(false);
			}
		}
		else
		{
			x->SetAngle(vec2(actor.position[0], actor.position[1]));
			x->Spawn();
			x->SetAlive(true);
		}
	}
}

void MoveActor(float speed) {
	if (actor.position[0] > 1 && actorFrameX < frameMax)
	{
		actorFrameX++;
		actor.SetPosition(vec2(-float(actor.position[0] - 0.1), actor.position[1]));
		frameChanged = true;
	}
	if (actor.position[0] < -1 && actorFrameX > 0)
	{
		actorFrameX--;
		actor.SetPosition(vec2(-float(actor.position[0] + 0.1), actor.position[1]));
		frameChanged = true;
	}
	if (actor.position[1] > 1 && actorFrameY < frameMax)
	{
		actorFrameY++;
		actor.SetPosition(vec2(actor.position[0], -float(actor.position[1] - 0.1)));
		frameChanged = true;
	}
	if (actor.position[1] < -1 && actorFrameY > 0)
	{
		actorFrameY--;
		actor.SetPosition(vec2(actor.position[0], -float(actor.position[1] + 0.1)));
		frameChanged = true;
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

	if (kb.count(GLFW_KEY_LEFT)) RotateActor(roationalSpeed);
	if (kb.count(GLFW_KEY_RIGHT)) RotateActor(-roationalSpeed);
	if (kb.count(GLFW_KEY_DOWN)) MoveActor(d);
	if (kb.count(GLFW_KEY_UP)) MoveActor(-d);
}

void Keyboard(int k, bool press, bool shift, bool control) {
	key = press ? k : 0;
	if (press) {
		kb[k] = clock();
		return;
	}
	kb.erase(k);
}

void StartGravity()
{
	if (gravityEnabled)
	{
		for (Planet x : planets)
		{
			ApplyGravity(x);
		}
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
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);

	if (frameChanged)
	{
		currentFrame = frames[actorFrameX][actorFrameY];
		planets = currentFrame.GetPlanets();

		for (Planet x : planets)
		{
			x.compensateAspectRatio = true;
		}

		frameChanged = false;
	}

	if (playerDead)
	{
		death.SetPosition(vec2(actor.position[0], actor.position[1]));
		
		if (deathFrames == death.nFrames * 6)
		{
			gameRunning = false;
		}

		death.Display();
		deathFrames++;
	}
	else {
		actor.Display();
	}

	background.Display();
	DisplayPlanets();

	const int nShuttleSensors = sizeof(shuttleSensors) / sizeof(vec2);
	vec3 shuttleProbes[nShuttleSensors];
	for (int i = 0; i < nShuttleSensors; i++)
		shuttleProbes[i] = Probe(shuttleSensors[i], actor.ptTransform);

	for (int i = 0; i < nShuttleSensors; i++)
	{
		if (abs(shuttleProbes[i].z - planets[0].z) < 0.05f)
		{
			cout << abs(shuttleProbes[i].z - asteroid1.z) << endl;
			playerDead = true;
			gravityEnabled = false;
		}
	}

	glDisable(GL_DEPTH_TEST);
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
	background.Initialize(BASE_PATH + "/Images/background.jpg", 0);
	actor.Initialize(BASE_PATH + "/Images/shuttle.png", -1);
	actor.SetScale(vec2(.05f, .05f));
	actor.SetPosition(vec2(0.0, 0.0));
	death.InitializeGIF(BASE_PATH + "Images/DeathExplosion.gif");
	death.SetScale(vec2(0.15f, 0.15f));
	death.SetPosition(vec2(actor.position[0], actor.position[1]));

	asteroids.push_back(&asteroid1);
	asteroids.push_back(&asteroid2);
	asteroids.push_back(&asteroid3);
	asteroids.push_back(&asteroid4);
	asteroids.push_back(&asteroid5);
	asteroids.push_back(&asteroid6);
}

int main(int ac, char** av) {
	srand(time(NULL));

	bool gameStart = false;

	GLFWwindow* mainGame = InitGLFW(100, 100, 1000, 1000, "SpaceRocks");
	glfwMakeContextCurrent(mainGame);
	
	cout << "Starting up world generator" << endl;
	frames = gen.getData();
	actorFrameX = 1;
	actorFrameY = 1;
	frameMax = 2;
	cout << "Finished World Gen" << endl;
	SetupGameWorld();
	cout << "Finished game setup" << endl;

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
		if (gameRunning)
		{
			gameDisplay();
			StartGravity();
			SendAstroids();
		}
		else
		{
			EndScreen();
		}

		TestKey();
		glfwSwapBuffers(mainGame);
		glfwPollEvents();
	}
}
