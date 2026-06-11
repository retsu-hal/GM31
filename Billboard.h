#pragma once
#include "GameObject.h"

// 常にカメラへ正対する板ポリゴン（ビルボード）
class Billboard : public GameObject
{
private:
	float m_Width = 1.0f;		// 板の幅
	float m_Height = 1.0f;		// 板の高さ
	const wchar_t* m_TexturePath = L"asset\\texture\\tree.png";	// テクスチャパス

public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	// 生成前にサイズ・テクスチャを変更したい場合に使用
	void SetSize(float width, float height) { m_Width = width; m_Height = height; }
	void SetTexturePath(const wchar_t* path) { m_TexturePath = path; }
};
