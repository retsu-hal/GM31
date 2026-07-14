#pragma once
#include "GameObject.h"
class Explosion :public GameObject
{
public:
	int  m_Frame = 0;
	
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

};

