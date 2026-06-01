#include "main.h"
#include "renderer.h"
#include "Polygon2D.h"

void Polygon2D::Init()
{
	VERTEX_3D vertex[4];

	{
		vertex[0].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);
		vertex[0].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);

		vertex[1].Position = XMFLOAT3(200.0f, 0.0f, 0.0f);
		vertex[1].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].TexCoord = XMFLOAT2(1.0f, 0.0f);

		vertex[2].Position = XMFLOAT3(000.0f, 200.0f, 0.0f);
		vertex[2].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].TexCoord = XMFLOAT2(0.0f, 1.0f);

		vertex[3].Position = XMFLOAT3(200.0f, 200.0f, 0.0f);
		vertex[3].Normal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].TexCoord = XMFLOAT2(1.0f, 1.0f);
	}

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA sd{};
	sd.pSysMem = vertex;

	Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_vertexBuffer);
	//シェーダー読み込み
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");
	
	//テクスチャ読み込み
	TexMetadata metadata;
	ScratchImage image;
	LoadFromWICFile(L"asset\\texture\\iaigami.jpg", WIC_FLAGS_NONE, &metadata, image);//テクスチャは変更可
	CreateShaderResourceView(Renderer::GetDevice(), image.GetImages(),
		image.GetImageCount(), metadata, &m_Texture);
	assert(m_Texture);//読み込み失敗時にダイアログを表示
}

void Polygon2D::Uninit()
{
	m_vertexBuffer->Release();

	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
	m_Texture->Release();
}

void Polygon2D::Update()
{
}

void Polygon2D::Draw()
{
	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダー設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//マトリックス設定
	Renderer::SetWorldViewProjection2D();

	XMMATRIX WorldMatrix,ScaleMatrix,RotMatrix,TransMatrix;
	ScaleMatrix = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);								//拡大縮小
	RotMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);		//回転
	TransMatrix = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);						//平行移動
	WorldMatrix = ScaleMatrix * RotMatrix * TransMatrix;

	Renderer::SetWorldMatrix(WorldMatrix);

	//マテリアル設定
	MATERIAL material{};
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	material.TextureEnable = true;			//true:テクスチャを使用する、false:テクスチャを使用しない
	Renderer::SetMaterial(material);


	//テクスチャ設定
	Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);
	//頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	//プリミティブトポロジ設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//描画
	Renderer::GetDeviceContext()->Draw(4, 0);
}

void Polygon2D::DrawImGui()
{
	float w = ImGui::CalcItemWidth() * 0.5f - 4.0f;

	ImGui::PushItemWidth(w);
	ImGui::SliderFloat("##PosX", &m_Position.x, 0.0f, SCREEN_WIDTH - 200.0f, "X %.1f");
	ImGui::SameLine();
	ImGui::SliderFloat("##PosY", &m_Position.y, 0.0f, SCREEN_HEIGHT - 200.0f, "Y %.1f");
	ImGui::PopItemWidth();
	ImGui::SameLine(); ImGui::Text("Position");

	w = (ImGui::CalcItemWidth() - ImGui::GetStyle().ItemSpacing.x * 2.0f) / 3.0f;

	ImGui::PushItemWidth(w);
	ImGui::SliderFloat("##RotX", &m_Rotation.x, 0.0f, 360.0f, "X %.1f"); ImGui::SameLine();
	ImGui::SliderFloat("##RotY", &m_Rotation.y, 0.0f, 360.0f, "Y %.1f"); ImGui::SameLine();
	ImGui::SliderFloat("##RotZ", &m_Rotation.z, 0.0f, 360.0f, "Z %.1f");
	ImGui::PopItemWidth();
	ImGui::SameLine(); ImGui::Text("Rotation");


	ImGui::PushItemWidth(w);
	ImGui::SliderFloat("##ScaleX", &m_Scale.x, 0.0f, 1.0f, "X %.1f"); ImGui::SameLine();
	ImGui::SliderFloat("##ScaleY", &m_Scale.y, 0.0f, 1.0f, "Y %.1f"); ImGui::SameLine();
	ImGui::SliderFloat("##ScaleZ", &m_Scale.z, 0.0f, 1.0f, "Z %.1f"); ImGui::SameLine();
	ImGui::PopItemWidth();
	ImGui::SameLine(); ImGui::Text("Scale");
}
