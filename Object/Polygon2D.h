#pragma once

#include "GameObject.h"
#include "Mesh.h"

struct ShaderSet;

class Polygon2D : public GameObject
{
private:
	Mesh m_Mesh;
	const ShaderSet* m_Shader = nullptr;
	ID3D11ShaderResourceView* m_Texture = nullptr;

public:
	void Init() override{}
	void Init(float x, float y, float width, float height, const WCHAR* TextureName);
	void Uninit() override;
	void Draw() override;
};
