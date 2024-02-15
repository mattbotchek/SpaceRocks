#pragma once
#include "resources.h"

class Planet
{
public:
	Planet();
	void Display();
	vec2 GetPosition();
	void initialize(vec2 pos, vec2 scale);
	float GetGravityStrength();
	float GetGravityReach();
	float GetGravitySpeed(double distance);

private:
	Sprite planetSprite;
	float GravityStrength;
	float GravityReach;
};

