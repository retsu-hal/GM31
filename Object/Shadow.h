#pragma once
#include "main.h"
#include "Vector3.h"
#include "GameObject.h"

class Shadow : public GameObject
{
private:
	ID3D11RasterizerState* m_RasterState = nullptr;	// 両面描画用（Initで1回だけ生成）

public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
};


