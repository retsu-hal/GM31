#pragma once
#include "GameObject.h"
#include "Mesh.h"

struct ShaderSet;

class Particle :  public GameObject
{
private:

	struct PARTICLE
	{
		Vector3 Position;
		Vector3 Velocity;
		int  Life;
		bool Enable;
	};

	static const int PARTICLE_MAX = 10000;
	PARTICLE m_Particle[PARTICLE_MAX];

	Mesh m_Mesh;
	const ShaderSet* m_Shader = nullptr;
	ID3D11ShaderResourceView* m_Texture = nullptr;

	void Emit(int count);	// 空いているパーティクルを count 個だけ発射する

public:
	void Init() override;
	void Uninit() override;
	void Update() override;
	void Draw() override;
};

