#pragma once

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

};