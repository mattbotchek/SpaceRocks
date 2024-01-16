// 7-Demo-Dig-It.cpp, from Graphics/Game Project 2023
// TEAM 9 Excavator.cpp, Kevin Khong, Matthew Lau, Cong Ho

#include <glad.h>
#include <glfw3.h>
#include <time.h>
#include "Camera.h"
#include "Draw.h"
#include "GLXtras.h"
#include "Mesh.h"

// window, camera, colors
int		winWidth = 1000, winHeight = 1000;
Camera	camera(0, 0, winWidth, winHeight, vec3(0, 0, 0), vec3(0, 0, -10));
vec3	red(1, 0, 0), org(1, .55f, 0), blk(0, 0, 0), yel(.8f, .8f, 0), gry(.5f), olv(.4f,.4f,0), blu(.2f,.2f,.5f);

// meshes
Mesh	tracks, cab, boom, arm, bucket;	// excavator
Mesh	ball, tree, terrain;			// scene

// excavator control
int		armRotation = 0, boomRotation = 0;
vec3	excavatorVelocity(0, 0, 0), excavatorAcceleration(0, 0, 0), bucketLoc(0, -.9f, -.4f);

// ball
vec3	ballPosition(0, 0, 0);
float	ballScale = .03f, fallProgress = 0, fallSpeed = .01f, dropZ = 0;
bool	ballPickedUp = false;

// lights
vec3	lights[] = { {1.3f, -.4f, .45f}, {-.4f, .6f, 1}, {.02f, .01f, .85f} };
int		nLights = sizeof(lights)/sizeof(vec3);

// interaction
Mover	mover;
void   *picked = NULL;

// Ball

void DropBall() {
	fallProgress += fallSpeed;
	if (!ballPickedUp && fallProgress < 10) {
		float currentX = ball.wrtParent[3][0]; // x
		float currentY = ball.wrtParent[3][1]; // y
		float newZ = dropZ-fallProgress;
		if (newZ < .1f)
			newZ = .1f;  // prevent ball dropping below floor
		vec3 newPos(currentX, currentY, newZ);
		ball.wrtParent = Translate(newPos)*Scale(ballScale);
		ball.SetWrtParent();
	}
}

void PickupBall() {
	// drop
	if (ballPickedUp) { // if the ball has been picked up
		if (boomRotation < -4) { // if the upper arm's rotation count is more than 3 drop the ball
			// find the ball in the upper arm's children and remove it
			auto iter = std::find(boom.children.begin(), boom.children.end(), &ball);
			if (iter != boom.children.end())
				boom.children.erase(iter);
			ball.parent = NULL; // remove the ball's parent
			ballPickedUp = false; // set the flag to false
			// begin dropping the ball
			DropBall();
		}
	}
	// pickup
	else {
		vec4 ballPosition = ball.toWorld*vec4(vec3(0, 0, 0), 1); // get the ball's position
		vec4 bottomOfUpperArmPosition = boom.toWorld*vec4(bucketLoc, 1); 
		float distance = length(vec3(ballPosition)-vec3(bottomOfUpperArmPosition)); 
		// calculate the distance between them
		if (distance <= .2f && boomRotation > -1) {
			boom.children.push_back(&ball); // add the ball to the children of the upper arm
			ball.parent = &boom; //set the parent of the ball to be the upper arm
			ball.SetWrtParent(); // update the ball's position relative to its parent
			ballPickedUp = true; // set the flag to true
			dropZ = vec3(bottomOfUpperArmPosition).z;
			fallProgress = 0;  
		}
	}
}

// Keyboard
int		key = 0, keyCount = 0;
time_t	keydownTime = 0;

void TestKey() {
	if (keyCount < 20) keyCount++;
	if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
		// rotate tracks
		float a = key == GLFW_KEY_LEFT ? 1.f: -1.f;
		tracks.SetToWorld(tracks.toWorld*Translate(tracks.centerOfRotation)*RotateZ(a)*Translate(-tracks.centerOfRotation));
	}
	if (key == 'K' || key == 'L') {
		// rotate cab
		float a = key == 'K' ? -1.f : 1.f;
		cab.SetToWorld(cab.toWorld*Translate(tracks.centerOfRotation)*RotateZ(a)*Translate(-tracks.centerOfRotation));
	}
	if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) {
		// move tracks
		float speedMin = .005f, speedMax = .02f;
		float a = keyCount/20.f, speed = speedMin+a*(speedMax-speedMin); // accelerate for 20 keystrokes
		float dy = key == GLFW_KEY_UP ? -speed : speed;
		tracks.SetToWorld(tracks.toWorld*Translate(0, dy, 0));
	}
	if (key == 'P' && (armRotation < 5)) {
		// open arm
		arm.SetToWorld(arm.toWorld*Translate(arm.centerOfRotation)*RotateX(7)*Translate(-arm.centerOfRotation));
		armRotation++;
	}
	if (key == 'O' && (armRotation > -5)) {
		// close arm
		arm.SetToWorld(arm.toWorld*Translate(arm.centerOfRotation)*RotateX(-7)*Translate(-arm.centerOfRotation));
		armRotation--;
	}
	if (key == 'G' && (boomRotation < 5)) {
		// close boom
		boom.SetToWorld(boom.toWorld*Translate(boom.centerOfRotation)*RotateX(7)*Translate(-boom.centerOfRotation));
		boomRotation++;
	}
	if (key == 'H' && (boomRotation > -5)) {
		// open boom
		boom.SetToWorld(boom.toWorld*Translate(boom.centerOfRotation)*RotateX(-7)*Translate(-boom.centerOfRotation));
		boomRotation--;
	}
	PickupBall();
}

