#include "main.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Manager.h"
#include "Shadow.h"
#include "MeshField.h"

#define SHADOW_OFFSET_Y	(0.05f)		// 影を地面から浮かせる量（Zファイティング回避）


void Shadow::Init()
{
	m_Layer = 2;
	m_Position = { 0.0f, 0.0f, 0.0f };
	m_Scale = { 1.0f, 1.0f, 1.0f };

	// 頂点は毎フレーム地形の高さで作り直すので動的バッファにする
	for (int z = 0; z <= DIV; z++)
	{
		for (int x = 0; x <= DIV; x++)
		{
			m_Vertex[z][x].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
			m_Vertex[z][x].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			m_Vertex[z][x].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			m_Vertex[z][x].TexCoord = XMFLOAT2((float)x / DIV, (float)z / DIV);
		}
	}
	m_Mesh.CreateVertex(&m_Vertex[0][0], (DIV + 1) * (DIV + 1), true);

	// インデックスバッファ生成（DIV×DIVマスを三角形リストで張る）
	{
		unsigned int index[DIV * DIV * 6];

		int i = 0;
		for (int z = 0; z < DIV; z++)
		{
			for (int x = 0; x < DIV; x++)
			{
				unsigned int v0 = z * (DIV + 1) + x;			// 左上
				unsigned int v1 = v0 + 1;					// 右上
				unsigned int v2 = (z + 1) * (DIV + 1) + x;	// 左下
				unsigned int v3 = v2 + 1;					// 右下

				index[i++] = v0;
				index[i++] = v1;
				index[i++] = v2;

				index[i++] = v1;
				index[i++] = v3;
				index[i++] = v2;
			}
		}

		m_Mesh.CreateIndex(index, _countof(index));
	}

	// カリング無効化（両面描画）のラスタライザステートを1回だけ生成
	D3D11_RASTERIZER_DESC rasterDesc{};
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;  // ← 裏面もカリングしない
	rasterDesc.FrontCounterClockwise = FALSE;
	rasterDesc.DepthClipEnable = TRUE;
	// 地形に密着させるので、深度を少しだけ手前に寄せてZファイティングを防ぐ
	rasterDesc.DepthBias = -100;
	rasterDesc.SlopeScaledDepthBias = -1.0f;
	rasterDesc.DepthBiasClamp = 0.0f;
	Renderer::GetDevice()->CreateRasterizerState(&rasterDesc, &m_RasterState);

	m_Shader = ShaderManager::Load("shader\\unlitTextureVS.cso", "shader\\unlitTexturePS.cso");
	m_Texture = TextureManager::Load(L"asset\\texture\\Shadow.png");
}

void Shadow::Uninit()
{
	m_Mesh.Release();

	if (m_RasterState)	m_RasterState->Release();

	GameObject::Uninit();
}

//==============================================================================
//影メッシュを地形の高さに貼り付ける
//==============================================================================
void Shadow::UpdateMesh()
{
	MeshField* meshField = Manager::GetGameObject<MeshField>();

	for (int z = 0; z <= DIV; z++)
	{
		for (int x = 0; x <= DIV; x++)
		{
			// ローカル -1〜1 の広がりをワールド座標へ展開する
			Vector3 world;
			world.x = m_Position.x + ((float)x / DIV * 2.0f - 1.0f) * m_Scale.x;
			world.y = m_Position.y;
			world.z = m_Position.z + (1.0f - (float)z / DIV * 2.0f) * m_Scale.z;

			// 頂点ごとに地形の高さを拾うので、坂でも影が地面に埋まらない
			if (meshField)
				world.y = meshField->GetHeight(world) + SHADOW_OFFSET_Y;

			m_Vertex[z][x].Position = XMFLOAT3(world.x, world.y, world.z);
		}
	}

	m_Mesh.UpdateVertex(&m_Vertex[0][0], (DIV + 1) * (DIV + 1));
}

void Shadow::Draw()
{
	// SetPosition する側（Player）より確実に後で走るように、Updateではなく描画直前で作る
	UpdateMesh();

	// カリング無効化（両面描画）に切り替え。元のステートは描画後に戻す
	ID3D11RasterizerState* prevRasterState = nullptr;
	Renderer::GetDeviceContext()->RSGetState(&prevRasterState);	// RSGetStateはAddRefするので後でRelease
	Renderer::GetDeviceContext()->RSSetState(m_RasterState);

	m_Shader->Set();

	//頂点をワールド座標で作っているのでワールド行列は単位行列
	Renderer::SetWorldMatrix(XMMatrixIdentity());
	Renderer::SetTextureMaterial();
	Renderer::SetTexture(m_Texture);

	// 深度書き込みだけOFF（深度テストは残すので、手前の山にはちゃんと隠れる）
	Renderer::SetDepthEnable(false);

	m_Mesh.Bind(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_Mesh.Draw();

	Renderer::SetDepthEnable(true);

	//ラスタライザステートを元に戻す（以降の描画がCULL_NONEのままになるのを防ぐ）
	Renderer::GetDeviceContext()->RSSetState(prevRasterState);
	if (prevRasterState)	prevRasterState->Release();

	GameObject::Draw();
}
