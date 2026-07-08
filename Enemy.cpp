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
#include "Enemy.h"
#include "ModelRenderer.h"
#include "manager.h"
#include "Player.h"
#include "Box.h"
#include "Collision.h"
#include "Tree.h"

//==============================================================================
//マクロ宣言
//==============================================================================

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
	m_Position = { 0.0f, 5.0f, 0.0f };
	m_Scale = { 1.0f, 1.0f, 1.0f };

	//m_ModelRenderer = new ModelRenderer();
	ModelRenderer* modelRenderer = AddComponent<ModelRenderer>(this);
	modelRenderer->Load("asset\\model\\player.obj");

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

	// 地面
	if (m_Position.y < 0.0f)
	{
		m_Position.y = 0.0f;
		m_Velocity.y = 0.0f;
		m_Ground = true;
	}

	//------------------------------------------------------------
	// boxとの当たり判定（側面に当たったらジャンプして上に乗る）
	//------------------------------------------------------------
	auto boxes = Manager::GetGameObjects<Box>();
	for (auto box : boxes)
	{
		Vector3 pushVector;
		bool onBoxTop = false;

		Vector3 enemyCenter = { m_Position.x, m_Position.y + m_Scale.y, m_Position.z };
		Vector3 enemySize = { m_Scale.x, m_Scale.y * 2.0f, m_Scale.z };

		Vector3 boxPos = box->GetPosition();
		Vector3 boxScl = box->GetScale();
		Vector3 boxCenter = { boxPos.x, boxPos.y + boxScl.y, boxPos.z };
		Vector3 boxSize = { boxScl.x * 2.0f, boxScl.y * 2.0f, boxScl.z * 2.0f };

		if (Collision::AABB(enemyCenter, enemySize, boxCenter, boxSize, pushVector, onBoxTop))
		{
			// めり込みを解消する（ボックスの上に乗れるようにする）
			m_Position += pushVector;

			if (onBoxTop)
			{
				// 上面に着地して乗る（ボックスの上に立つ）
				m_Velocity.y = 0.0f;
				m_Ground = true;
			}
			else if (pushVector.y != 0.0f)
			{
				// 下面に当たった（天井）→縦速度を止める
				m_Velocity.y = 0.0f;
			}
			else if (m_Ground)
			{
				// 接地中に側面へ当たった→ボックスに乗ろうとジャンプ
				m_Velocity.y = 16.0f;
				m_Ground = false;
			}
		}
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

	//------------------------------------------------------------
	// エネミー同士の当たり判定
	//------------------------------------------------------------
	auto enemies = Manager::GetGameObjects<Enemy>();
	for (auto other : enemies)
	{
		if (other == this) continue;
		Vector3 pushVector;
		if (Collision::Circle2D(m_Position, m_Scale.x * 0.5f, other->GetPosition(), other->GetScale().x * 0.5f, pushVector))
		{
			m_Position += pushVector;
		}
	}

	//------------------------------------------------------------
	// ツリーとの当たり判定
	//------------------------------------------------------------
	//木の当たり判定
	auto trees = Manager::GetGameObjects<Tree>();
	for (auto tree : trees)
	{
		Vector3 pushVector;
		if (Collision::Circle2D(m_Position, m_Scale.x * 0.5f, tree->GetPosition(), 1.0f, pushVector))
		{
			m_Position += pushVector;
			float dot = m_Velocity.x * (pushVector.x) + m_Velocity.z * (pushVector.z);
		}
	}

	GameObject::Update();
}

//==============================================================================
//描画処理
//==============================================================================
void Enemy::Draw()
{

	ImGui::Begin("Enemy");
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", m_Position.x, m_Position.y, m_Position.z);
	ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", m_Rotation.x, m_Rotation.y, m_Rotation.z);
	ImGui::Text("Scale: (%.2f, %.2f, %.2f)", m_Scale.x, m_Scale.y, m_Scale.z);
	ImGui::End();

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
