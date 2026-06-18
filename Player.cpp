#include "main.h"
#include "renderer.h"
#include "Player.h"
#include "Camera.h"
#include "manager.h"
#include "ModelRenderer.h"
#include "Bullet.h"
#include "Tree.h"
#include "Box.h"
#include "Collision.h"



void Player::Init()
{
	m_Layer = 1;
	m_Position = { 0.0f, 1.0f, 0.0f };
	m_Scale = { 1.0f, 1.0f, 1.0f };
	m_Velocity = { 0.0f, 0.0f, 0.0f };
	m_Speed = 50.0f;
	m_jumpPower = 20.0f;
	m_Gravity = 9.8f;

	//m_ModelRenderer = new ModelRenderer();
	ModelRenderer* modelRenderer = AddComponent<ModelRenderer>(this);
	modelRenderer->Load("asset\\model\\player.obj");

	//シェーダー読み込み
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");
}

void Player::Uninit()
{
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();

	GameObject::Uninit();
}

void Player::Update()
{
	Vector3 oldPosition = m_Position;

	float dt = Manager::GetDeltaTime();

	CAMERA* camera = Manager::GetGameObject<CAMERA>();
	Vector3 forward = camera->GetForward();
	Vector3 right = camera->GetRight();

	forward.y = 0.0f;
	forward.normalize();

	right.y = 0.0f;
	right.normalize();

	//入力による加速
	if (Input::GetKeyPress('W'))
		m_Velocity += forward * m_Speed * dt;
	if (Input::GetKeyPress('S'))
		m_Velocity -= forward * m_Speed * dt;
	if (Input::GetKeyPress('D'))
		m_Velocity += right * m_Speed * dt;
	if (Input::GetKeyPress('A'))
		m_Velocity -= right * m_Speed * dt;

	m_Rotation.y = atan2f(m_Velocity.x, m_Velocity.z);

	//ジャンプ
	if (Input::GetKeyTrigger(VK_SPACE))
	{
		m_Velocity.y += m_jumpPower;
	}

	//ジャンプ後のアニメーション
	m_Scale.x += (1.0f - m_Scale.x) * 0.1f;
	m_Scale.y += (1.0f - m_Scale.y) * 0.1f;
	m_Scale.z += (1.0f - m_Scale.z) * 0.1f;


	//重力
	m_Velocity.y += -m_Gravity * dt;

	m_Position += m_Velocity * dt;

	//抵抗力
	m_Velocity.x += -m_Velocity.x * m_Friction * dt;
	m_Velocity.z += -m_Velocity.z * m_Friction	 * dt;

	bool oldGraund = m_Ground;
	m_Ground = false;

	//地面に衝突
	if (m_Position.y < 0.0f)
	{
			m_Position.y = .0f;
			m_Velocity.y = 0.0f;
			m_Ground = true;
	}

	Vector3 center = { m_Position.x, m_Position.y + m_Scale.y, m_Position.z };

	//木の衝突
	auto trees = Manager::GetGameObjects<Tree>();
	for (auto tree : trees)
	{
		Vector3 push;
		if (Collision::RectAndCircle(center, m_Scale, tree->GetPosition(), 1.0f, push))
			m_Position += push;   // push.y は 0 なので水平にだけ押し戻す
	}

	//boxとの衝突
	auto boxes = Manager::GetGameObjects<Box>();
	for (auto box : boxes)
	{
		Vector3 push;
		if (Collision::RectAndRect(center, m_Scale, box->GetPosition(), box->GetScale(), push))
		{
			m_Position += push;

			if (push.x != 0.0f) m_Velocity.x = 0.0f;
			if (push.y != 0.0f) m_Velocity.y = 0.0f;
			if (push.z != 0.0f) m_Velocity.z = 0.0f;

			if (push.y > 0.0f) m_Ground = true;   // 上に押し戻された → 箱の上に乗っている
		}
	}

	if (!oldGraund && m_Ground)
	{
		//着地のアニメーション
		m_Scale.x = 2.0f;
		m_Scale.y = 0.5f;
		m_Scale.z = 2.0f;
	}

	//弾発射
	if (Input::GetKeyTrigger('F'))
	{
		Bullet* bullet = Manager::AddGameObject<Bullet>();
		bullet->SetPosition(m_Position);
		bullet->SetVelocity(GetForward()*10.0f);
	}

	if (m_Ground)
	{
		m_MoveAnimation += m_Velocity.lenght() * dt;
		m_Scale.y += sinf(m_MoveAnimation*3.0f)*0.03f;
	}

	GameObject::Update();
}

void Player::Draw()
{
	ImGui::Begin("PlayerDebug");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", m_Position.x, m_Position.y, m_Position.z);
	ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", m_Rotation.x, m_Rotation.y, m_Rotation.z);
	ImGui::SliderFloat("Speed", &m_Speed, 0.0f, 100.0f);
	ImGui::SliderFloat("Jump Power", &m_jumpPower, 0.0f, 100.0f);
	ImGui::SliderFloat("Gravity", &m_Gravity, 0.0f, 100.0f);
	ImGui::End();

	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダー設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//マトリックス設定
	XMMATRIX WorldMatrix, ScaleMatrix, RotMatrix, TransMatrix;
	ScaleMatrix = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	RotMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y + XM_PI, m_Rotation.z);
	TransMatrix = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	WorldMatrix = ScaleMatrix * RotMatrix * TransMatrix;

	Renderer::SetWorldMatrix(WorldMatrix);

	GameObject::Draw();
}