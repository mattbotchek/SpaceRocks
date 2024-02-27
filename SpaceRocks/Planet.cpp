#include "Planet.h"

Planet::Planet()
{ 
	GravityStrength = 0.0005f;
	GravityReach = 0.5f;
}


void Planet::initialize(vec2 pos, vec2 scale)
{
	planetSprite.Initialize("C:/repos/SpaceRocks/SpaceRocks/Assets/Images/sun.png");
	planetSprite.SetPosition(pos);
	planetSprite.SetScale(scale);
}

void Planet::Display()
{
	planetSprite.Display();
}

vec2 Planet::GetPosition()
{
	return planetSprite.GetPosition();
}

float Planet::GetGravityStrength()
{
	return GravityStrength;
}

float Planet::GetGravityReach()
{
	return GravityReach;
}

float Planet::GetGravitySpeed(double distance)
{
	double multiplier = GravityReach / distance;

	return multiplier * GravityStrength;
}
