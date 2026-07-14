#include "main.h"
#include "renderer.h"
#include "Box.h"
#include "ModelRenderer.h"



void Box::Init()
{
	m_Layer = 1;
	m_Position = { 0.0f, 5.0f, 0.0f };
	m_Scale = { 1.0f, 1.0f, 1.0f };

	//m_ModelRenderer = new ModelRenderer();
	ModelRenderer* modelRenderer = AddComponent<ModelRenderer>(this);
	modelRenderer->Load("asset\\model\\box.obj");

	//シェーダー読み込み
	Renderer::CreateVertexShader(&m_VertexShader, &m_VertexLayout, "shader\\unlitTextureVS.cso");
	Renderer::CreatePixelShader(&m_PixelShader, "shader\\unlitTexturePS.cso");

}

void Box::Uninit()
{
	m_VertexLayout->Release();
	m_VertexShader->Release();
	m_PixelShader->Release();

	GameObject::Uninit();
}

void Box::Update()
{

	GameObject::Update();
}

void Box::Draw()
{
	Vector3 bMin = { m_Position.x - m_Scale.x * 0.5f, m_Position.y - m_Scale.y * 0.5f, m_Position.z - m_Scale.z * 0.5f };
	Vector3 bMax = { m_Position.x + m_Scale.x * 0.5f, m_Position.y + m_Scale.y * 0.5f, m_Position.z + m_Scale.z * 0.5f };

	ImGui::Begin("BoxDebug");
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", m_Position.x, m_Position.y, m_Position.z);
	ImGui::Text("AABB Min: (%.2f, %.2f, %.2f)", bMin.x, bMin.y, bMin.z);
	ImGui::Text("AABB Max: (%.2f, %.2f, %.2f)", bMax.x, bMax.y, bMax.z);
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


