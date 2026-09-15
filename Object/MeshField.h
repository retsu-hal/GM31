#pragma once 
//================================================================
//	インクルード
//================================================================
#include"gameObject.h"
#include"Mesh.h"

//================================================================
//	構造体
//================================================================
struct ShaderSet;

class MeshField : public GameObject
{
private:
    static const int DIV = 20;                                      // 1辺のマス数
    static const int VERTEX_NUM = DIV + 1;                          // 1辺の頂点数
    static const int INDEX_NUM = (VERTEX_NUM * 2 + 2) * DIV - 2;    // 縮退ポリゴン込みのインデックス数

    VERTEX_3D m_Vertex[VERTEX_NUM][VERTEX_NUM];

    Mesh m_Mesh;
    const ShaderSet* m_Shader = nullptr;
    ID3D11ShaderResourceView* m_Texture = nullptr;

    void CreateVertex();    // 高さテーブルから頂点と法線を作る
    void CreateIndex();     // 三角形ストリップ用のインデックスを作る

public:
    void Init() override;
    void Uninit() override;
    void Draw() override;

    float GetHeight(Vector3 position);
};
