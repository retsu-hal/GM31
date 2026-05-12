#pragma once
#include "main.h"
#include "Vector3.h"
#include "GameObject.h"

class FIELD : public GameObject
{
	public:
	// 頂点構造体
	struct Vertex3D {
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
		XMFLOAT4 Diffuse;
		XMFLOAT2 TexCoord;
	};
private:
	
	ID3D11Buffer* m_vertexBuffer;					// 頂点バッファ

	ID3D11InputLayout* m_VertexLayout;		// 頂点レイアウト
	ID3D11VertexShader* m_VertexShader;		// 頂点シェーダー
	ID3D11PixelShader* m_PixelShader;			// ピクセルシェーダー	
	ID3D11ShaderResourceView* m_Texture;	// テクスチャ

public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

};


