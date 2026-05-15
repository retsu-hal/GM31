#pragma once
#include "main.h"
#include "Vector3.h"
#include "GameObject.h"

class CUBE : public GameObject
{
private:
	Vector3 m_Velocity;		
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
	ID3D11Buffer* m_indexBuffer;					// インデックスバッファ
	ID3D11ShaderResourceView* m_Texture;	// テクスチャ

	bool m_isRotating = true;

public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

};