#include "WorldGenerator.h"

WorldGenerator::WorldGenerator()
{ }

WorldFrame** WorldGenerator::getData()
{
	WorldFrame** frames = new WorldFrame * [3];

	// Dynamically allocate memory for each row
	for (int i = 0; i < 3; ++i) {
		frames[i] = new WorldFrame[3];
	}

	Planet planet;
	frames[0][0];
	planet.init(vec2(0.2, 0.4), vec2(0.2, 0.2));
	frames[0][0].AddPlanet(planet);
	planet.init(vec2(-0.2, 0.4), vec2(0.25, 0.25));
	frames[0][0].AddPlanet(planet);

	planet.init(vec2(-0.3, 0.4), vec2(0.2, 0.2));
	frames[0][1].AddPlanet(planet);
	planet.init(vec2(-0.2, -0.4), vec2(0.25, 0.25));
	frames[0][1].AddPlanet(planet);

	planet.init(vec2(-0.2, -0.4), vec2(0.2, 0.2));
	frames[0][2].AddPlanet(planet);
	planet.init(vec2(0.4, 0.3), vec2(0.25, 0.25));
	frames[0][2].AddPlanet(planet);

	planet.init(vec2(0.8, 0.2), vec2(0.2, 0.2));
	frames[1][0].AddPlanet(planet);
	planet.init(vec2(-0.2, 0.4), vec2(0.25, 0.25));
	frames[1][0].AddPlanet(planet);

	planet.init(vec2(0.4, 0.4), vec2(0.2, 0.2));
	frames[1][1].AddPlanet(planet);
	planet.init(vec2(-0.4, -0.4), vec2(0.25, 0.25));
	frames[1][1].AddPlanet(planet);

	planet.init(vec2(0.5, 0.4), vec2(0.2, 0.2));
	frames[1][2].AddPlanet(planet);
	planet.init(vec2(-0.1, -0.4), vec2(0.25, 0.25));
	frames[1][2].AddPlanet(planet);

	planet.init(vec2(0.0, 0.0), vec2(0.2, 0.2));
	frames[2][0].AddPlanet(planet);
	planet.init(vec2(0.4, 0.4), vec2(0.25, 0.25));
	frames[2][0].AddPlanet(planet);

	planet.init(vec2(0.2, 0.2), vec2(0.2, 0.2));
	frames[2][1].AddPlanet(planet);
	planet.init(vec2(-0.7, 0.1), vec2(0.25, 0.25));
	frames[2][1].AddPlanet(planet);

	planet.init(vec2(0.2, 0.4), vec2(0.2, 0.2));
	frames[2][2].AddPlanet(planet);
	planet.init(vec2(-0.2, 0.4), vec2(0.25, 0.25));
	frames[2][2].AddPlanet(planet);

	return frames;
}
