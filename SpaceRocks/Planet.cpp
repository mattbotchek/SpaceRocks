#include "Planet.h"
//
//Planet::Planet()
//{ 
//	GravityStrength = 0.0005f;
//	GravityReach = 0.5f;
//}


void Planet::init(vec2 pos, vec2 scale)
{
	GravityStrength = 0.0005f;
	GravityReach = 0.5f;
	Initialize("C:/repos/SpaceRocks/SpaceRocks/Assets/Images/sun.png", -0.5);
	SetPosition(pos);
	SetScale(scale);
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