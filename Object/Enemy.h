#pragma once
#include "GameObject.h"
class Enemy :public GameObject
{
private:
	class ModelRenderer* m_ModelRenderer;

	float m_Speed = 3.0f;
	Vector3 m_Velocity;
	bool m_Ground = true;
	float m_Gravity = 40.0f;

	Vector3 m_Shake{ 0.0f, 0.0f, 0.0f };
	Vector3 m_ShakeOffset{ 0.0f, 0.0f, 0.0f };	// 現在位置に乗っているシェイク分
	float m_ShakeTime = 0.0f;

	int m_Life = 0;

	bool m_Flash = false;
	float m_FlashTime = 0.0f;

public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void Shake(Vector3 Shake) 
	{
		m_Shake = Shake;
		m_ShakeTime = 0.0f;
	}

	void AddDamage(int Damage);
};

