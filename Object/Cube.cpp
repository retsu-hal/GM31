#include "main.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Cube.h"



void CUBE::Init()
{
	m_Position = { 0.0f, 10.0f, 0.0f };
	m_Scale = { 1.0f, 1.0f, 1.0f };
	m_RotSpeed = { 0.1f, 0.0f, 0.1f };

	static const XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

	static const VERTEX_3D vertex[] =
	{
		// Position                      Normal                TexCoord
		// 上面 (Y+)　サイコロ（１）
		{ {-1.0f,  1.0f,  1.0f }, { 0.0f,  1.0f,  0.0f }, color, {  0.25f,  0.0f } },
		{ { 1.0f,  1.0f,  1.0f }, { 0.0f,  1.0f,  0.0f }, color, { 0.5f,  0.0f } },
		{ {-1.0f,  1.0f, -1.0f }, { 0.0f,  1.0f,  0.0f }, color, {  0.25f, 0.333f } },
		{ { 1.0f,  1.0f, -1.0f }, { 0.0f,  1.0f,  0.0f }, color, { 0.5f, 0.333f } },

		// 下面 (Y-)　サイコロ（６）
		{ {1.0f, -1.0f,  1.0f }, { 0.0f, -1.0f,  0.0f }, color, {  0.25f,  0.666f } },
		{ {-1.0f, -1.0f,  1.0f }, { 0.0f, -1.0f,  0.0f }, color, { 0.50f,  0.666f } },
		{ {1.0f, -1.0f, -1.0f }, { 0.0f, -1.0f,  0.0f }, color, {  0.25f, 1.0f } },
		{ {- 1.0f, -1.0f, -1.0f }, { 0.0f, -1.0f,  0.0f }, color, { 0.5f, 1.0f } },

		// 前面 (Z+) サイコロ(２)
		{ {1.0f,  1.0f,  1.0f }, { 0.0f,  0.0f,  1.0f }, color, {  0.0f,  0.333f } },
		{ {-1.0f, 1.0f,  1.0f }, { 0.0f,  0.0f,  1.0f }, color, { 0.25f,  0.333f } },
		{ { 1.0f,  -1.0f,  1.0f }, { 0.0f,  0.0f,  1.0f }, color, {  0.0f, 0.666f } },
		{ { -1.0f, -1.0f,  1.0f }, { 0.0f,  0.0f,  1.0f }, color, { 0.25f, 0.666f } },

		// 後面 (Z-) サイコロ（５）
		{ {-1.0f,  1.0f, -1.0f }, { 0.0f,  0.0f, -1.0f }, color, {  0.75f,  0.333f } },
		{ { 1.0f,  1.0f, -1.0f }, { 0.0f,  0.0f, -1.0f }, color, { 0.75f,  0.666f } },
		{ {-1.0f, -1.0f, -1.0f }, { 0.0f,  0.0f, -1.0f }, color, {  1.0f, 0.333f } },
		{ { 1.0f, -1.0f, -1.0f }, { 0.0f,  0.0f, -1.0f }, color, { 1.0f, 0.666f } },

		// 右面 (X+) サイコロ（３）
		{ { 1.0f,  1.0f, -1.0f }, { 1.0f,  0.0f,  0.0f }, color, {  0.25f,  0.333f } },
		{ { 1.0f,  1.0f,  1.0f }, { 1.0f,  0.0f,  0.0f }, color, { 0.5f,  0.333f } },
		{ { 1.0f, -1.0f, -1.0f }, { 1.0f,  0.0f,  0.0f }, color, {  0.25f, 0.666f } },
		{ { 1.0f, -1.0f,  1.0f }, { 1.0f,  0.0f,  0.0f }, color, { 0.5f, 0.666f } },

		// 左面 (X-)
		{ {-1.0f,  1.0f,  1.0f }, { -1.0f, 0.0f,  0.0f }, color, {  0.5f,  0.333f } },
		{ {-1.0f,  1.0f, -1.0f }, { -1.0f, 0.0f,  0.0f }, color, { 0.75f,  0.333f } },
		{ {-1.0f, -1.0f,  1.0f }, { -1.0f, 0.0f,  0.0f }, color, {  0.5f, 0.666f } },
		{ {-1.0f, -1.0f, -1.0f }, { -1.0f, 0.0f,  0.0f }, color, { 0.75f, 0.666f } },
	};

	// 面ごとに四角形（頂点4つ）を三角形2枚に張る
	WORD index[36];
	for (int i = 0; i < 6; i++)
	{
		int base = i * 4;
		index[i * 6 + 0] = base + 0;
		index[i * 6 + 1] = base + 1;
		index[i * 6 + 2] = base + 2;
		index[i * 6 + 3] = base + 2;
		index[i * 6 + 4] = base + 1;
		index[i * 6 + 5] = base + 3;
	}

	m_Mesh.CreateVertex(vertex, _countof(vertex));
	m_Mesh.CreateIndex(index, _countof(index));

	m_Shader = ShaderManager::Load("shader\\unlitTextureVS.cso", "shader\\unlitTexturePS.cso");
	m_Texture = TextureManager::Load(L"asset\\texture\\dice.jpg");
}

void CUBE::Update()
{
	m_Rotation += m_RotSpeed;

	GameObject::Update();
}

void CUBE::Draw()
{
	m_Shader->Set();

	Renderer::SetWorldMatrix(GetWorldMatrix());
	Renderer::SetTextureMaterial();
	Renderer::SetTexture(m_Texture);

	m_Mesh.Bind(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_Mesh.Draw();

	GameObject::Draw();
}
