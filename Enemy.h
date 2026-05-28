#pragma once
#include "GameObject.h"
class Enemy :public GameObject
{
public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
};

