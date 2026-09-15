#pragma once

#include "GameObject.h"
#include "Mesh.h"

struct ShaderSet;

class Score : public GameObject
{
private:
	static const int DIGIT_NUM = 6;//桁数

	Mesh m_Mesh;
	const ShaderSet* m_Shader = nullptr;
	ID3D11ShaderResourceView* m_Texture = nullptr;

	int m_Score = 0;        // 実際に表示されるスコア（徐々に増える）
	int m_TargetScore = 0;  // 最終的に到達すべきスコア

	float m_PopTimer = 0.0f;   // ポップ演出用タイマー
	float m_CountTimer = 0.0f; // カウントアップの間隔調整用

public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;

	void AddScore(int value) { m_TargetScore += value; }
	int GetScore() const { return m_Score; }
};
