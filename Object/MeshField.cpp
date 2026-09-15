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
#include"ShaderManager.h"
#include"TextureManager.h"
#include"meshField.h"
#include"audio.h"

//==============================================================================
//マクロ宣言
//==============================================================================
#define FIELD_STEP	(5.0f)					// 頂点どうしの間隔
#define FIELD_HALF	(DIV / 2)				// 中心のマス番号（原点）

//==============================================================================
//プロトタイプ宣言
//==============================================================================

//==============================================================================
//グローバル変数
//==============================================================================
static const float g_FieldHeight[21][21] =
{
    {5.0f, 6.3f, 7.2f, 7.5f, 7.2f, 6.3f, 4.9f, 3.4f, 3.1f, 3.9f, 4.3f, 4.2f, 3.6f, 2.7f, 2.4f, 2.8f, 2.9f, 2.5f, 1.9f, 1.2f, 0.5f},
    {5.6f, 7.1f, 8.0f, 8.4f, 8.0f, 7.0f, 5.6f, 4.0f, 3.4f, 4.2f, 4.6f, 4.5f, 3.8f, 2.8f, 3.5f, 4.1f, 4.2f, 3.8f, 3.1f, 2.1f, 1.2f},
    {5.9f, 7.3f, 8.2f, 8.6f, 8.2f, 7.3f, 5.8f, 4.2f, 3.3f, 4.0f, 4.4f, 4.2f, 3.5f, 3.4f, 4.5f, 5.2f, 5.3f, 5.0f, 4.1f, 3.0f, 1.9f},
    {5.6f, 6.9f, 7.8f, 8.1f, 7.8f, 6.9f, 5.6f, 4.1f, 2.7f, 3.4f, 3.7f, 3.5f, 2.8f, 3.9f, 5.0f, 5.8f, 6.1f, 5.8f, 4.9f, 3.7f, 2.4f},
    {5.0f, 6.1f, 6.8f, 7.1f, 6.8f, 6.1f, 5.0f, 3.6f, 2.1f, 1.9f, 2.0f, 1.9f, 2.3f, 3.9f, 5.1f, 6.0f, 6.3f, 6.0f, 5.3f, 4.1f, 2.8f},
    {4.0f, 4.9f, 5.4f, 5.7f, 5.5f, 4.9f, 3.6f, 2.0f, 0.9f, 0.5f, 0.4f, 0.4f, 1.1f, 2.4f, 4.3f, 5.6f, 5.9f, 5.7f, 5.1f, 4.0f, 2.7f},
    {2.8f, 3.4f, 3.9f, 4.0f, 3.9f, 3.1f, 1.7f, 0.6f, 0.1f, 0.0f, 0.0f, 0.0f, 0.1f, 0.8f, 2.3f, 4.2f, 5.0f, 4.9f, 4.4f, 3.5f, 2.4f},
    {1.7f, 2.1f, 2.4f, 2.5f, 2.3f, 1.4f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.7f, 2.3f, 3.7f, 3.8f, 3.4f, 2.7f, 2.0f},
    {0.9f, 1.0f, 1.2f, 1.2f, 1.0f, 0.5f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 1.0f, 2.2f, 2.5f, 2.7f, 3.1f, 2.9f},
    {0.4f, 0.4f, 0.4f, 0.4f, 0.3f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.4f, 1.2f, 2.6f, 3.4f, 3.8f, 3.5f},
    {0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.2f, 1.3f, 2.8f, 3.6f, 3.9f, 3.6f},
    {0.5f, 0.7f, 1.0f, 1.1f, 0.8f, 0.2f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.3f, 1.4f, 2.6f, 3.3f, 3.5f, 3.2f},
    {1.0f, 1.6f, 2.0f, 2.2f, 1.7f, 0.7f, 0.1f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.1f, 0.5f, 1.2f, 2.0f, 2.5f, 2.6f, 2.4f},
    {1.7f, 2.6f, 3.2f, 3.5f, 3.1f, 1.7f, 0.4f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.6f, 1.6f, 2.1f, 1.7f, 1.5f, 1.6f, 1.4f},
    {2.4f, 3.5f, 4.3f, 4.6f, 4.3f, 3.1f, 1.4f, 0.3f, 0.1f, 0.0f, 0.0f, 0.0f, 0.1f, 0.8f, 2.1f, 3.4f, 3.5f, 2.8f, 1.9f, 1.0f, 0.5f},
    {3.0f, 4.2f, 5.1f, 5.4f, 5.0f, 4.1f, 2.6f, 1.1f, 0.3f, 0.2f, 0.2f, 0.6f, 1.4f, 2.8f, 4.5f, 5.2f, 4.9f, 4.1f, 3.0f, 1.8f, 0.9f},
    {3.2f, 4.4f, 5.3f, 5.6f, 5.2f, 4.4f, 3.1f, 1.8f, 0.7f, 0.4f, 1.1f, 2.1f, 3.6f, 5.2f, 6.1f, 6.4f, 6.0f, 5.2f, 4.0f, 2.6f, 1.4f},
    {3.0f, 4.1f, 4.9f, 5.1f, 4.8f, 4.1f, 2.9f, 1.8f, 0.8f, 0.9f, 1.9f, 3.3f, 4.7f, 6.0f, 6.8f, 7.1f, 6.8f, 5.9f, 4.7f, 3.2f, 1.9f},
    {2.4f, 3.3f, 4.0f, 4.2f, 4.0f, 3.3f, 2.4f, 1.5f, 0.8f, 1.1f, 2.2f, 3.5f, 4.9f, 6.1f, 7.0f, 7.3f, 7.0f, 6.2f, 5.0f, 3.6f, 2.2f},
    {1.7f, 2.3f, 2.8f, 2.9f, 2.8f, 2.3f, 1.7f, 1.1f, 0.7f, 1.2f, 2.1f, 3.4f, 4.6f, 5.8f, 6.5f, 6.8f, 6.6f, 5.9f, 4.8f, 3.5f, 2.2f},
    {1.0f, 1.3f, 1.5f, 1.6f, 1.6f, 1.3f, 1.0f, 0.8f, 0.7f, 1.1f, 1.9f, 2.9f, 4.0f, 4.9f, 5.6f, 5.9f, 5.7f, 5.1f, 4.2f, 3.1f, 2.0f}
};
//==============================================================================
//初期化処理
//==============================================================================
void MeshField::Init()
{
    m_Layer = 1;

    CreateVertex();
    CreateIndex();

    m_Shader = ShaderManager::Load("shader\\fieldVS.cso", "shader\\fieldPS.cso");
    m_Texture = TextureManager::Load(L"asset\\texture\\grass.jpg");

    Audio* bgm = AddComponent<Audio>(this);
    bgm->Load("asset\\audio\\bgm.wav");
    //bgm->Play(true);
}

