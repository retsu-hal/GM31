#include "main.h"
#include "renderer.h"
#include "Bullet.h"
#include "Enemy.h"
#include "manager.h"
#include "ModelRenderer.h"
#include "Explosion.h"



void Bullet::Init()
{
	m_Layer = 1;
	//m_ModelRenderer = new ModelRenderer();
	ModelRenderer* modelRenderer = AddComponent<ModelRenderer>(this);
	modelRenderer->Load("asset\\model\\bullet.obj");

	//シェーダー読み込み
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

}

void Bullet::Uninit()
{
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();

	GameObject::Uninit();
}

void Bullet::Update()
{
	float dt = Manager::GetDeltaTime();

	m_Position += m_Velocity * dt;

	//敵との当たり判定
	auto enemies = Manager::GetGameObjects<Enemy>();
	for (auto enemy : enemies)
	{
		Vector3 direction = enemy->GetPosition() - m_Position;
		float lenght = direction.lenght();

		if (lenght < 1.5f)
		{
			enemy->SetDestroy();
			SetDestroy();
			Vector3 pos = enemy->GetPosition();
			pos.y += 1.0f;
			Manager::AddGameObject<Explosion>()->SetPosition(pos);

			break;
		}
	}

	m_Lifetime -= dt;
	if (m_Lifetime <= 0.0f)
	{
		SetDestroy();
	}

	GameObject::Update();
}

void Bullet::Draw()
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

	GameObject::Draw();
}