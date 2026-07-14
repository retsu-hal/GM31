#pragma once
#include "GameObject.h"
class Enemy :public GameObject
{
private:
	float m_Speed = 3.0f;
	Vector3 m_Velocity;
	bool m_Ground = true;
	float m_Gravity = 40.0f;
public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
};

