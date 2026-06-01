#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "Polygon2D.h"
#include "Camera.h"
#include "Field.h"
#include "Cube.h"
#include "Player.h"
#include "Enemy.h"


std::list<GameObject* > Manager::m_GameObjects;

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
	//AddGameObject<CUBE>();
	AddGameObject<FIELD>();
	AddGameObject<Player>();
	//AddGameObject<Enemy>()->SetPosition({ -2.0f, 2.0f, 1.0f });
	//AddGameObject<Enemy>()->SetPosition({ 0.0f, 2.0f, 1.0f });
	//AddGameObject<Enemy>()->SetPosition({ 2.0f, 2.0f, 1.0f });

	AddGameObject<Polygon2D>();


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
	Input::Update();
	for(GameObject* gameObject : m_GameObjects)
	{
		if(gameObject != nullptr)
		{
			gameObject->Update();
		}
	}
}

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

	ImGui::Begin("Inspector");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	for (GameObject* gameObject : m_GameObjects)
	{
		if (gameObject == nullptr) continue;

		ImGui::PushID(gameObject);                       // 同名ヘッダーのID衝突を防ぐ
		if (ImGui::CollapsingHeader(gameObject->GetName()))
		{
			gameObject->DrawImGui();
		}
		ImGui::PopID();
	}
	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());


	Renderer::End();
}
