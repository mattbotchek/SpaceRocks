#include "Astroid.h"
#include <random>

void Asteroid::SetAngle(vec2 actor)
{
	float actorY, actorX, dx, dy, gx, gy, astroidX, astroidY;
	actorX = actor[0];
	actorY = actor[1];
	astroidX = position[0];
	astroidY = position[1];

	dx = actorX - astroidX;
	dy = actorY - astroidY;

	angleToActor = atan2(dy, dx);
}

void Asteroid::Move()
{
	float gx, gy;

	gx = speed * cos(angleToActor);
	gy = speed * sin(angleToActor);

	SetPosition(position + vec2(gx, gy));
}

void Asteroid::SetAlive(bool value)
{
	alive = value;
}

bool Asteroid::IsAlive()
{
	return alive;
}

pair<double, double> Asteroid::randomEdgePoint() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> widthDist(-1.0, 1.0);
    std::uniform_real_distribution<double> heightDist(-1.0, 1.0);
    std::uniform_int_distribution<int> edgeDist(0, 3);

    int edge = edgeDist(gen); // Randomly choose an edge (0: top, 1: right, 2: bottom, 3: left)
    double x, y;

    switch (edge) {
    case 0: // Top edge
        x = widthDist(gen);
        y = 1.0;
        break;
    case 1: // Right edge
        x = 1.0;
        y = heightDist(gen);
        break;
    case 2: // Bottom edge
        x = widthDist(gen);
        y = -1.0;
        break;
    case 3: // Left edge
        x = -1.0;
        y = heightDist(gen);
        break;
    }

    return std::make_pair(x, y);
}

void Asteroid::Spawn()
{
    Initialize("C:/repos/SpaceRocks/SpaceRocks/Assets/Images/asteroid1.tga", -0.5);

    SetScale(vec2(0.025, 0.025));

    alive = false;

    double lower_bound = 0.001;
    double upper_bound = 0.004;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> dist(lower_bound, upper_bound);

    speed = dist(gen);

    pair<double, double> spawnPosition = randomEdgePoint();

    SetPosition(vec2(spawnPosition.first, spawnPosition.second));
}
