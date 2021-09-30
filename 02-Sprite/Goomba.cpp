#include "Goomba.h"
#include "Game.h"

Goomba::Goomba(float x, float y, float vx) :CGameObject(x, y)
{
	this->vx = vx;
};

void Goomba::Update(DWORD dt)
{
	x += vx * dt;

	int BackBufferWidth = CGame::GetInstance()->GetBackBufferWidth();
	if (x <= 0 || x >= BackBufferWidth - GOOMBA_WIDTH) {

		vx = -vx;

		if (x <= 0)
		{
			x = 0;
		}
		else if (x >= BackBufferWidth - GOOMBA_WIDTH)
		{
			x = (float)(BackBufferWidth - GOOMBA_WIDTH);
		}
	}
}

void Goomba::Render()
{
	LPANIMATION ani;

	//[RED FLAG][TODO]: Student needs to think about how to associate this animation/asset to Mario!!
	if (vx > 0) ani = CAnimations::GetInstance()->Get(601);
	else ani = CAnimations::GetInstance()->Get(602);

	ani->Render(x, y);
}
