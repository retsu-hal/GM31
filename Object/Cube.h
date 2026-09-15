#pragma once
#include "main.h"
#include "Vector3.h"
#include "GameObject.h"
#include "Mesh.h"

struct ShaderSet;

class CUBE : public GameObject
{
private:
	Mesh m_Mesh;
	const ShaderSet* m_Shader = nullptr;
	ID3D11ShaderResourceView* m_Texture = nullptr;

public:
	Vector3 m_RotSpeed;

public:
	void Init() override;
	void Update() override;
	void Draw() override;
};
