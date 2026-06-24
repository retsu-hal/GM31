#pragma once
#include "Scene.h"
class ResultScene :public Scene
{
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
};

