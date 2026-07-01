#include "main.h"
#include "manager.h"
#include "input.h"
#include "renderer.h"
#include "Polygon2D.h"
#include "TitleScene.h"
#include "GameScene.h"


void TitleScene::Init()
{
	Manager::AddGameObject<Polygon2D>()->Init(0.0f,0.0f,SCREEN_WIDTH,SCREEN_HEIGHT,L"asset\\texture\\DemoLogo.png");
}

void TitleScene::Uninit()
{

}

void TitleScene::Update()
{
	if(Input::GetKeyTrigger(VK_RETURN))
	{
		Manager::ChangeScene<GameScene>();
	}
}

void TitleScene::Draw()
{

}

