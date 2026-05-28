#pragma once

#include "main.h"
#include "GameObject.h"
#include "GameObject.h"

class GameObject;	// ‘O•ûéŒ¾

class Manager
{

private:
	static std::list<GameObject*> m_GameObjects;

public:
	static void Init();
	static void Uninit();
	static void Update();
	static void Draw();

	template<typename T>
	static T* AddGameObject()
	{
		T* gameObject = new T();
		gameObject->Init();
		m_GameObjects.push_back(gameObject);

		return gameObject;
	}

	template<typename T>
	static T* GetGameObject()
	{
		for (GameObject* gameObject : m_GameObjects)
		{
			T* find = dynamic_cast<T*>(gameObject);
			if (find)
			{
				return find;
			}
		}

		return nullptr;
	}

	
};