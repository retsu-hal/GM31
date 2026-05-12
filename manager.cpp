#include "main.h"
#include "manager.h"
#include "renderer.h"
#include "Polygon2D.h"
#include "Camera.h"
#include "Field.h"
#include "Cube.h"
#include "keyboard.h"

std::list<GameObject*> Manager::m_GameObjects;

void Manager::Init()
{
	Renderer::Init();
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

	Renderer::End();
}
