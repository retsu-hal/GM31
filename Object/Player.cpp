#include "main.h"
#include "renderer.h"
#include "Player.h"
#include "Camera.h"
#include "manager.h"
#include "animationModel.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Collider.h"
#include "Audio.h"
#include "Shadow.h"
#include "MeshField.h"

#define SHADOW_OFFSET_Y	(0.01f)		// 影を地面から浮かせる量（Zファイティング回避）


void Player::Init()
{
	m_Layer = 1;
	m_Position = { 0.0f, 1.0f, 0.0f };
	m_Scale = { 0.01f, 0.01f, 0.01f };
	m_Velocity = { 0.0f, 0.0f, 0.0f };
	m_Speed = 50.0f;
	m_jumpPower = 16.0f;
	m_Gravity = 40.0f;

	m_AnimationModel = AddComponent<AnimationModel>(this);
	m_AnimationModel->Load("asset\\model\\Akai.fbx");
	m_AnimationModel->LoadAnimation("asset\\model\\Akai_Idle.fbx", "Idle");
	m_AnimationModel->LoadAnimation("asset\\model\\Akai_Run.fbx", "Run");

	m_AnimationName = "Idle";
	m_NextAnimationName = "Idle";
	
	CapsuleCollider*collider = AddComponent<CapsuleCollider>(this);
	collider->SetRadius(40.0f);
	collider->SetHeight(180.0f);
	collider->SetOffset({ 0.0f, 90.0f, 0.0f });

	//SE
	m_JumpSE = AddComponent<Audio>(this);
	m_JumpSE->Load("asset\\audio\\wan.wav");

	m_Shadow = Manager::AddGameObject<Shadow>();
	m_Shadow->SetScale({ 1.5f, 1.5f, 1.5f });
}

void Player::Uninit()
{
	//影はManager管理なので、プレイヤーが消えるときに一緒に破棄する
	if (m_Shadow)
	{
		m_Shadow->SetDestroy();
		m_Shadow = nullptr;
	}

	GameObject::Uninit();
}

