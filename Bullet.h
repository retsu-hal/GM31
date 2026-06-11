#pragma once
#include "GameObject.h"
class Bullet :public GameObject
{
private:
	Vector3 m_Velocity{ 0.0f, 0.0f, 0.0f };

	float m_Lifetime = 3.0f; // 弾の寿命（秒）

public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void SetVelocity(const Vector3& velocity) { m_Velocity = velocity; }
};

