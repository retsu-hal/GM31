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

	auto players = Manager::GetGameObjects<Player>();
	if (!players.empty())
	{
		Player* player = players[0];

		// プレイヤーへの方向ベクトルを計算
		Vector3 direction = player->GetPosition() - m_Position;
		float length = direction.length();

		// 近すぎる場合は正規化時のゼロ除算を防ぐ
		if (length > 0.01f)
		{
			direction = direction / length; // 正規化

			float dt = Manager::GetDeltaTime();
			m_Position += direction * m_Speed * dt;

			// プレイヤーの方を向かせたい場合（Y軸回転のみ）
			m_Rotation.y = atan2f(-direction.x, -direction.z);
		}
	}

	//boxとの当たり判定
	auto boxes = Manager::GetGameObjects<Box>();
	for (auto box : boxes)
	{
		Vector3 pushVector;

		Vector3 playerCenter = { m_Position.x, m_Position.y + m_Scale.y, m_Position.z };
		Vector3 playerSize = { m_Scale.x, m_Scale.y * 2.0f, m_Scale.z };

		Vector3 boxPos = box->GetPosition();
		Vector3 boxScl = box->GetScale();
		Vector3 boxCenter = { boxPos.x, boxPos.y + boxScl.y, boxPos.z };
		Vector3 boxSize = { boxScl.x * 2.0f, boxScl.y * 2.0f, boxScl.z * 2.0f };

		if (Collision::AABB(playerCenter, playerSize, boxCenter, boxSize, pushVector, m_Ground))
		{
			m_Position += pushVector;
			if (pushVector.x != 0.0f) m_Velocity.x = 0.0f;
			if (pushVector.y != 0.0f) m_Velocity.y = 0.0f;
			if (pushVector.z != 0.0f) m_Velocity.z = 0.0f;
			if (m_Ground) m_Ground = true;
		}
	}

	//敵との当たり判定
	auto enemies = Manager::GetGameObjects<Enemy>();
	for (auto other : enemies)
	{
		if (other == this) continue; // 自分自身は無視
		Vector3 pushVector;
		if (Collision::Circle2D(m_Position, m_Scale.x * 0.5f, other->GetPosition(), other->GetScale().x * 0.5f, pushVector))
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