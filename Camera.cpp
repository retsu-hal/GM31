#include "manager.h"
#include "Camera.h"
#include "Player.h"

#define POS_MAX 100.0f
#define POS_MIN -100.0f
#define FOV_MAX 46.0f
#define FOV_MIN 44.0f

void CAMERA::Init()
{
	m_Layer = 0;
	m_Position = { 0.0f, 10.0f, -10.0f };
	m_Target = { 0.0f, 0.0f, 0.0f };
	m_Angle = { 0.0f, 0.0f, 0.0f };
	m_Fov = 45.0f;
	m_MoveSpeed = 0.5f;
}

void CAMERA::Uninit()
{

}

void CAMERA::Update()
{
	Player* player = Manager::GetGameObject<Player>();
	Vector3 playerPos = player->GetPosition();
	Vector3 playerForward = player->GetForward();

	float dt = Manager::GetDeltaTime();
	if(Input::GetKeyPress(VK_RIGHT))
		m_Rotation.y -= 5.0f * dt;
	if (Input::GetKeyPress(VK_LEFT))
		m_Rotation.y += 5.0f * dt;

	m_Target = playerPos;

	m_Position = m_Target + Vector3(-sinf(m_Rotation.y) * 10.0f, 5.0f, -cos(m_Rotation.y) * 10.0f);
}

void CAMERA::Draw()
{
	XMMATRIX projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(m_Fov), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 1000.0f);
	Renderer::SetProjectionMatrix(projection);
    
    XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_ViewMatrix= XMMatrixLookAtLH(XMLoadFloat3((XMFLOAT3*)&m_Position), XMLoadFloat3((XMFLOAT3*)&m_Target), XMLoadFloat3(&up));
	
	
	Renderer::SetViewMatrix(m_ViewMatrix);
}
