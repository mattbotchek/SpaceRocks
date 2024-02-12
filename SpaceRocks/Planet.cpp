#include "Planet.h"

Planet::Planet()
{ 
	GravityStrength = 0.0005f;
	GravityReach = 0.5f;
}

void Planet::SetPlanet(vec2 p)
{
	planetSprite.Initialize("C:/repos/SpaceRocks/SpaceRocks/Assets/Images/sun.png");
	planetSprite.SetPosition(p);
	planetSprite.SetScale(vec2(.2f, .2f));
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
	cout << "Gravitational Multiplier:	" << multiplier << endl;
	cout << "Gravitational Force:		" << multiplier * GravityStrength << endl;

	return multiplier * GravityStrength;
}
