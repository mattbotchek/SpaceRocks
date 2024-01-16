// Go Ballistic!

#include <glad.h>											// OpenGL access 
#include <glfw3.h>											// application framework
#include <time.h>											// application framework
#include "Camera.h"											// view transforms on mouse input
#include "Draw.h"
#include "GLXtras.h"										// SetUniform
#include "Mesh.h"											// cube.Read

// window, camera, light
int		winWidth = 700, winHeight = 700;
Camera	camera(0, 0, winWidth, winHeight, vec3(-50, -15, 0), vec3(0, 0, -10));
vec3	light(.5f, .2f, 1);

// objects
Mesh	square, cannon, sphere;

// interaction
int		key = 0;
time_t	keydownTime = 0;

// cannon
float	azimuth = 0, elevation = 45;

// cannonball animation
time_t	prevAnimateTime = 0;
bool	animating = false;

class Projectile {
public:
	vec3 p, v;
	float speed = 2, gravity = .5f, drag = 0, ground = .05f;
	bool Step(float dt) {
		if (p.z > .05f) {
			p += dt*speed*v;
			v.z -= dt*gravity;
			v *= (1-dt*drag);
		}
		return p.z > ground;
	}
	void Set(vec3 pt, vec3 vec) { p = pt; v = vec; }
	Projectile(float speed = 2, float gravity = .5f, float drag = 0) : speed(speed), gravity(gravity), drag(drag) { }
};

Projectile cannonball;

// Cannon Aim

bool In(float x, float min, float max) {
	return x >= min && x <= max;
}

void ChangeCannonAim(float dAzimuth, float dElevation) {
	if (abs(dAzimuth) > .01f && In(azimuth+dAzimuth, -45, 45)) {
		cannon.toWorld = cannon.toWorld*RotateY(dAzimuth);
		azimuth += dAzimuth;
	}
	if (abs(dElevation) > .01f && In(elevation+dElevation, 0, 90)) {
		cannon.toWorld = cannon.toWorld*RotateX(dElevation);
		elevation += dElevation;
	}
}

// Cannonball

void ResetCannonball(vec3 &p, vec3 &v) {
	v = normalize(Vec3(cannon.toWorld*vec4(0, .05f, 1, 0)));
	p = Vec3(cannon.toWorld*vec4(0, .25f, 1, 1))-.05f*v;
}

void Fire() {
	animating = true;
	ResetCannonball(cannonball.p, cannonball.v);
	prevAnimateTime = clock();
}

// Animation

void Animate() {
	time_t now = clock();
	float dt = (float) (now-prevAnimateTime)/CLOCKS_PER_SEC;
	prevAnimateTime = now;
	if (animating) {
		animating = cannonball.Step(dt);
		sphere.toWorld = Translate(cannonball.p)*Scale(.03f);
	}
}

// Display

void DisplayCannonballArc(float stepSize, vec3 color, float lineWidth) {
	Projectile ptile;
	ResetCannonball(ptile.p, ptile.v);
	vec3 p = ptile.p;
	while (ptile.Step(stepSize)) {
		Line(p, ptile.p, lineWidth, color);
		p = ptile.p;
	}
}

void Display() {
	glClearColor(1, 1, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	int shader = UseMeshShader();
	// transform, upload light
	vec3 xlight = Vec3(camera.modelview*vec4(light, 1));
	SetUniform(shader, "defaultLight", xlight);
	// render ground, cannon, cannonball arc
	square.Display(camera);
	cannon.Display(camera, 0);
	sphere.Display(camera);
	UseDrawShader(camera.fullview);
	glDisable(GL_DEPTH_TEST);
	DisplayCannonballArc(.01f, vec3(0, 1, 0), 2);
	glFlush();
}

// Mouse

void MouseButton(float xmouse, float ymouse, bool left, bool down) {
	if (down)
		camera.Down(xmouse, ymouse, Shift(), Control());
	if (!down)
		camera.Up(); 
}

void MouseMove(float xmouse, float ymouse, bool leftDown, bool rightDown) {
	if (leftDown)
		camera.Drag(xmouse, ymouse);
}

void MouseWheel(float spin) {
	camera.Wheel(spin, Shift());
}

// Keyboard

void TestKey() {
	if (key == GLFW_KEY_RIGHT)
		ChangeCannonAim(-1, 0);
	if (key == GLFW_KEY_LEFT)
		ChangeCannonAim(1,  0);
	if (key == GLFW_KEY_DOWN)
		ChangeCannonAim(0, 1);
	if (key == GLFW_KEY_UP)
		ChangeCannonAim(0, -1);
}

void CheckUser() {
	if (KeyDown(key) && (float) (clock()-keydownTime)/CLOCKS_PER_SEC > .2f)
		TestKey();
}

void Keyboard(int k, bool press, bool shift, bool control) {
	if (press) {
		if (k == 'F')
			Fire();
		else {
			key = k;
			keydownTime = clock();
			TestKey();
		}
	}
}

// Application

void Resize(int width, int height) {
	camera.Resize(width, height);
	glViewport(0, 0, width, height);
}

const char *usage = R"(
	left/right/up/down arrow keys: aim
	F: fire!
)";

int main(int ac, char **av) {
	GLFWwindow *w = InitGLFW(100, 100, winWidth, winHeight, "Go Ballistic!");
	string dir = "C:/Assets/";
	// read OBJ mesh files
	square.Read(dir+"Models/Square.obj");
	sphere.Read(dir+"Models/Sphere.obj");
	cannon.Read(dir+"Models/Cannon.obj", dir+"Images/CannonTexture.png");
	// adjust scene
	cannon.toWorld = Scale(.5f)*Translate(vec3(0, 3.5f, .35f))*RotateX(elevation);
	square.toWorld = Scale(2);
	// position cannonball at end of cannon
	ResetCannonball(cannonball.p, cannonball.v);
	sphere.toWorld = Translate(cannonball.p)*Scale(.03f);
	// callbacks
	RegisterMouseMove(MouseMove);
	RegisterMouseButton(MouseButton);
	RegisterMouseWheel(MouseWheel);
	RegisterResize(Resize);
	RegisterKeyboard(Keyboard);
	printf("Usage: %s\n", usage);
	// event loop
	while (!glfwWindowShouldClose(w)) {
		CheckUser();
		Animate();
		Display();
		glfwPollEvents();
		glfwSwapBuffers(w);
	}
}
