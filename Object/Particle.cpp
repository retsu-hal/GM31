/*==============================================================================

	[Particle.cpp]
														Author :Watanabe Retsu
														Date   :2026/06/18
--------------------------------------------------------------------------------

==============================================================================*/

//==============================================================================
//インクルード
//==============================================================================
#include "main.h"
#include "Manager.h"
#include "Renderer.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "Particle.h"
#include "Camera.h"

//==============================================================================
//マクロ宣言
//==============================================================================
#define EMIT_PER_FRAME	(10)		// 1フレームに発射する数
#define PARTICLE_LIFE	(60)		// パーティクルの寿命（フレーム）
#define EMIT_SPEED		(30.0f)		// 発射速度のばらつき幅

//==============================================================================
//初期化処理
//==============================================================================
void Particle::Init()
{
	m_Layer = 2;

	static const XMFLOAT3 normal = { 0.0f, 0.0f, -1.0f };
	static const XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

	static const VERTEX_3D vertex[] =
	{
		{ { -0.5f,  0.5f, 0.0f }, normal, color, { 0.0f, 0.0f } },
		{ {  0.5f,  0.5f, 0.0f }, normal, color, { 1.0f, 0.0f } },
		{ { -0.5f, -0.5f, 0.0f }, normal, color, { 0.0f, 1.0f } },
		{ {  0.5f, -0.5f, 0.0f }, normal, color, { 1.0f, 1.0f } },
	};

	m_Mesh.CreateVertex(vertex, _countof(vertex));

	m_Shader = ShaderManager::Load("shader\\unlitTextureVS.cso", "shader\\unlitTexturePS.cso");
	m_Texture = TextureManager::Load(L"asset\\texture\\particle.png");

	//パーティクルの初期化
	for(int i = 0; i < PARTICLE_MAX; i++)
	{
		m_Particle[i].Enable = false;
	}
}

//==============================================================================
//終了処理
//==============================================================================
void Particle::Uninit()
{
	m_Mesh.Release();

	GameObject::Uninit();
}

//==============================================================================
//発射処理
//==============================================================================
void Particle::Emit(int count)
{
	for (int i = 0; i < PARTICLE_MAX && count > 0; i++)
	{
		if (m_Particle[i].Enable) continue;

		m_Particle[i].Enable = true;
		m_Particle[i].Life = PARTICLE_LIFE;
		m_Particle[i].Position = m_Position;
		m_Particle[i].Velocity.x = ((float)rand() / RAND_MAX - 0.5f) * EMIT_SPEED;
		m_Particle[i].Velocity.y = ((float)rand() / RAND_MAX - 0.5f) * EMIT_SPEED;
		m_Particle[i].Velocity.z = ((float)rand() / RAND_MAX - 0.5f) * EMIT_SPEED;

		count--;
	}
}

//==============================================================================
//更新処理
//==============================================================================
void Particle::Update()
{
	float dt = Manager::GetDeltaTime();

	Emit(EMIT_PER_FRAME);

	Vector3 gravity{ 0.0f, -9.8f, 0.0f }; // 重力加速度

	//パーティクルの更新
	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		if (!m_Particle[i].Enable) continue;

		m_Particle[i].Velocity += gravity * dt;
		m_Particle[i].Velocity += m_Particle[i].Velocity * -1.0f * dt;	//空気抵抗
		m_Particle[i].Position += m_Particle[i].Velocity * dt;

		m_Particle[i].Life--;
		if (m_Particle[i].Life <= 0)	m_Particle[i].Enable = false;
	}

	GameObject::Update();
}

//==============================================================================
//描画処理
//==============================================================================
void Particle::Draw()
{
	CAMERA* camera = Manager::GetGameObject<CAMERA>();
	if (camera == nullptr) return;

	//ビルボード用マトリクス（ビュー行列の逆行列で回転だけ打ち消す）
	XMMATRIX invViewMatrix = XMMatrixInverse(NULL, camera->GetViewMatrix());
	invViewMatrix.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);	//平行移動成分を消す

	m_Shader->Set();

	Renderer::SetTextureMaterial(XMFLOAT4(0.2f, 0.2f, 1.0f, 1.0f));
	Renderer::SetTexture(m_Texture);

	Renderer::SetDepthEnable(false);	//深度バッファを無効
	Renderer::SetAddEnable(true);		//加算合成を有効

	//同じ四角形を位置違いで描くので、バッファの設定は1回でよい
	m_Mesh.Bind(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	XMMATRIX scaleMatrix = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);

	for (int i = 0; i < PARTICLE_MAX; i++)
	{
		if (!m_Particle[i].Enable) continue;

		const Vector3& pos = m_Particle[i].Position;
		Renderer::SetWorldMatrix(scaleMatrix * invViewMatrix * XMMatrixTranslation(pos.x, pos.y, pos.z));

		m_Mesh.Draw();
	}

	Renderer::SetDepthEnable(true);	//深度バッファを有効
	Renderer::SetAddEnable(false);	//加算合成を無効

	GameObject::Draw();
}
