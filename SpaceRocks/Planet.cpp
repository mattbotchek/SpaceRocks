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
	Initialize(GetRandomPlanet(), -0.5);
	SetPosition(pos);
	SetScale(scale);
}

int Planet::GetRandomNumber()
{
	return rand() % 6 + 1;
}

string Planet::GetRandomPlanet()
{
	int num = GetRandomNumber();
	if (num == 1){
		return "C:/Users/miami/SpaceRocks/SpaceRocks/Assets/Images/planet1.tga";
	}
	else if (num == 2){
		return "C:/Users/miami/SpaceRocks/SpaceRocks/Assets/Images/planet2.tga";
	}
	else if (num == 3){
		return "C:/Users/miami/SpaceRocks/SpaceRocks/Assets/Images/planet3.tga";
	}
	else if (num == 4){
		return "C:/Users/miami/SpaceRocks/SpaceRocks/Assets/Images/planet4.tga";
	}
	else if (num == 5){
		return "C:/Users/miami/SpaceRocks/SpaceRocks/Assets/Images/planet5.tga";
	}
	else{
		return "C:/Users/miami/SpaceRocks/SpaceRocks/Assets/Images/planet6.tga";
	}
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