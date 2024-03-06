#pragma once
#include "Sprite.h"

class Asteroid : public Sprite
{
public:
	void init();
	void SetAngle(vec2 actorPos);
	void Move();
	void SetAlive(bool value);
	bool IsAlive();
	void Spawn();

private:
	float speed;
	bool alive;
	double angleToActor;

	pair<double, double> randomEdgePoint();
};

