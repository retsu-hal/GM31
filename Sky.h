#pragma once
#include "GameObject.h"
class Sky :public GameObject
{
public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
};

