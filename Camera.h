#pragma once

#include "Vector3.h"
#include "GameObject.h"
class CAMERA : public GameObject
{
public:
	Vector3 m_Target;		// 注視点
	Vector3 m_Angle;			// カメラの角度
	float  m_Fov;					// カメラの視野角

public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
};
