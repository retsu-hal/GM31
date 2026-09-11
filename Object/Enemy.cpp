/*==============================================================================

[Enemy.cpp]
														Author :Watanabe Retsu
														Date   :
--------------------------------------------------------------------------------

==============================================================================*/

//==============================================================================
//インクルード
//==============================================================================
#include "main.h"
#include "renderer.h"
#include "Manager.h"
#include "Enemy.h"
#include "ModelRenderer.h"
#include "Player.h"
#include "Box.h"
#include "Collider.h"
#include "Tree.h"
#include "Explosion.h"
#include "Camera.h"
#include "MeshField.h"

//==============================================================================
//マクロ宣言
//==============================================================================
#define FLASH_DURATION	(0.05f)		// 被弾時に白く光る時間（秒）

//==============================================================================
//プロトタイプ宣言
//==============================================================================

//==============================================================================
//グローバル変数
//==============================================================================

//==============================================================================
//初期化処理
//==============================================================================
void Enemy::Init()
{
	m_Layer = 1;
	m_Life = 6;
	m_Flash = false;

	m_Position = { 0.0f, 5.0f, 0.0f };
	m_Scale = { 1.0f, 1.0f, 1.0f };

	//m_ModelRenderer = new ModelRenderer();
	m_ModelRenderer = AddComponent<ModelRenderer>(this);
	m_ModelRenderer->Load("asset\\model\\player.obj");

	//シェーダー読み込み
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

}

//==============================================================================
//終了処理
//==============================================================================
void Enemy::Uninit()
{
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();

	GameObject::Uninit();
}

//==============================================================================
//更新処理
//==============================================================================
void Enemy::Update()
{
	float dt = Manager::GetDeltaTime();

	//前フレームのシェイク分を打ち消し、素の位置で移動・当たり判定を行う
	m_Position -= m_ShakeOffset;

	//------------------------------------------------------------
	// プレイヤー追従（XZ平面のみ。ジャンプに繋げない）
	//------------------------------------------------------------
	auto players = Manager::GetGameObjects<Player>();
	if (!players.empty())
	{
		Player* player = players[0];

		Vector3 direction = player->GetPosition() - m_Position;
		direction.y = 0.0f;

		float length = direction.length();
		if (length > 0.01f)
		{
			direction = direction / length;
			m_Position += direction * m_Speed * dt;
			m_Rotation.y = atan2f(-direction.x, -direction.z);
		}
	}

	//------------------------------------------------------------
	// 重力
	//------------------------------------------------------------
	m_Velocity.y += -m_Gravity * dt;
	m_Position.y += m_Velocity.y * dt;

	bool oldGround = m_Ground;   // 着地判定用に前フレームの接地状態を保存
	m_Ground = false;

	// 地面（MeshFieldのないシーンでは従来通り y = 0 を床にする）
	MeshField* meshField = Manager::GetGameObject<MeshField>();
	float height = meshField ? meshField->GetHeight(m_Position) : 0.0f;

	if (m_Position.y < height)
	{
		m_Position.y = height;
		m_Velocity.y = 0.0f;
		m_Ground = true;
	}

	

	//------------------------------------------------------------
	// 着地アニメーション（空中→接地に変わった瞬間に潰す）
	//------------------------------------------------------------
	if (!oldGround && m_Ground)
	{
		m_Scale.x = 2.0f;
		m_Scale.y = 0.5f;
		m_Scale.z = 2.0f;
	}

	// 元のサイズへ戻す（毎フレーム）
	m_Scale.x += (1.0f - m_Scale.x) * 0.1f;
	m_Scale.y += (1.0f - m_Scale.y) * 0.1f;
	m_Scale.z += (1.0f - m_Scale.z) * 0.1f;

	//シェイク（位置に加算し続けるとずれたままになるのでオフセットとして持つ）
	m_ShakeTime += dt;
	m_Shake *= 0.9f;
	m_ShakeOffset = m_Shake * cosf(m_ShakeTime * 100.0f);
	m_Position += m_ShakeOffset;

	//ヒットフラッシュ
	if (m_FlashTime > 0.0f)
	{
		m_FlashTime -= dt;
		if (m_FlashTime <= 0.0f)	m_Flash = false;
	}

	m_ModelRenderer->SetFlash(m_Flash);

	GameObject::Update();
}

//==============================================================================
//描画処理
//==============================================================================
void Enemy::Draw()
{
#if _DEBUG
	ImGui::Begin("Enemy");
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", m_Position.x, m_Position.y, m_Position.z);
	ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", m_Rotation.x, m_Rotation.y, m_Rotation.z);
	ImGui::Text("Scale: (%.2f, %.2f, %.2f)", m_Scale.x, m_Scale.y, m_Scale.z);
	ImGui::End();
#endif // _DEBUG



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

	GameObject::Draw();
}

void Enemy::AddDamage(int Damage)
{
	m_Life -= Damage;

	m_Flash = true;
	m_FlashTime = FLASH_DURATION;

	if (m_Life <= 0)
	{
		SetDestroy();
		Explosion* explosion = Manager::AddGameObject<Explosion>();
		explosion->SetPosition(m_Position);
		explosion->SetScale({ 2.0f,2.0f,2.0f });

		//既存のカメラを取得して揺らす（AddGameObjectだとカメラが増えてしまう）
		CAMERA* camera = Manager::GetGameObject<CAMERA>();
		if (camera)	camera->Shake({ 0.0f,1.0f,0.0f });
	}
}
