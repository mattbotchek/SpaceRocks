#pragma once
#include "Planet.h"

class WorldFrame
{
public:
	WorldFrame();
	void AddPlanet(Planet planet);
	vector<Planet> GetPlanets();

private:
	vector<Planet> planets;
};

