#include "Astroid.h"

void Astroid::init(vec2 pos, vec2 scale)
{
	Initialize("C:/repos/SpaceRocks/SpaceRocks/Assets/Images/sun.png", -0.5);
	speed = ((double)rand()) / 10000;
	SetPosition(pos);
	SetScale(scale);
}

void Astroid::Move(Sprite actor)
{
	float actorY, actorX, dx, dy, gx, gy, astroidX, astroidY;
	actorX = actor.position[0];
	actorY = actor.position[1];
	astroidX = position[0];
	astroidY = position[1];

	dx = actorX - astroidX;
	dy = actorY - astroidY;

	double angleToActor = atan2(dy, dx);

	gx = speed * cos(angleToActor);
	gy = speed * sin(angleToActor);

	SetPosition(actor.position + vec2(gx, gy));
}