void CheckKey() {
	if (KeyDown(key) && (float)(clock()-keydownTime)/CLOCKS_PER_SEC > .1f)
		TestKey();
}

void Keyboard(int k, bool press, bool shift, bool control) {
	key = press? k : 0;
	if (press) {
		// cache key for subsequent test by CheckKey
		keydownTime = clock();
		keyCount = 0;
		TestKey();
	}
}

// Display

void Display() {
	glClearColor(.4f, .4f, .8f, 1);						// set background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// clear background and z-buffer
	glEnable(GL_DEPTH_TEST);							// see only nearest surface 
	int s = UseMeshShader();
	SetUniform(s, "facetedShading", true);
	SetUniform(s, "twoSidedShading", true);				// no normals in excavator files
	// lights
	SetUniform(s, "nLights", nLights);
	SetUniform3v(s, "lights", nLights, (float *) lights, camera.modelview);
	// excavator
	tracks.Display(camera);
	cab.Display(camera);
	arm.Display(camera);
	boom.Display(camera);
	SetUniform(s, "twoSidedShading", false);			// normals in ball, tree, terrain files
	// terrain, tree, ball
	terrain.Display(camera);
	ball.Display(camera);
	tree.Display(camera);
	// annotations
	UseDrawShader(camera.fullview);
	glDisable(GL_DEPTH_TEST);
	for (int i = 0; i < nLights; i++)
		Star(lights[i], 6, org, blk);
	if (picked == &camera)
		camera.Draw();
	glFlush();
}

// Mouse

void MouseButton(float x, float y, bool left, bool down) {
	picked = NULL;
	if (down) {
		vec3 *v = MouseOver(lights, nLights, (int) x, (int) y, camera.fullview);
		if (v) {
			mover.Down(v, (int) x, (int) y, camera.modelview, camera.persp);
			picked = &mover;
		}
		if (!picked) {
			picked = &camera;
			camera.Down(x, y, Shift(), Control());
		}
	}
	else camera.Up();
}

void MouseMove(float x, float y, bool leftDown, bool rightDown) {
	if (leftDown && picked == &mover)
		mover.Drag((int) x, (int) y, camera.modelview, camera.persp);
	if (leftDown && picked == &camera)
		camera.Drag(x, y);
}

void MouseWheel(float spin) {
	camera.Wheel(spin, Shift());
}

// Application

void Resize(int width, int height) {
	camera.Resize(width, height);
	glViewport(0, 0, width, height);
}

const char* usage = R"(
	mouse-drag:        rotate scene
	with shift:        translate scene xy
	mouse-wheel:       translate scene z
	up/down arrows:    drive forward/backward
	left/right arrows: rotate tracks
	K/L:               rotate cab
	G/H:               close/open upper arm
	O/P:               close/open lower arm
)";

void ReadMesh(Mesh &mesh, Mesh *parent, string name, vec3 color, mat4 mat, vec3 cor = vec3(0, 0, 0)) {
	string dir("C:/Users/Jules/Code/GG-Projects/2023/#9-Excavator/");
	mesh.Read(dir+name, NULL, false);
	mesh.color = color;
	mesh.toWorld = mat;
	mesh.centerOfRotation = cor;
	mesh.parent = parent;
	if (parent)
		parent->children.push_back(&mesh);
	mesh.SetWrtParent();
}

int main(int ac, char** av) {
	GLFWwindow *w = InitGLFW(100, 100, winWidth, winHeight, "Excavation Simulator!");
	// read OBJ files
	ReadMesh(tracks,  NULL,    "Tracks.obj",  gry, Translate(0, 0, .1f), vec3(0, .4f, 0));
	ReadMesh(cab,     &tracks, "Cab.obj",     yel, Translate(0, 0, .25f));
	ReadMesh(boom,    &cab,    "Boom.obj",    yel, Translate(0, 0, .5f), vec3(0, .1f, -.3f));
	ReadMesh(arm,     &boom,   "Arm.obj",     yel, Translate(0, 0, .35f), vec3(0, -1, .3f));
	ReadMesh(terrain, NULL,    "Terrain.obj", blu, Translate(0, 0, -.5f)*Scale(5));
	ReadMesh(ball,    NULL,    "Sphere.obj",  red, Translate(0, 0, -.4f)*Scale(ballScale));
	ReadMesh(tree,    NULL,    "Tree.obj",    olv, Translate(0, 3, -.45f)*RotateX(90)*Scale(.06f));
	// callbacks
	RegisterMouseMove(MouseMove);
	RegisterMouseButton(MouseButton);
	RegisterMouseWheel(MouseWheel);
	RegisterResize(Resize);
	RegisterKeyboard(Keyboard);
	printf("Usage:%s", usage);
	// event loop
	while (!glfwWindowShouldClose(w)) {
		CheckKey();
		if (!ballPickedUp)
			DropBall();
		Display();
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
}
