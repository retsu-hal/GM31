#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "Camera.h"
#include "Field.h"
#include "Player.h"
#include "Enemy.h"
#include "Tree.h"
#include "Sky.h"
#include "Box.h"

#include "GameScene.h"
#include "ResultScene.h"


void GameScene::Init()
{
	Manager::AddGameObject<CAMERA>();

	Manager::AddGameObject<Sky>();

	Manager::AddGameObject<FIELD>();
	Box* box = Manager::AddGameObject<Box>();
	box->SetPosition({ 2.0f, 0.0f, -3.0f });
	box->SetScale({ 1.0f, 1.0f, 1.0f });

	Manager::AddGameObject<Player>();
	Manager::AddGameObject<Enemy>()->SetPosition({ -2.0f, 0.0f, 1.0f });
	Manager::AddGameObject<Enemy>()->SetPosition({ 0.0f, 0.0f, 1.0f });
	Manager::AddGameObject<Enemy>()->SetPosition({ 2.0f, 0.0f, 1.0f });


	for (int i = 0; i < 20; i++)
	{
		Vector3 pos = { (float)(rand() % 40 - 20),0.0f,(float)(rand() % 40 - 20) };
		Manager::AddGameObject<Tree>()->SetPosition(pos);
	}

	//Manager::AddGameObject<Polygon2D>();
}

void GameScene::Uninit()
{

}

void GameScene::Update()
{
	auto enemies = Manager::GetGameObjects<Enemy>();

	if(enemies.size() == 0)
	{
		Manager::ChangeScene<ResultScene>(2.0f);
	}
}

void GameScene::Draw()
{

}

