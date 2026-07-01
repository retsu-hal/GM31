#include "main.h"
#include "manager.h"
#include "input.h"
#include "renderer.h"
#include "Polygon2D.h"
#include "ResultScene.h"
#include "TitleScene.h"


void ResultScene::Init()
{
	Manager::AddGameObject<Polygon2D>()->Init(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, L"asset\\texture\\DemoResult.png");
}

void ResultScene::Uninit()
{

}

void ResultScene::Update()
{
	if (Input::GetKeyTrigger(VK_RETURN))
	{
		Manager::ChangeScene<TitleScene>();
	}
}

void ResultScene::Draw()
{

}

