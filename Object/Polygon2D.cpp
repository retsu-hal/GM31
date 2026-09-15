#include "main.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Polygon2D.h"

void Polygon2D::Init(float x, float y, float width, float height,const WCHAR* TextureName)
{
	m_Layer = 3;

	static const XMFLOAT3 normal = { 0.0f, 0.0f, 0.0f };
	static const XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

	const VERTEX_3D vertex[] =
	{
		{ { x,         y,          0.0f }, normal, color, { 0.0f, 0.0f } },
		{ { x + width, y,          0.0f }, normal, color, { 1.0f, 0.0f } },
		{ { x,         y + height, 0.0f }, normal, color, { 0.0f, 1.0f } },
		{ { x + width, y + height, 0.0f }, normal, color, { 1.0f, 1.0f } },
	};

	m_Mesh.CreateVertex(vertex, _countof(vertex));

	m_Shader = ShaderManager::Load("shader\\unlitTextureVS.cso", "shader\\unlitTexturePS.cso");
	m_Texture = TextureManager::Load(TextureName);
}

void Polygon2D::Uninit()
{
	m_Mesh.Release();

	GameObject::Uninit();
}

void Polygon2D::Draw()
{
	m_Shader->Set();

	//2D用のビュー・プロジェクション
	Renderer::SetWorldViewProjection2D();
	Renderer::SetWorldMatrix(GetWorldMatrix());
	Renderer::SetTextureMaterial();
	Renderer::SetTexture(m_Texture);

	m_Mesh.Bind(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_Mesh.Draw();

	GameObject::Draw();
}
