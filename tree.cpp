#include "main.h"
#include "tree.h"

void Tree::Init()
{
	SetSize(8.0f, 6.0f);							// 幅8 × 高さ6
	SetTexturePath(L"asset\\texture\\tree.png");	// 木のテクスチャ

	// 板ポリゴン生成・描画はすべてBillboard側で処理
	Billboard::Init();
}
