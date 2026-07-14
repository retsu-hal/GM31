#pragma once
#include "Billboard.h"

// Billboardを継承した木。サイズとテクスチャを指定するだけ
class Tree : public Billboard
{
public:
	void Init() override;
};
