#include "main.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Field.h"
#include "Audio.h"



void FIELD::Init()
{
	m_Layer = 1;
	m_Position = { 0.0f, 0.0f, 0.0f };
	m_Scale = {20.0f, 1.0f, 20.0f };
	static const XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

	static const VERTEX_3D vertex[] =
	{
		// Position                      Normal                TexCoord
		// 上面 (Y+)
		{ {-1.0f,  0.0f,  1.0f }, { 0.0f,  1.0f,  0.0f }, color, {  0.0f,  0.0f } },
		{ { 1.0f,  0.0f,  1.0f }, { 0.0f,  1.0f,  0.0f }, color, { 1.0f,  0.0f } },
		{ {-1.0f,  0.0f, -1.0f }, { 0.0f,  1.0f,  0.0f }, color, {  0.0f, 1.0f } },
		{ { 1.0f,  0.0f, -1.0f }, { 0.0f,  1.0f,  0.0f }, color, { 1.0f, 1.0f } },
	};

	m_Mesh.CreateVertex(vertex, _countof(vertex));

	m_Shader = ShaderManager::Load("shader\\unlitTextureVS.cso", "shader\\unlitTexturePS.cso");
	m_Texture = TextureManager::Load(L"asset\\texture\\iaigami.jpg");

	// カリング無効化（両面描画）のラスタライザステートを1回だけ生成する
	D3D11_RASTERIZER_DESC rasterDesc{};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;  // ← 裏面もカリングしない
	rasterDesc.FrontCounterClockwise = FALSE;
	rasterDesc.DepthClipEnable = TRUE;
	Renderer::GetDevice()->CreateRasterizerState(&rasterDesc, &m_RasterState);

	Audio* bgm = AddComponent<Audio>(this);
	bgm->Load("asset\\audio\\bgm.wav");
	bgm->Play(true);
}

void FIELD::Uninit()
{
	m_Mesh.Release();

	if (m_RasterState) m_RasterState->Release();

	GameObject::Uninit();
}

void FIELD::Draw()
{
	// 両面描画に切り替え。元のステートは描画後に戻す
	ID3D11RasterizerState* prevRasterState = nullptr;
	Renderer::GetDeviceContext()->RSGetState(&prevRasterState);	// RSGetStateはAddRefするので後でRelease
	Renderer::GetDeviceContext()->RSSetState(m_RasterState);

	m_Shader->Set();

	Renderer::SetWorldMatrix(GetWorldMatrix());
	Renderer::SetTextureMaterial();
	Renderer::SetTexture(m_Texture);

	m_Mesh.Bind(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	m_Mesh.Draw();

	Renderer::GetDeviceContext()->RSSetState(prevRasterState);
	if (prevRasterState) prevRasterState->Release();

	GameObject::Draw();
}
