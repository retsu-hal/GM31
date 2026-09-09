/*==============================================================================

[MeshField.cpp]
														Author :Watanabe Retsu
														Date   :2026/09/09
--------------------------------------------------------------------------------

==============================================================================*/

//==============================================================================
//インクルード
//==============================================================================
#include"main.h"
#include"renderer.h"
#include"meshField.h"
#include"audio.h"

//==============================================================================
//マクロ宣言
//==============================================================================

//==============================================================================
//プロトタイプ宣言
//==============================================================================

//==============================================================================
//グローバル変数
//==============================================================================
float g_FieldHeight[21][21] =
{
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}
};

//==============================================================================
//初期化処理
//==============================================================================
void MeshField::Init()
{
    m_Layer = 1;

    // 頂点バッファ生成
    {
        

        for (int x = 0; x < 21; x++)
        {
            for (int z = 0; z < 21; z++)
            {
                m_Vertex[x][z].Position = XMFLOAT3((x - 10) * 5.0f, g_FieldHeight[z][x], (z - 10) * -5.0f);
                m_Vertex[x][z].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
                m_Vertex[x][z].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
                m_Vertex[x][z].TexCoord = XMFLOAT2((float)x, (float)z);
            }
        }

        for (int x = 1; x < 20; x++)
        {
            for (int z = 1; z < 20; z++)
            {
                Vector3 vx, vz, vn;

                vx.x = m_Vertex[x + 1][z].Position.x - m_Vertex[x - 1][z].Position.x;
                vx.y = m_Vertex[x + 1][z].Position.y - m_Vertex[x - 1][z].Position.y;
                vx.z = m_Vertex[x + 1][z].Position.z - m_Vertex[x - 1][z].Position.z;

                vz.x = m_Vertex[x][z - 1].Position.x - m_Vertex[x][z + 1].Position.x;
                vz.y = m_Vertex[x][z - 1].Position.y - m_Vertex[x][z + 1].Position.y;
                vz.z = m_Vertex[x][z - 1].Position.z - m_Vertex[x][z + 1].Position.z;

                vn = Vector3::cross(vz, vx); // 外積
                vn.normalize(); // 正規化

                m_Vertex[x][z].Normal.x = vn.x;
                m_Vertex[x][z].Normal.y = vn.y;
                m_Vertex[x][z].Normal.z = vn.z;
            }
        }

        // 頂点バッファ生成
        D3D11_BUFFER_DESC bd{};
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(VERTEX_3D) * 21 * 21;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA sd{};
        sd.pSysMem = m_Vertex;

        Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_vertexBuffer);
    }

    // インデックスバッファ生成
    {
        unsigned int index[(22 * 2) * 20 - 2];

        int i = 0;

        for (int x = 0; x < 20; x++)
        {
            for (int z = 0; z < 21; z++)
            {
                index[i] = x * 21 + z;
                i++;

                index[i] = (x + 1) * 21 + z;
                i++;
            }

            if (x == 19)
                break;

            // 縮退ポリゴン
            index[i] = (x + 1) * 21 + 20;
            i++;

            index[i] = (x + 1) * 21;
            i++;
        }

        // 頂点バッファ生成
        D3D11_BUFFER_DESC bd{};
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(unsigned int) * ((22 * 2) * 20 - 2);
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;

        D3D11_SUBRESOURCE_DATA sd{};
        sd.pSysMem = index;

        Renderer::GetDevice()->CreateBuffer(&bd, &sd, &m_indexBuffer);
    }

    // シェーダー読込
    Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\fieldVS.cso");
    Renderer::CreatePixelShader(&m_PixelShader, "shader\\fieldPS.cso");

    // テクスチャ読み込み
    TexMetadata metadata;
    ScratchImage image;
    LoadFromWICFile(L"asset\\texture\\grass.jpg", WIC_FLAGS_NONE, &metadata, image);
    CreateShaderResourceView(Renderer::GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &m_Texture);
    assert(m_Texture);

    Audio* bgm = AddComponent<Audio>(this);
    bgm->Load("asset\\audio\\bgm.wav");
    //bgm->Play(true);
}

//==============================================================================
//終了処理
//==============================================================================
void MeshField::Uninit()
{
    m_Texture->Release();

    m_vertexBuffer->Release();
    m_indexBuffer->Release();

    m_VertexLayout->Release();
    m_VertexShader->Release();
    m_PixelShader->Release();

    GameObject::Uninit();
}

//==============================================================================
//更新処理
//==============================================================================
void MeshField::Update()
{
    GameObject::Update();
}

//==============================================================================
//描画処理
//==============================================================================
void MeshField::Draw()
{
    // 入力レイアウト設定
    Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

    // シェーダ設定
    Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
    Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

    // マトリクス設定
    XMMATRIX world, scale, rot, trans;
    scale = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
    rot = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y, m_Rotation.z);
    trans = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
    world = scale * rot * trans;

    Renderer::SetWorldMatrix(world);

    // マテリアル設定
    MATERIAL material{};
    material.Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
    material.TextureEnable = true;
    Renderer::SetMaterial(material);

    // テクスチャ設定
    Renderer::GetDeviceContext()->PSSetShaderResources(0, 1, &m_Texture);

    // 頂点バッファ設定
    UINT stride = sizeof(VERTEX_3D);
    UINT offset = 0;
    Renderer::GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

    // インデックスバッファ設定
    Renderer::GetDeviceContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // プリミティブトポロジ設定
    Renderer::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    // ポリゴン描画
    // Renderer::GetDeviceContext()->Draw(21 * 21, 0);
    Renderer::GetDeviceContext()->DrawIndexed((22 * 2) * 20 - 2, 0, 0);
}

float MeshField::GetHeight(Vector3 position)
{
    int x, z;
    
    //ブロック番号算出
    x = (int)(position.x / 10.0f + 10.0f);
	z = (int)(position.z / 10.0f + 10.0f);

	XMFLOAT3 pos0, pos1, pos2, pos3;

	pos0 = m_Vertex[x][z].Position;
	pos1 = m_Vertex[x + 1][z].Position;
	pos2 = m_Vertex[x][z + 1].Position;
	pos3 = m_Vertex[x + 1][z + 1].Position;


    return 0.0f;
}
