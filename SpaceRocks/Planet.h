#pragma once
#include "resources.h"

class Planet
{
public:
	Planet();
	void SetPlanet(vec2 p);
	vec2 GetPosition();
	float GetGravityStrength();
	float GetGravityReach();
	float GetGravitySpeed(double distance);

private:
	Sprite planetSprite;
	float GravityStrength;
	float GravityReach;
};

