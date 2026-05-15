#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "Camera.h"
#include "keyboard.h"

#define POS_MAX 100.0f
#define POS_MIN -100.0f
#define FOV_MAX 46.0f
#define FOV_MIN 44.0f

void CAMERA::Init()
{
	m_Position = { 0.0f, 10.0f, -15.0f };
	m_Target = { 0.0f, 0.0f, 0.0f };
	m_Angle = { 0.0f, 0.0f, 0.0f };
	m_Fov = 45.0f;
}

void CAMERA::Uninit()
{

}

void CAMERA::Update()
{
    float moveSpeed = 0.5f;
    
    if (Keyboard_IsKeyDown(KK_R))
    {
        CAMERA::Init();
        return;
    }

    float dirX = sinf(m_Angle.y) * cosf(m_Angle.x);
    float dirY = sinf(m_Angle.x);
    float dirZ = cosf(m_Angle.y) * cosf(m_Angle.x);

	//前後の移動
    if (Keyboard_IsKeyDown(KK_W)) {
        m_Position.x += dirX * moveSpeed;
        m_Position.z += dirZ * moveSpeed;
        if (m_Position.z > POS_MAX) m_Position.z = POS_MAX;
    }
    else if (Keyboard_IsKeyDown(KK_S)) {
        m_Position.x -= dirX * moveSpeed;
        m_Position.z -= dirZ * moveSpeed;
        if (m_Position.z < POS_MIN) m_Position.z = POS_MIN;
    }
	//左右の移動
    if (Keyboard_IsKeyDown(KK_A)) {
        m_Position.x -= dirZ * moveSpeed;
        m_Position.z += dirX * moveSpeed;
        if (m_Position.x < POS_MIN) m_Position.x = POS_MIN;
    }
    else if (Keyboard_IsKeyDown(KK_D)) {
        m_Position.x += dirZ * moveSpeed;
        m_Position.z -= dirX * moveSpeed;
        if (m_Position.x > POS_MAX) m_Position.x = POS_MAX;
    }

	//上下の移動
    if (Keyboard_IsKeyDown(KK_LEFTSHIFT)) {
        m_Position.y += moveSpeed;
        if (m_Position.y > POS_MAX) m_Position.y = POS_MAX;
    }
    else if (Keyboard_IsKeyDown(KK_SPACE)) {
        m_Position.y -= moveSpeed;
        if (m_Position.y < POS_MIN) m_Position.y = POS_MIN;
    }

	//カメラの角度
    if (Keyboard_IsKeyDown(KK_UP)) {
        m_Angle.x += 0.03f;
        if (m_Angle.x > XM_PIDIV2) m_Angle.x = XM_PIDIV2;
    }
    else if (Keyboard_IsKeyDown(KK_DOWN)) {
        m_Angle.x -= 0.03f;
        if (m_Angle.x < -XM_PIDIV2) m_Angle.x = -XM_PIDIV2;
	}
    if (Keyboard_IsKeyDown(KK_LEFT)) {
        m_Angle.y -= 0.03f;
    }
    else if (Keyboard_IsKeyDown(KK_RIGHT)) {
        m_Angle.y += 0.03f;
	}

    m_Target.x = m_Position.x + dirX;
    m_Target.y = m_Position.y + dirY;
    m_Target.z = m_Position.z + dirZ;
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