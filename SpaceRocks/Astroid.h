#pragma once
#include "Sprite.h"

class Astroid : public Sprite
{
public:
	void init(vec2 pos, vec2 scale);

	void Move(Sprite actor);

private:
	float speed;
};

