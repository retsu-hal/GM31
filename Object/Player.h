#pragma once
#include "Vector3.h"
#include "GameObject.h"

class Player : public GameObject
{
private:
	Vector3 m_Velocity;
	float m_Speed ;
	float m_jumpPower ;
	float m_Gravity;
	float m_Friction = 5.0f;
	bool m_Ground = true;
	float m_MoveAnimation = 0.0f;
	float m_HitTimer = 0.0f;
	class Audio* m_JumpSE = nullptr;

	class AnimationModel* m_animationModel;
	int m_AnimationFrame = 0;

public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

};



