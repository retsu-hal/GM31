#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "Polygon2D.h"
#include "Camera.h"
#include "Field.h"
#include "Cube.h"
#include "keyboard.h"


std::list<GameObject* > Manager::m_GameObjects;

void Manager::Init()
{
	Renderer::Init();
#if _DEBUG
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(GetWindow());
	ImGui_ImplDX11_Init(Renderer::GetDevice(),Renderer::GetDeviceContext());
#endif
	Keyboard_Initialize();

	GameObject* gameObject;
	
	gameObject = new CAMERA();
	gameObject->Init();
	m_GameObjects.push_back(gameObject);
	

	gameObject = new CUBE();
	gameObject->Init();
	m_GameObjects.push_back(gameObject);

	gameObject = new FIELD();
	gameObject->Init();
	m_GameObjects.push_back(gameObject);

	gameObject = new Polygon2D();
	gameObject->Init();
	m_GameObjects.push_back(gameObject);
}


void Manager::Uninit()
{
#if  _DEBUG
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif

	Renderer::Uninit();
	for (GameObject* gameObject : m_GameObjects)
	{
		if (gameObject != nullptr)
		{
			gameObject->Uninit();
			delete gameObject;
		}
	}
}

void Manager::Update()
{
	keycopy();
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

	for (GameObject* gameObject : m_GameObjects)
	{
		if (gameObject != nullptr)
		{
			gameObject->Draw();
		}
	}

#if _DEBUG
	// �t���[���J�n
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// ---- �E�B�W�F�b�g�`�ʂ͂����ɏ��� ----
	ImGui::Begin("Debug");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);

	CAMERA* camera = dynamic_cast<CAMERA*>(m_GameObjects.front());
	if (camera)
	{
		Vector3 camPos = camera->GetPosition();
		Vector3 camAngle = camera->GetAngle();
		ImGui::Text("CameraPos: (%.1f, %.1f, %.1f)", camPos.x, camPos.y, camPos.z);
		ImGui::Text("CameraAngle: (%.1f, %.1f, %.1f)", camAngle.x, camAngle.y, camAngle.z);
		ImGui::Text("CameraFov: %.1f", camera->GetFov());
	}
	ImGui::End();
	// ----------------------------------

	// �����_�����O
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

#endif
	Renderer::End();
}
