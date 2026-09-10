/*==============================================================================

[Tree.cpp]
														Author :Watanabe Retsu
														Date   :
--------------------------------------------------------------------------------

==============================================================================*/

//==============================================================================
//インクルード
//==============================================================================
#include "main.h"
#include "Tree.h"
#include  "BillboardRenderer.h"
#include "SpriteAnimation.h"
//==============================================================================
//初期化処理
//==============================================================================


void Tree::Init()
{
	m_Layer = 2;
	BillboardRenderer* renderer = AddComponent<BillboardRenderer>(this);
	renderer->Load(L"asset\\texture\\tree.png");
	renderer->SetMode(BillboardMode::AxisY);		// Y軸だけ回転する
	renderer->SetAnchorBottom(true);					// 足元を原点にする
	renderer->SetSize(7.0f, 7.0f);
}