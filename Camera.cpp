#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "Camera.h"
#include "keyboard.h"

#define ANGLE_UP_MAX 1.5f
#define ANGLE_UP_MIN -1.5f
#define ANGLE_HEIGHT_MAX 80.0f
#define ANGLE_HEIGHT_MIN -100.0f
#define FOV_MAX 2.0f
#define FOV_MIN 0.5f

void CAMERA::Init()
{
	m_Position = { 0.0f, 10.0f, -15.0f };
	m_Target = { 0.0f, 0.0f, 0.0f };
	m_Angle = { 0.0f, 0.0f, 0.0f };
	m_Fov = 1.0f;
}

void CAMERA::Uninit()
{

}

void CAMERA::Update()
{
    float rotSpeed = 0.03f;
    float moveSpeed = 0.5f;
	float fovSpeed = 0.01f;

    //ズーム
    if(Keyboard_IsKeyDown(KK_Q))
    {
        m_Fov += fovSpeed;
        if (m_Fov > FOV_MAX)
        {
            m_Fov = FOV_MAX; // 上限
        }
    }
    else if (Keyboard_IsKeyDown(KK_E))
    {
        m_Fov  -= fovSpeed;
        if (m_Fov < FOV_MIN)
        {
            m_Fov = FOV_MIN; // 下限
        }
    }

    // 左右回転
    if (Keyboard_IsKeyDown(KK_LEFT))
    {
        m_Angle.y -= rotSpeed;
    }
    else if (Keyboard_IsKeyDown(KK_RIGHT))
    {
        m_Angle.y += rotSpeed;
    }

	// 上下回転
    if (Keyboard_IsKeyDown(KK_UP))
    {
		m_Angle.x += rotSpeed;
        if (m_Angle.x > ANGLE_UP_MAX)
        {
			m_Angle.x = ANGLE_UP_MAX; // 上限
        }
    }
    else if (Keyboard_IsKeyDown(KK_DOWN))
	{
		m_Angle.x -= rotSpeed;
        if (m_Angle.x < ANGLE_UP_MIN)
		{
            m_Angle.x = ANGLE_UP_MIN; // 下限
        }
	}

    // カメラの向きベクトルを計算
    float dirX = sinf(m_Angle.y)*cosf(m_Angle.x);
    float dirY = sinf(m_Angle.x);
    float dirZ = cosf(m_Angle.y )*cosf(m_Angle.x);

    // 前後移動（W/S）：向いている方向に移動
    if (Keyboard_IsKeyDown(KK_W)) {
        m_Position.x += dirX * moveSpeed;
        m_Position.z += dirZ * moveSpeed;
    }
    else if (Keyboard_IsKeyDown(KK_S)) {
        m_Position.x -= dirX * moveSpeed;
        m_Position.z -= dirZ * moveSpeed;
    }

    // 左右移動（A/D）：向きに対して横方向に移動
    if (Keyboard_IsKeyDown(KK_A)) {
        m_Position.x -= dirZ * moveSpeed;
        m_Position.z += dirX * moveSpeed;
    }
    else if (Keyboard_IsKeyDown(KK_D)) {
        m_Position.x += dirZ * moveSpeed;
        m_Position.z -= dirX * moveSpeed;
    }

	// 上下移動（スペース/Shift）：Y軸方向に移動
    if (Keyboard_IsKeyDown(KK_LEFTSHIFT)) {
        m_Position.y += moveSpeed;
        if (m_Position.y > ANGLE_HEIGHT_MAX)
        {
            m_Position.y = ANGLE_HEIGHT_MAX; // 上限
        }
    }
    else if (Keyboard_IsKeyDown(KK_SPACE)) {
        m_Position.y -= moveSpeed;
        if (m_Position.y < ANGLE_HEIGHT_MIN)
        {
            m_Position.y = ANGLE_HEIGHT_MIN; // 下限
        }
	}

    // 注視点をカメラの向きに合わせて更新
    m_Target.x = m_Position.x + dirX;
    m_Target.y = m_Position.y + dirY;
    m_Target.z = m_Position.z + dirZ;
}

void CAMERA::Draw()
{
	//プロジェクションマトリクス
	XMMATRIX projection = XMMatrixPerspectiveFovLH(m_Fov, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 1.0f, 1000.0f);
	Renderer::SetProjectionMatrix(projection);

	//ビュー行列
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMMATRIX view = XMMatrixLookAtLH(XMLoadFloat3((XMFLOAT3*)&m_Position), XMLoadFloat3((XMFLOAT3*)&m_Target), XMLoadFloat3(&up));
	Renderer::SetViewMatrix(view);
}