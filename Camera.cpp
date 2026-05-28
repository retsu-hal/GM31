#include "manager.h"
#include "Camera.h"
#include "Player.h"

#define POS_MAX 100.0f
#define POS_MIN -100.0f
#define FOV_MAX 46.0f
#define FOV_MIN 44.0f

void CAMERA::Init()
{
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
	Vector3 playerForward = player->GetFront();

	m_Target = playerPos;

	m_Position = m_Target - playerForward * 5.0f + Vector3{0.0f,5.0f,0.0f};
}

void CAMERA::Draw()
{
	XMMATRIX projection = XMMatrixPerspectiveFovLH(m_Fov, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 1000.0f);
	Renderer::SetProjectionMatrix(projection);
    
    XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3((XMFLOAT3*)&m_Position), XMLoadFloat3((XMFLOAT3*)&m_Target), XMLoadFloat3(&up));
	Renderer::SetViewMatrix(view);
}


void CAMERA::SetPosition(Vector3 position)
{
    m_Position = position;
}

Vector3 CAMERA::GetPosition()
{
    return m_Position;
}

void CAMERA::SetTarget(Vector3 target)
{
	m_Target = target;
}

Vector3 CAMERA::GetTarget()
{
    return m_Angle;
}

void CAMERA::SetAngle(Vector3 angle)
{
	m_Angle = angle;
}

Vector3 CAMERA::GetAngle()
{
	return m_Angle;
}

void CAMERA::SetFov(float fov)
{
	m_Fov = fov;
}

float CAMERA::GetFov()
{
    return m_Fov;
}