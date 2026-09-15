#pragma once
#include "main.h"
#include "Renderer.h"

//==============================================================================
// 描画用メッシュ
// 頂点バッファ・インデックスバッファの生成／書き換え／描画をまとめて持つ。
// DirectXの手続きをここに閉じ込めることで、GameObject側は「形のデータ」だけを扱えばよくなる。
//==============================================================================
class Mesh
{
private:
	ID3D11Buffer* m_VertexBuffer = nullptr;
	ID3D11Buffer* m_IndexBuffer = nullptr;
	UINT m_VertexCount = 0;
	UINT m_IndexCount = 0;
	DXGI_FORMAT m_IndexFormat = DXGI_FORMAT_R32_UINT;

public:
	Mesh() = default;
	~Mesh() { Release(); }

	//コピーすると同じバッファを二重解放してしまうので禁止
	Mesh(const Mesh&) = delete;
	Mesh& operator=(const Mesh&) = delete;

	//------------------------------------------------------------
	// 生成・解放
	//------------------------------------------------------------
	//dynamic = true にすると Map() で毎フレーム書き換えられる（vertex は nullptr でも可）
	void CreateVertex(const VERTEX_3D* vertex, UINT count, bool dynamic = false);
	void CreateIndex(const unsigned int* index, UINT count);
	void CreateIndex(const WORD* index, UINT count);
	void Release();

	//------------------------------------------------------------
	// 動的頂点バッファの書き換え
	//------------------------------------------------------------
	VERTEX_3D* Map() const;					//書き換え開始（中身は破棄される）
	void Unmap() const;						//書き換え終了
	void UpdateVertex(const VERTEX_3D* vertex, UINT count) const;	//まとめて流し込む

	//------------------------------------------------------------
	// 描画
	// Bind でバッファをデバイスに設定し、Draw で描画命令を出す。
	// 同じメッシュを位置違いで何度も描くときは Bind 1回＋Draw 複数回でよい。
	//------------------------------------------------------------
	void Bind(D3D11_PRIMITIVE_TOPOLOGY topology) const;
	void Draw() const;							//メッシュ全体
	void Draw(UINT count, UINT start) const;	//一部だけ（インデックスがあればインデックス番号）
};
