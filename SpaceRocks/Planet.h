#pragma once
#include "Sprite.h"

class Planet : public Sprite
{
public:
	//Planet();
	void init(vec2 pos, vec2 scale);
	float GetGravityStrength();
	float GetGravityReach();
	float GetGravitySpeed(double distance);

private:
	//Sprite planetSprite;
	string GetRandomPlanet();
	int GetRandomNumber();
	float GravityStrength;
	float GravityReach;
};

