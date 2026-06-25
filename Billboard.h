#pragma once
#include "GameObject.h"

// 常にカメラへ正対する板ポリゴン（ビルボード）
class Billboard : public GameObject
{
public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
};
