/*==============================================================================

[Bullet.cpp]
														Author :Watanabe Retsu
														Date   :
--------------------------------------------------------------------------------

==============================================================================*/

//==============================================================================
//インクルード
//==============================================================================
#include "main.h"
#include "Renderer.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Manager.h"
#include "ModelRenderer.h"
#include "Explosion.h"
#include "Score.h"

//==============================================================================
//初期化処理
//==============================================================================
void Bullet::Init()
{
	m_Layer = 1;
	AddComponent<ModelRenderer>(this)->Load("asset\\model\\bullet.obj");
}

//==============================================================================
//更新処理
//==============================================================================
void Bullet::Update()
{
	float dt = Manager::GetDeltaTime();

	m_Position += m_Velocity * dt;

	//敵との当たり判定
	auto enemies = Manager::GetGameObjects<Enemy>();
	for (auto enemy : enemies)
	{
		Vector3 direction = enemy->GetPosition() - m_Position;
		float lenght = direction.length();

		if (lenght < 1.5f)
		{
			enemy->AddDamage(1);
			SetDestroy();
			Vector3 pos = enemy->GetPosition();
			pos.y += 1.0f;
			Manager::AddGameObject<Explosion>()->SetPosition(pos);

			auto scores = Manager::GetGameObjects<Score>();
			for (auto score : scores)
			{
				score->AddScore(100);
			}

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
