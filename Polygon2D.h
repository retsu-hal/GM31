#pragma once

#include "GameObject.h"

class Polygon2D : public GameObject
{
public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override	;

	const char* GetName() const override { return "Polygon2D"; }
	void DrawImGui() override;
};