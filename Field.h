#pragma once
#include "main.h"
#include "Vector3.h"
#include "GameObject.h"

class FIELD : public GameObject
{
	public:
	// í∏ì_ç\ë¢ëÃ
	struct Vertex3D {
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT4 Diffuse;
		XMFLOAT2 TexCoord;
	};

public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

};


