#pragma once

#include "main.h"
#include "Vector3.h"

namespace Collision
{
	// 地面との当たり判定。groundYより下に潜ったら押し戻し、接地したら true。
	bool Ground(Vector3& position, Vector3& velocity, float groundY = 1.0f);

	// 四角(AABB) と 四角(AABB)。half は中心からの半分の大きさ。GetScale() がそのまま使える。
	bool RectAndRect(const Vector3& posA, const Vector3& halfA,
		const Vector3& posB, const Vector3& halfB);

	bool RectAndRect(const Vector3& posA, const Vector3& halfA,
		const Vector3& posB, const Vector3& halfB,
		Vector3& pushA);

	// 四角(AABB) と 丸。丸は XZ 平面の円（高さ無限の円柱）として扱う。
	bool RectAndCircle(const Vector3& rectPos, const Vector3& rectHalf,
		const Vector3& circlePos, float radius);

	bool RectAndCircle(const Vector3& rectPos, const Vector3& rectHalf,
		const Vector3& circlePos, float radius,
		Vector3& pushRect);
}
