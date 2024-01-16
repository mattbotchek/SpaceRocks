// Particles.cpp - demonstrate ballistic particles

//  ALTERNATIVES
//      A. flash/blink before a particle dies
//      B. a particle should bounce when it hits the ground
//      C. it should bounce when it hits some object
//      D. it should create new particles when it dies
//      E. when it hits the ground, it should slide across and then come to a stop
//      F. play with the emit rate and/or the lifetime
//      G. display the particle with transparency

#include <glad.h>										// OpenGL access 
#include <glfw3.h>										// application framework
#include <time.h>
#include "Camera.h"										// view transforms on mouse input
#include "Draw.h"										// view transforms on mouse input
#include "GLXtras.h"									// SetUniform
#include "Text.h"										// Text
#include "Widgets.h"									// Button

#define PI 3.141592f
#define MAX_PARTICLES 5000

vec3	wht(1, 1, 1), blk(0, 0, 0), red(1, 0, 0), org(1, .7f, 0), blu(0, 0, .7f), grn(0, .7f, 0);
float   gravity = 1;
float   ground = 0;
Button  reset("Reset", 20, 20, 100, 20, red);
vec3    lightSource(2, 2, -6);
float	height = 1, radius = 1;
int		winWidth = 700, winHeight = 700;
Camera	camera(0, 0, winWidth, winHeight, vec3(0,0,0), vec3(0,0,-10), 30, .001f, 500);

//GLuint  cylBufferId = 0;
//mat4    view, persp;
//vec2    mouseDown;                          // for each mouse down, need start point
//vec2    rotOld(-190,-20), rotNew(rotOld);   // previous, current rotations, rot.x is about Y-axis, rot.y is about X-axis
//float   dolly = -8;

struct Vertex {
	vec3 point, normal;
	Vertex() { }
	Vertex(vec3 &p, vec3 &n) : point(p), normal(n) { }
};

// Misc

float Lerp(float a, float b, float alpha) { return a+alpha*(b-a); }

float Blend(float x) {
	// cubic blend function f: f(0)=1, f(1)=0, f'(0)=f'(1)=0
	x = abs(x);
	float x2 = x*x, x4 = x2*x2;
	return x < DBL_EPSILON? 1 : x > 1? 0 : (-4.f/9.f)*x2*x4+(17.f/9.f)*x4+(-22.f/9.f)*x2+1;
};

float Random() { return (float) (rand()%1000)/1000.f; }
	// return a random number between 0 and 1

float Random(float a, float b) { return Lerp(a, b, Random()); }
	// return a random number between a and b

// Cylinder

class Cyl {
public:
	float height = 0, radius = 0;
	vec3 color, location;
//    Cyl();
	Cyl(float h, float r, vec3 c, vec3 l) : height(h), radius(r), color(c), location(l) { }
	bool Inside(vec3 p) {
		if (p.y > ground+height)
			return false;
		float dx = p.x-location.x, dz = p.z-location.z;
		return dx*dx+dz*dz < radius*radius;
	}
	void Draw() {
		Cylinder(location, location+vec3(0, height, 0), radius, radius, camera.modelview, camera.persp, color);
	}
};

Cyl cylinders[] = {
	Cyl(height/2, radius/4, org, vec3(-.3f, ground, .6f)),
	Cyl(height/3, radius/2, blu, vec3(.3f, ground, -.2f)),
	Cyl(height/4, radius/3, grn, vec3(.2f, ground, -.7f))
};
int nCylinders = sizeof(cylinders)/sizeof(Cyl);

// Particle

class Particle {
public:
	int     level;      // first generation is level 0
	bool    grounded;   // has particle returned to earth?
	clock_t birth;      // when instantiated
	float   lifetime;   // duration of activity
	float   speed;      // speed of motion per unit time
	float   size;       // in pixels
	float   emitRate;   // sub-particle emission rate if grounded
	vec3    position;   // current location in 3D
	vec3    velocity;   // current linear 3D direction
	vec3    color;      // r, g, b, alpha
	clock_t prevEmit;   // when last spawned another particle
	void Init(int l, float lt, float s, float sz, float er) {
		grounded = false;
		birth = clock();
		level = l;
		lifetime = lt;
		speed = s;
		size = sz;
		emitRate = er;
	}
	void Move(float deltaTime) {
		// update vertical component of velocity
		velocity[1] -= deltaTime*gravity;
		normalize(velocity);
		// update position
		position += speed*deltaTime*velocity;
		// bounce against cylinders
		for (int c = 0; c < nCylinders; c++) {
			Cyl &cyl = cylinders[c];
			if (cyl.Inside(position) && velocity[1] < 0) {
				position[1] = cyl.height;
				velocity[1] = -.5f*velocity[1];
				break;
			}
		}
	}
	void Update(float deltaTime) {
		// move ungrounded particle
		if (!grounded) {
			Move(deltaTime);
			// test for grounded
			if (position[1] <= ground) {
				position[1] = ground;
				grounded = true;
				prevEmit = clock();
			}
		}
	}
	void Draw() {
		Disk(position, size, color);
	}
};

// Emitter

