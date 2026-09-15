/*==============================================================================

[Mesh.cpp]
														Author :Watanabe Retsu
														Date   :2026/09/16
--------------------------------------------------------------------------------
・頂点／インデックスバッファの生成・書き換え・描画

==============================================================================*/

//==============================================================================
//インクルード
//==============================================================================
#include "main.h"
#include "Renderer.h"
#include "Mesh.h"

//==============================================================================
//生成
//==============================================================================
void Mesh::CreateVertex(const VERTEX_3D* vertex, UINT count, bool dynamic)
{
	if (m_VertexBuffer) m_VertexBuffer->Release();

	D3D11_BUFFER_DESC bd{};
	bd.Usage = dynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * count;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = dynamic ? D3D11_CPU_ACCESS_WRITE : 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertex;

	Renderer::GetDevice()->CreateBuffer(&bd, vertex ? &sd : nullptr, &m_VertexBuffer);
	m_VertexCount = count;
}

void Mesh::CreateIndex(const unsigned int* index, UINT count)
{
	if (m_IndexBuffer) m_IndexBuffer->Release();

	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(unsigned int) * count;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = index;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_IndexBuffer);
	m_IndexCount = count;
	m_IndexFormat = DXGI_FORMAT_R32_UINT;
}

void Mesh::CreateIndex(const WORD* index, UINT count)
{
	if (m_IndexBuffer) m_IndexBuffer->Release();

	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(WORD) * count;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = index;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_IndexBuffer);
	m_IndexCount = count;
	m_IndexFormat = DXGI_FORMAT_R16_UINT;
}

//==============================================================================
//解放
//==============================================================================
void Mesh::Release()
{
	if (m_VertexBuffer)
	{
		m_VertexBuffer->Release();
		m_VertexBuffer = nullptr;
	}
	if (m_IndexBuffer)
	{
		m_IndexBuffer->Release();
		m_IndexBuffer = nullptr;
	}
	m_VertexCount = 0;
	m_IndexCount = 0;
}

//==============================================================================
//動的頂点バッファの書き換え
//==============================================================================
VERTEX_3D* Mesh::Map() const
{
	D3D11_MAPPED_SUBRESOURCE msr{};
	Renderer::GetDeviceContext()->Map(m_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	return (VERTEX_3D*)msr.pData;
}

void Mesh::Unmap() const
{
	Renderer::GetDeviceContext()->Unmap(m_VertexBuffer, 0);
}

void Mesh::UpdateVertex(const VERTEX_3D* vertex, UINT count) const
{
	VERTEX_3D* dest = Map();
	memcpy(dest, vertex, sizeof(VERTEX_3D) * count);
	Unmap();
}

//==============================================================================
//描画
//==============================================================================
void Mesh::Bind(D3D11_PRIMITIVE_TOPOLOGY topology) const
{
	ID3D11DeviceContext* context = Renderer::GetDeviceContext();

	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	if (m_IndexBuffer) context->IASetIndexBuffer(m_IndexBuffer, m_IndexFormat, 0);

	context->IASetPrimitiveTopology(topology);
}

void Mesh::Draw() const
{
	Draw(m_IndexBuffer ? m_IndexCount : m_VertexCount, 0);
}

void Mesh::Draw(UINT count, UINT start) const
{
	if (m_IndexBuffer) Renderer::GetDeviceContext()->DrawIndexed(count, start, 0);
	else               Renderer::GetDeviceContext()->Draw(count, start);
}
