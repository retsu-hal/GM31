/*==============================================================================

[Score.cpp]
														Author :Watanabe Retsu
														Date   :2026/06/30
--------------------------------------------------------------------------------

==============================================================================*/

//==============================================================================
//インクルード
//==============================================================================
#include "main.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Manager.h"
#include "Score.h"

//==============================================================================
//マクロ宣言
//==============================================================================
#define TEXTURE_NUM_WIDTH 5
#define TEXTURE_NUM_HEIGHT 5
#define SCORE_MAX 999999
#define DIGIT_WIDTH  50.0f   // 1桁の横幅
#define DIGIT_HEIGHT 50.0f   // 1桁の高さ
#define DIGIT_STEP   30.0f   // 次の桁までの送り幅（詰める/空けるを調整）
#define POP_TIME     0.15f   // ポップ演出の長さ（秒）

//==============================================================================
//プロトタイプ宣言
//==============================================================================
//1桁分の四角形（TRIANGLESTRIP用の4頂点）を作る
static void MakeDigitVertex(VERTEX_3D* vertex, float offsetX, float u, float v, float sizeU, float sizeV);

//==============================================================================
//初期化処理
//==============================================================================
void Score::Init()
{
	m_Layer = 3;

	//頂点は毎フレーム書き換えるので、初期データなしの動的バッファを作る
	m_Mesh.CreateVertex(nullptr, 4 * DIGIT_NUM, true);

	m_Shader = ShaderManager::Load("shader\\unlitTextureVS.cso", "shader\\unlitTexturePS.cso");
	m_Texture = TextureManager::Load(L"asset\\texture\\number.png");
}

//==============================================================================
//終了処理
//==============================================================================
void Score::Uninit()
{
	m_Mesh.Release();

	GameObject::Uninit();
}

//==============================================================================
//更新処理
//==============================================================================
void Score::Update()
{
	float dt=Manager::GetDeltaTime();

	//カウントアップ処理
	if (m_Score < m_TargetScore)
	{
		m_CountTimer += dt;

		int diff = m_TargetScore - m_Score;
		int step = (diff / 10 > 1) ? diff / 10 : 1;

		const float COUNT_INTERVAL = 0.02f; // 1カウントあたりの間隔（秒）
		if (m_CountTimer >= COUNT_INTERVAL)
		{
			m_CountTimer = 0.0f;
			m_Score += step;
			if (m_Score > m_TargetScore) m_Score = m_TargetScore;

			m_PopTimer = POP_TIME; // カウントするたびにポップさせる
		}
	}

	if (m_PopTimer > 0.0f)
	{
		m_PopTimer -= dt;
		if (m_PopTimer < 0.0f) m_PopTimer = 0.0f;
	}

	//上限
	if (m_Score > SCORE_MAX) m_Score = SCORE_MAX;

	GameObject::Update();
}

//==============================================================================
//描画処理
//==============================================================================
void Score::Draw()
{
	m_Shader->Set();

	//2D用のビュー・プロジェクション
	Renderer::SetWorldViewProjection2D();

	//ポップ演出のスケール計算（1.0→0.0で減衰し、最大1.2倍まで膨らむ）
	float popScale = 1.0f;
	if (m_PopTimer > 0.0f) popScale = 1.0f + 0.2f * (m_PopTimer / POP_TIME);

	//ポップ拡大は通常のスケールと同じ位置（平行移動より前）に掛ける
	Renderer::SetWorldMatrix(XMMatrixScaling(popScale, popScale, 1.0f) * GetWorldMatrix());
	Renderer::SetTextureMaterial();
	Renderer::SetTexture(m_Texture);

	//スコアを桁に分解（下位桁から取り出して上位桁へ詰める）
	int digit[DIGIT_NUM];
	int score = m_Score;
	for (int i = DIGIT_NUM - 1; i >= 0; i--)
	{
		digit[i] = score % 10;
		score /= 10;
	}

	//桁ごとに数字テクスチャの切り出し位置を書き込む
	float sizeU = 1.0f / TEXTURE_NUM_WIDTH;
	float sizeV = 1.0f / TEXTURE_NUM_HEIGHT;

	VERTEX_3D* vertex = m_Mesh.Map();
	for (int i = 0; i < DIGIT_NUM; i++)
	{
		float u = sizeU * (digit[i] % TEXTURE_NUM_WIDTH);
		float v = sizeV * (digit[i] / TEXTURE_NUM_WIDTH);

		MakeDigitVertex(&vertex[i * 4], DIGIT_STEP * i, u, v, sizeU, sizeV);
	}
	m_Mesh.Unmap();

	//桁ごとに1枚ずつ描く（四角形はTRIANGLESTRIPなので繋げて描けない）
	m_Mesh.Bind(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	for (int i = 0; i < DIGIT_NUM; i++) m_Mesh.Draw(4, i * 4);

	GameObject::Draw();
}

//==============================================================================
//1桁分の頂点作成
//==============================================================================
static void MakeDigitVertex(VERTEX_3D* vertex, float offsetX, float u, float v, float sizeU, float sizeV)
{
	static const XMFLOAT3 normal = { 0.0f, 0.0f, 0.0f };
	static const XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

	vertex[0] = { { offsetX,               0.0f,         0.0f }, normal, color, { u,         v } };
	vertex[1] = { { offsetX + DIGIT_WIDTH, 0.0f,         0.0f }, normal, color, { u + sizeU, v } };
	vertex[2] = { { offsetX,               DIGIT_HEIGHT, 0.0f }, normal, color, { u,         v + sizeV } };
	vertex[3] = { { offsetX + DIGIT_WIDTH, DIGIT_HEIGHT, 0.0f }, normal, color, { u + sizeU, v + sizeV } };
}
