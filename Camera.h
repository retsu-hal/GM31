#pragma once

#include "main.h"
#include "renderer.h"
#include "Vector3.h"
#include "GameObject.h"
class CAMERA : public GameObject
{
public:
	Vector3 m_Target;		// 注視点
	Vector3 m_Angle;			// カメラの角度
	float  m_Fov;					// カメラの視野角
	float m_MoveSpeed ;

public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	//ImGui関連
	const char* GetName() const  override{ return "Camera"; }
	void DrawImGui() override;
	
	void SetPosition(Vector3 position);
	Vector3 GetPosition();

	void SetTarget(Vector3 target);
	Vector3 GetTarget();

	void SetAngle(Vector3 angle);
	Vector3 GetAngle();

	void SetFov(float fov);
	float GetFov();
};