void Player::Update()
{
#if _DEBUG
	ImGui::Begin("PlayerDebug");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", m_Position.x, m_Position.y, m_Position.z);
	ImGui::Text("Rotation: (%.2f, %.2f, %.2f)", m_Rotation.x, m_Rotation.y, m_Rotation.z);
	ImGui::Text("Scale: (%.2f, %.2f, %.2f)", m_Scale.x, m_Scale.y, m_Scale.z);
	ImGui::Text("state: %s", m_NextAnimationName.c_str());
	ImGui::Text("Hit Timer: %.2f", m_HitTimer);
	ImGui::Separator();
	ImGui::Text("Anim : %s(%d) -> %s(%d)",
		m_AnimationName.c_str(), m_AnimationFrame,
		m_NextAnimationName.c_str(), m_NextAnimationFrame);
	ImGui::Text("Blend: %.2f", m_Blend);
	ImGui::Text("Found: cur=%d next=%d",
		m_AnimationModel->HasAnimation(m_AnimationName.c_str()),
		m_AnimationModel->HasAnimation(m_NextAnimationName.c_str()));
	ImGui::Text("Bone : %d / matched cur=%d next=%d",
		m_AnimationModel->GetBoneNum(),
		m_AnimationModel->GetMatchedBoneNum1(),
		m_AnimationModel->GetMatchedBoneNum2());
	ImGui::Separator();
	ImGui::SliderFloat("Speed", &m_Speed, 0.0f, 100.0f);
	ImGui::SliderFloat("Jump Power", &m_jumpPower, 0.0f, 100.0f);
	ImGui::SliderFloat("Gravity", &m_Gravity, 0.0f, 100.0f);
	ImGui::End();
#endif // _DEBUG

	


	float dt = Manager::GetDeltaTime();
	Vector3 bulletoffset = { m_Position.x, m_Position.y + m_Scale.y, m_Position.z };

	CAMERA* camera = Manager::GetGameObject<CAMERA>();
	Vector3 forward = camera->GetForward();
	Vector3 right = camera->GetRight();

	forward.y = 0.0f;
	forward.normalize();

	right.y = 0.0f;
	right.normalize();

	bool move = false;

	//入力による加速
	if (Input::GetKeyPress('W')) 
	{
		m_Velocity += forward * m_Speed * dt; 
		move = true;
	}

	if (Input::GetKeyPress('S')) 
	{ 
		m_Velocity -= forward * m_Speed * dt; 
		move = true;
	}

	if (Input::GetKeyPress('D')) 
	{ 
		m_Velocity += right * m_Speed * dt; 
		move = true;
	}
	if (Input::GetKeyPress('A')) 
	{ 
		m_Velocity -= right * m_Speed * dt; 
		move = true;
	}

	if (move)
	{
		SetAnimation("Run");
		// 移動方向に回転
		m_Rotation.y = -atan2f(m_Velocity.x, -m_Velocity.z);
	}
	else
	{
		SetAnimation("Idle");
	}


	//ジャンプ
	if (Input::GetKeyTrigger(VK_SPACE)&&m_Ground)
	{
		m_Velocity.y += m_jumpPower;
		m_JumpSE->Play();		//ジャンプSE
	}

		//重力
	m_Velocity.y += -m_Gravity * dt;

	m_Position += m_Velocity * dt;

	//抵抗力
	m_Velocity.x += -m_Velocity.x * m_Friction * dt;
	m_Velocity.z += -m_Velocity.z * m_Friction	 * dt;

	bool oldGraund = m_Ground;
	m_Ground = false;

	// 地形の高さ（MeshFieldのないシーンでは従来通り y = 0 を床にする）
	MeshField* meshField = Manager::GetGameObject<MeshField>();
	float height = meshField ? meshField->GetHeight(m_Position) : 0.0f;

	//地面に衝突
	if (m_Position.y < height)
	{
			m_Position.y = height;
			m_Velocity.y = 0.0f;
			m_Ground = true;
	}

	//
	if(m_HitTimer>0.0f) m_HitTimer -= dt;
	if(m_HitTimer<0.0f) m_HitTimer = 0.0f;

	//弾発射
	if (Input::GetMouseTrigger(Input::MOUSE_LEFT))
	{
		Bullet* bullet = Manager::AddGameObject<Bullet>();
		bullet->SetPosition(bulletoffset);
		bullet->SetVelocity(GetForward()*10.0f);
	}

	//影移動（地面と同一平面だとZファイティングするので少し浮かせる）
	if (m_Shadow)
	{
		Vector3 ShadowPos = m_Position;
		ShadowPos.y = height + SHADOW_OFFSET_Y;
		m_Shadow->SetPosition(ShadowPos);
	}
	
	m_AnimationFrame++;
	m_NextAnimationFrame++;

	m_Blend += 0.1f;
	if (m_Blend > 1.0f) m_Blend = 1.0f;

	GameObject::Update();
}

void Player::Draw()
{
	// 点滅
	if (m_HitTimer > 0.0f)
	{
		if (((int)(m_HitTimer * 10.0f)) % 2 == 0)
			return;
	}

	m_AnimationModel->Update(m_AnimationName.c_str(), m_AnimationFrame, m_NextAnimationName.c_str(), m_NextAnimationFrame, m_Blend);

	GameObject::Draw();
}

void Player::SetAnimation(const char* AnimationName)
{
	if (m_NextAnimationName != AnimationName)
	{
		// 次のアニメーションを設定
		m_AnimationName = m_NextAnimationName;
		m_AnimationFrame = m_NextAnimationFrame;

		m_NextAnimationName = AnimationName;
		m_NextAnimationFrame = 0;

		m_Blend = 0.0f;
	}
}

void Player::OnCollision(GameObject* other)
{
	if (dynamic_cast<Enemy*>(other) && m_HitTimer <= 0.0f) m_HitTimer = 1.0f;
}

void Player::OnPushed(const Vector3& push)
{
	//上に押し戻された＝何かの上に乗った
	if (push.y > 0.0f && m_Velocity.y < 0.0f)
	{
		m_Velocity.y = 0.0f;
		m_Ground = true;
	}
	//下に押し戻された＝頭をぶつけた
	else if (push.y < 0.0f && m_Velocity.y > 0.0f)
	{
		m_Velocity.y = 0.0f;
	}

	//横に押し戻された：壁に向かう速度だけ消す（壁に沿って滑れる）
	float len = sqrtf(push.x * push.x + push.z * push.z);
	if (len > 0.0001f)
	{
		float nx = push.x / len;
		float nz = push.z / len;
		float dot = m_Velocity.x * nx + m_Velocity.z * nz;
		if (dot < 0.0f)
		{
			m_Velocity.x -= nx * dot;
			m_Velocity.z -= nz * dot;
		}
	}
}
