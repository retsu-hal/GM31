#pragma once 
//================================================================
//	インクルード
//================================================================
#include"gameObject.h"

//================================================================
//	構造体
//================================================================
class Wave : public GameObject
{
private:
    VERTEX_3D m_Vertex[21][21];

	float m_Time = 0.0f;
public:
    void Init() override;
    void Uninit() override;
    void Update() override;
    void Draw() override;

    float GetHeight(Vector3 position);
};
