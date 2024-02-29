#include "WorldFrame.h"

WorldFrame::WorldFrame()
{
}

void WorldFrame::AddPlanet(Planet planet)
{
    planets.push_back(planet);
}

vector<Planet> WorldFrame::GetPlanets()
{
    return planets;
}


