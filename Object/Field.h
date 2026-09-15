#pragma once
#include "main.h"
#include "GameObject.h"
#include "Mesh.h"

struct ShaderSet;

class FIELD : public GameObject
{
private:
	Mesh m_Mesh;
	const ShaderSet* m_Shader = nullptr;
	ID3D11ShaderResourceView* m_Texture = nullptr;
	ID3D11RasterizerState* m_RasterState = nullptr;	// 両面描画用（Initで1回だけ生成）

public:
	void Init() override;
	void Uninit() override;
	void Draw() override;
};