//==============================================================================
//頂点作成
//==============================================================================
void MeshField::CreateVertex()
{
    for (int x = 0; x < VERTEX_NUM; x++)
    {
        for (int z = 0; z < VERTEX_NUM; z++)
        {
            m_Vertex[x][z].Position = XMFLOAT3((x - FIELD_HALF) * FIELD_STEP, g_FieldHeight[z][x], (z - FIELD_HALF) * -FIELD_STEP);
            m_Vertex[x][z].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
            m_Vertex[x][z].Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            m_Vertex[x][z].TexCoord = XMFLOAT2((float)x, (float)z);
        }
    }

    //法線は隣の頂点との差を外積して求める（端は上向きのまま）
    for (int x = 1; x < VERTEX_NUM - 1; x++)
    {
        for (int z = 1; z < VERTEX_NUM - 1; z++)
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

            m_Vertex[x][z].Normal = XMFLOAT3(vn.x, vn.y, vn.z);
        }
    }

    m_Mesh.CreateVertex(&m_Vertex[0][0], VERTEX_NUM * VERTEX_NUM);
}

//==============================================================================
//インデックス作成
//==============================================================================
void MeshField::CreateIndex()
{
    unsigned int index[INDEX_NUM];

    int i = 0;

    for (int x = 0; x < DIV; x++)
    {
        for (int z = 0; z < VERTEX_NUM; z++)
        {
            index[i++] = x * VERTEX_NUM + z;
            index[i++] = (x + 1) * VERTEX_NUM + z;
        }

        if (x == DIV - 1)
            break;

        // 縮退ポリゴン（次の列へ折り返すために潰れた三角形を挟む）
        index[i++] = (x + 1) * VERTEX_NUM + (VERTEX_NUM - 1);
        index[i++] = (x + 1) * VERTEX_NUM;
    }

    m_Mesh.CreateIndex(index, INDEX_NUM);
}

//==============================================================================
//終了処理
//==============================================================================
void MeshField::Uninit()
{
    m_Mesh.Release();

    GameObject::Uninit();
}

//==============================================================================
//描画処理
//==============================================================================
void MeshField::Draw()
{
    m_Shader->Set();

    Renderer::SetWorldMatrix(GetWorldMatrix());
    Renderer::SetTextureMaterial();
    Renderer::SetTexture(m_Texture);

    m_Mesh.Bind(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    m_Mesh.Draw();

    GameObject::Draw();
}

//==============================================================================
//指定位置の地面の高さを求める
//==============================================================================
float MeshField::GetHeight(Vector3 position)
{
    int x, z;
    
    //ブロック番号算出
    // 頂点間隔はFIELD_STEP、かつZは (z - 10) * -FIELD_STEP と反転して生成している
    x = (int)(position.x / FIELD_STEP + FIELD_HALF);
    z = (int)(FIELD_HALF - position.z / FIELD_STEP);

    // フィールド外は端のマスにクランプ（m_Vertex[x + 1][z + 1] の範囲外アクセス防止）
    if (x < 0)  x = 0;
    if (x > DIV - 1) x = DIV - 1;
    if (z < 0)  z = 0;
    if (z > DIV - 1) z = DIV - 1;

	XMFLOAT3 pos0, pos1, pos2, pos3;

	pos0 = m_Vertex[x][z].Position;                         //左上
	pos1 = m_Vertex[x + 1][z].Position;                 //右上
	pos2 = m_Vertex[x][z + 1].Position;                 //左下
	pos3 = m_Vertex[x + 1][z + 1].Position;         //右下

    Vector3 v12, v1p;
	v12.x = pos2.x - pos1.x;
    v12.y = pos2.y - pos1.y;
	v12.z = pos2.z - pos1.z;
    
    v1p.x = position.x - pos1.x;
    v1p.y = position.y - pos1.y;
    v1p.z = position.z - pos1.z;

    //外積
	float cy = v12.z * v1p.x - v12.x * v1p.z;

    float py;
    Vector3 n;
    if (cy > 0.0f)
    {
        //左上
        Vector3 v10;
		v10.x = pos0.x - pos1.x;
		v10.y = pos0.y - pos1.y;
		v10.z = pos0.z - pos1.z;
		
        //外積
		n = Vector3::cross(v12, v10);
	}
    else
    {
        //右下
        Vector3 v13;
        v13.x = pos3.x - pos1.x;
        v13.y = pos3.y - pos1.y;
        v13.z = pos3.z - pos1.z;

        //外積
        n = Vector3::cross(v13, v12);
    }

    //高さ取得
    py = -((position.x - pos1.x) * n.x + (position.z - pos1.z) * n.z) / n.y + pos1.y;

    return py;
}
