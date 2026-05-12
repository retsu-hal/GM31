#include "main.h"
#include "renderer.h"
#include "Cube.h"



void CUBE::Init()
{
		VERTEX_3D vertex[24];

		{
			// 上面 (Y+)
			vertex[0].Position = XMFLOAT3(-95.0f, 0.0f, 30.0f);
			vertex[0].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertex[0].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[0].TexCoord = XMFLOAT2(0.0f, 0.0f);

			vertex[1].Position = XMFLOAT3(-35.0f, 0.0f, 30.0f);
			vertex[1].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertex[1].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[1].TexCoord = XMFLOAT2(10.0f, 0.0f);

			vertex[2].Position = XMFLOAT3(-95.0f, 0.0f, -30.0f);
			vertex[2].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertex[2].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[2].TexCoord = XMFLOAT2(0.0f, 10.0f);

			vertex[3].Position = XMFLOAT3(-35.0f, 0.0f, -30.0f);
			vertex[3].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertex[3].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[3].TexCoord = XMFLOAT2(10.0f, 10.0f);

			// 下面 (Y-)
			vertex[4].Position = XMFLOAT3(-35.0f, -60.0f, 30.0f);
			vertex[4].Normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
			vertex[4].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[4].TexCoord = XMFLOAT2(0.0f, 0.0f);

			vertex[5].Position = XMFLOAT3(-95.0f, -60.0f, 30.0f);
			vertex[5].Normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
			vertex[5].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[5].TexCoord = XMFLOAT2(10.0f, 0.0f);

			vertex[6].Position = XMFLOAT3(-35.0f, -60.0f, -30.0f);
			vertex[6].Normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
			vertex[6].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[6].TexCoord = XMFLOAT2(0.0f, 10.0f);

			vertex[7].Position = XMFLOAT3(-95.0f, -60.0f, -30.0f);
			vertex[7].Normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
			vertex[7].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[7].TexCoord = XMFLOAT2(10.0f, 10.0f);

			// 前面 (Z+)
			vertex[8].Position = XMFLOAT3(-35.0f, 0.0f, 30.0f);
			vertex[8].Normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
			vertex[8].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[8].TexCoord = XMFLOAT2(0.0f, 0.0f);

			vertex[9].Position = XMFLOAT3(-95.0f, 0.0f, 30.0f);
			vertex[9].Normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
			vertex[9].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[9].TexCoord = XMFLOAT2(10.0f, 0.0f);

			vertex[10].Position = XMFLOAT3(-35.0f, -60.0f, 30.0f);
			vertex[10].Normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
			vertex[10].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[10].TexCoord = XMFLOAT2(0.0f, 10.0f);

			vertex[11].Position = XMFLOAT3(-95.0f, -60.0f, 30.0f);
			vertex[11].Normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
			vertex[11].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[11].TexCoord = XMFLOAT2(10.0f, 10.0f);

			// 後面 (Z-)
			vertex[12].Position = XMFLOAT3(-95.0f, 0.0f, -30.0f);
			vertex[12].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
			vertex[12].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[12].TexCoord = XMFLOAT2(0.0f, 0.0f);

			vertex[13].Position = XMFLOAT3(-35.0f, 0.0f, -30.0f);
			vertex[13].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
			vertex[13].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[13].TexCoord = XMFLOAT2(10.0f, 0.0f);

			vertex[14].Position = XMFLOAT3( -95.0f, -60.0f, -30.0f);
			vertex[14].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
			vertex[14].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[14].TexCoord = XMFLOAT2(0.0f, 10.0f);

			vertex[15].Position = XMFLOAT3(-35.0f, -60.0f, -30.0f);
			vertex[15].Normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
			vertex[15].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[15].TexCoord = XMFLOAT2(10.0f, 10.0f);

			// 右面 (X+)
			vertex[16].Position = XMFLOAT3(-35.0f, 0.0f, -30.0f);
			vertex[16].Normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
			vertex[16].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[16].TexCoord = XMFLOAT2(0.0f, 0.0f);

			vertex[17].Position = XMFLOAT3(-35.0f, 0.0f, 30.0f);
			vertex[17].Normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
			vertex[17].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[17].TexCoord = XMFLOAT2(10.0f, 0.0f);

			vertex[18].Position = XMFLOAT3(-35.0f, -60.0f, -30.0f);
			vertex[18].Normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
			vertex[18].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[18].TexCoord = XMFLOAT2(0.0f, 10.0f);

			vertex[19].Position = XMFLOAT3(-35.0f, -60.0f, 30.0f);
			vertex[19].Normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
			vertex[19].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[19].TexCoord = XMFLOAT2(10.0f, 10.0f);

			// 左面 (X-)
			vertex[20].Position = XMFLOAT3(-95.0f, 0.0f, 30.0f);
			vertex[20].Normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
			vertex[20].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[20].TexCoord = XMFLOAT2(0.0f, 0.0f);

			vertex[21].Position = XMFLOAT3(-95.0f, 0.0f, -30.0f);
			vertex[21].Normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
			vertex[21].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[21].TexCoord = XMFLOAT2(10.0f, 0.0f);

			vertex[22].Position = XMFLOAT3(-95.0f, -60.0f, 30.0f);
			vertex[22].Normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
			vertex[22].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[22].TexCoord = XMFLOAT2(0.0f, 10.0f);

			vertex[23].Position = XMFLOAT3(-95.0f, -60.0f, -30.0f);
			vertex[23].Normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
			vertex[23].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			vertex[23].TexCoord = XMFLOAT2(10.0f, 10.0f);
		}

	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(VERTEX_3D) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	// インデックスバッファ生成
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
	{
		D3D11_BUFFER_DESC ibd{};
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.ByteWidth = sizeof(WORD) * 36;
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA isd{};
		isd.pSysMem = index;

		Renderer::GetDevice()->CreateBuffer(&ibd, &isd, &m_indexBuffer);

	}

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

void CUBE::Uninit()
{
	m_vertexBuffer->Release();

	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();
	m_indexBuffer->Release();
	m_Texture->Release();
}

void CUBE::Update()
{

}

void CUBE::Draw()
{
	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダー設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//マトリックス設定
	XMMATRIX WorldMatrix, ScaleMatrix, RotMatrix, TransMatrix;
	ScaleMatrix = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	RotMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	TransMatrix = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
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
	Renderer::GetDeviceContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	//プリミティブトポロジ設定
	Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//描画
	Renderer::GetDeviceContext()->DrawIndexed(36, 0, 0);
}