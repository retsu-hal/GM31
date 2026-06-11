#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "Polygon2D.h"
#include "Camera.h"
#include "Field.h"
#include "Cube.h"
#include "Player.h"
#include "Enemy.h"
#include "Tree.h"


std::list<GameObject* > Manager::m_GameObjects;
float Manager::m_DeltaTime = 1.0f / 60.0f;

void Manager::Init()
{
	Input::Init();
	Renderer::Init();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(GetWindow());
	ImGui_ImplDX11_Init(Renderer::GetDevice(),Renderer::GetDeviceContext());
	

	GameObject* gameObject;
	
	AddGameObject<CAMERA>();

	AddGameObject<FIELD>();
	AddGameObject<Player>();
	AddGameObject<Enemy>()->SetPosition({ -2.0f, 1.0f, 1.0f });
	AddGameObject<Enemy>()->SetPosition({ 0.0f, 1.0f, 1.0f });
	AddGameObject<Enemy>()->SetPosition({ 2.0f, 1.0f, 1.0f });


	for (int i = 0; i < 20; i++)
	{
		Vector3 pos = { (float)(rand() % 40 - 20),1.0f,(float)(rand() % 40 - 20) };
		AddGameObject<Tree>()->SetPosition(pos);
	}

	//AddGameObject<Polygon2D>();


}


void Manager::Uninit()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	
	for (GameObject* gameObject : m_GameObjects)
	{
		if (gameObject != nullptr)
		{
			gameObject->Uninit();
			delete gameObject;
		}
	}
	Renderer::Uninit();
	Input::Uninit();
}

void Manager::Update()
{
	m_DeltaTime = 1.0f / 60.0f;
	Input::Update();
	for (GameObject* gameObject : m_GameObjects)
	{
		if (gameObject != nullptr)
		{
			gameObject->Update();
		}
	}

	m_GameObjects.remove_if([](GameObject* object)
		{
			return object->Destroy();
		});
};

void Manager::Draw()
{
	Renderer::Begin();

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	for (GameObject* gameObject : m_GameObjects)
	{
		if (gameObject != nullptr)
		{
			gameObject->Draw();
		}
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


	Renderer::End();
}
