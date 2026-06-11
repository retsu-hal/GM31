#pragma once

#include "main.h"
#include "GameObject.h"
#include "GameObject.h"

class GameObject;	// ëOï˚êÈåæ

class Manager
{

private:
	static std::list<GameObject*> m_GameObjects;
	static float m_DeltaTime;

public:
	static void Init();
	static void Uninit();
	static void Update();
	static void Draw();
	static float GetDeltaTime() { return m_DeltaTime; }

	template<typename T>
	static T* AddGameObject()
	{
		T* gameObject = new T();
		gameObject->Init();
		m_GameObjects.push_back(gameObject);

		return gameObject;
	}

	static void RemoveGameObject(GameObject* gameobject)
	{
		m_GameObjects.remove(gameobject);
		delete gameobject;
	}

	template<typename T>
	static T* GetGameObject()
	{
		for (GameObject* gameObject : m_GameObjects)
		{
			T* find = dynamic_cast<T*>(gameObject);
			if (find!=nullptr)
			{
				return find;
			}
		}

		return nullptr;
	}

	template<typename T>
	static std::vector<T*>GetGameObjects()
	{
		std::vector<T*> gameObjects;
		for (GameObject* gameObject : m_GameObjects)
		{
			T* find = dynamic_cast<T*>(gameObject);
			if (find != nullptr)
			{
				gameObjects.push_back(find);
			}
		}
		return gameObjects;
	}
};