#pragma once

#include "GameObject.h"

#include "Animation.h"
#include "Animations.h"

#define GOOMBA_WIDTH 14

class Goomba : public CGameObject
{
protected:
	float vx;
public:
	Goomba(float x, float y, float vx);
	void Update(DWORD dt);
	void Render();
};
