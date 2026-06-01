#include "main.h"
#include "renderer.h"
#include "Player.h"
#include "ModelRenderer.h"



void Player::Init()
{
	m_Position = { 0.0f, 2.0f, 0.0f };
	m_Scale = { 1.0f, 1.0f, 1.0f };
	m_Velocity = { 0.0f, 0.0f, 0.0f };
	
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
	float dt = 1.0f / 60.0f; 
	



	Vector3 forward = GetFront();
	Vector3 right = GetRight();
	//入力による加速
	if(Input::GetKeyPress('W'))
		m_Velocity += forward * m_Speed * dt; 
	if (Input::GetKeyPress('S'))
		m_Velocity -= forward * m_Speed * dt; 
	if(Input::GetKeyPress('D'))
		m_Velocity += right * m_Speed * dt; 
	if(Input::GetKeyPress('A'))
	    m_Velocity -= right * m_Speed * dt; 


	//入力による回転
	if(Input::GetKeyPress('Q'))
		m_Rotation.y += 5.0f * dt;
	if (Input::GetKeyPress('E'))
		m_Rotation.y -= 5.0f * dt;

	//ジャンプ
	if (Input::GetKeyTrigger(VK_SPACE))
	{
		m_Velocity.y += m_jumpPower;
	}

	//重力
	m_Velocity.y += -m_Gravity * dt;

	m_Position += m_Velocity * dt;

	//抵抗力
	m_Velocity.x += -m_Velocity.x * m_Friction * dt;
	m_Velocity.y += -m_Velocity.y * m_Friction * dt;
	m_Velocity.z += -m_Velocity.z * m_Friction	 * dt;

	if (m_Position.y < 2.0f)
	{
			m_Position.y = 2.0f;
			m_Velocity.y = 0.0f;		
	}
	
	GameObject::Update();
}

void Player::Draw()
{
	//入力レイアウト設定
	Renderer::GetDeviceContext()->IASetInputLayout(m_VertexLayout);

	//シェーダー設定
	Renderer::GetDeviceContext()->VSSetShader(m_VertexShader, NULL, 0);
	Renderer::GetDeviceContext()->PSSetShader(m_PixelShader, NULL, 0);

	//マトリックス設定
	XMMATRIX WorldMatrix, ScaleMatrix, RotMatrix, TransMatrix;
	ScaleMatrix = XMMatrixScaling(m_Scale.x, m_Scale.y, m_Scale.z);
	RotMatrix = XMMatrixRotationRollPitchYaw(m_Rotation.x, m_Rotation.y+XM_PI, m_Rotation.z);
	TransMatrix = XMMatrixTranslation(m_Position.x, m_Position.y, m_Position.z);
	WorldMatrix = ScaleMatrix * RotMatrix * TransMatrix;

	Renderer::SetWorldMatrix(WorldMatrix);

	GameObject::Draw();
}

void Player::DrawImGui()
{
	GameObject::DrawImGui();

	ImGui::SliderFloat("Speed", &m_Speed, 0.0f, 100.0f);
	ImGui::SliderFloat("Jump Power", &m_jumpPower, 0.0f, 100.0f);
	ImGui::SliderFloat("Gravity", &m_Gravity, 0.0f, 100.0f);
}