class Emitter {
public:
	clock_t  prevTime;                 // needed to compute delta time
	clock_t  nextEmitTime;             // to control particle emissions
	Particle minParticle;              // minimum values for position, size, etc
	Particle maxParticle;              // maximum values
	Particle particles[MAX_PARTICLES]; // array of particles
	int      nparticles;               // # elements in array
	Emitter() {
		prevTime = clock();
		nparticles = 0;
		nextEmitTime = 0;
		srand((int) time(NULL));
		minParticle.Init(-1, .15f, .1f, 5, 15); // level, lifetime, speed, size, emitRate
		maxParticle.Init(-1, 7.f, .4f, 9, 50);
		minParticle.position = maxParticle.position = vec3(0, height, 0);
		minParticle.color = blk;
		maxParticle.color = wht;
	}
	void CreateParticle(int level = 0, vec3 *pos = NULL, vec3 *col = NULL) {
		// create new particle randomly between minParticle and maxParticle
		if (nparticles < MAX_PARTICLES) {
			Particle &p = particles[nparticles++];
			float b = Blend(level/10.f);
			float lifetime = Lerp(minParticle.lifetime, maxParticle.lifetime, b*Random());
			float speed =    Lerp(minParticle.speed,    maxParticle.speed,    b*Random());
			float size =     Lerp(minParticle.size,     maxParticle.size,     b*Random());
			float emitRate = Lerp(minParticle.emitRate, maxParticle.emitRate, b*Random());
			p.Init(level, lifetime, speed, size, emitRate);
			// set position, using argument if given
			if (pos)
				p.position = *pos;
			else
				// use min/max particle position
				for (int i = 0; i < 3; i++)
					p.position[i] = Random(minParticle.position[i], maxParticle.position[i]);
			// set color, using argument if given
			if (col)
				p.color = *col;
			else
				for (int k = 0; k < 3; k++)
					p.color[k] = Random(minParticle.color[k], maxParticle.color[k]);
			// set velocity
			float azimuth = Random(0., 2.f*PI);
			float elevation = Random(0., PI/2.f);
			float cosElevation = cos(elevation);
			p.velocity[1] = sin(elevation);
			p.velocity[2] = cosElevation*sin(azimuth);
			p.velocity[0] = cosElevation*cos(azimuth);
		}
	}
	void Draw() {
		for (int i = 0; i < nparticles; i++)
			particles[i].Draw();
	}
	void Update() {
		// need delta time to regulate speed
		clock_t now = clock();
		float dt = (float) (now-prevTime)/CLOCKS_PER_SEC;
		prevTime = now;
		// delete expired particles
		for (int i = 0; i < nparticles;) {
			Particle &p = particles[i];
			if (now > p.birth+p.lifetime*CLOCKS_PER_SEC) {
				// delete particle
				if (i < nparticles-1)
					particles[i] = particles[nparticles-1];
				nparticles--;
			}
			else
				i++;
		}
		// update ungrounded particles
		for (int k = 0; k < nparticles; k++) {
			Particle &p = particles[k];
			if (p.grounded) {
				float dt = (float) (now-p.prevEmit)/CLOCKS_PER_SEC;
				if (dt > 1./p.emitRate) {

					// spawn new particle
					CreateParticle(p.level+1, &p.position, &p.color);
					p.prevEmit = now;
				}
			}
			else
				p.Update(dt);
		}
		// possibly emit new particle
		if (now > nextEmitTime) {
			CreateParticle();
			float randomBoundedEmitRate = Random(minParticle.emitRate, maxParticle.emitRate);
			nextEmitTime = now+(clock_t)((float)CLOCKS_PER_SEC/randomBoundedEmitRate);
		}
	}
} emitter;

// Display

void Display(void) {
	// background, zbuffer, blend
	glClearColor(.65f, .5f, .5f, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   // draw ground, cylinders, particles
//    float g = 12, h = ground;
//    float groundfloor[][3] = {{-g, h, -g}, {-g, h, g}, {g, h, g}, {g, h, -g}};
	GLuint cylinderShader = GetCylinderShader();
	glUseProgram(cylinderShader);
	vec3 xlight = Vec3(camera.fullview*vec4(lightSource, 1));
	SetUniform(cylinderShader, "light", xlight);
	for (int c = 0; c < nCylinders; c++)
		cylinders[c].Draw();
	UseDrawShader(camera.fullview);
	emitter.Draw();
	// draw buttons last
	ScreenMode();
	glDisable(GL_DEPTH_TEST);
	reset.Draw(NULL, NULL);
	Text(winWidth-100, 50, blk, 12, "%i particles", emitter.nparticles);
	glFlush();
}

// Mouse

void MouseButton(float x, float y, bool left, bool down) {
	if (down) camera.Down(x, y, Shift()); else camera.Up();
}

void MouseMove(float x, float y, bool leftDown, bool rightDown) {
	if (leftDown) camera.Drag(x, y);
}

void MouseWheel(float spin) { camera.Wheel(spin, Shift()); }

// Application

void Resize(int width, int height) {
	camera.Resize(winWidth = width, winHeight = height);
	glViewport(0, 0, width, height);
}

int main(int ac, char **av) {
	GLFWwindow *w = InitGLFW(100, 100, winWidth, winHeight, "3D-Mesh");
	RegisterMouseMove(MouseMove);
	RegisterMouseButton(MouseButton);
	RegisterMouseWheel(MouseWheel);
	RegisterResize(Resize);
	while (!glfwWindowShouldClose(w)) {
		emitter.Update();
		Display();
		glfwPollEvents();
		glfwSwapBuffers(w);
	}
}
