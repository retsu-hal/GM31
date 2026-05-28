#pragma once
#include "main.h"
#include "Vector3.h"
#include "GameObject.h"
class Player : public GameObject
{
private:
	Vector3 m_Velocity;
	float m_Speed = 10.0f;
	float m_jumpPower = 10.0f;
	float m_Gravity = 9.8f;
	float m_Friction = 5.0f;

	//class ModelRenderer* m_ModelRenderer;

public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

};



