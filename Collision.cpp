#include "main.h"
#include "Collision.h"
#include <cmath>

namespace
{
	float Clampf(float v, float lo, float hi)
	{
		if (v < lo) return lo;
		if (v > hi) return hi;
		return v;
	}
}

namespace Collision
{
	bool Ground(Vector3& position, Vector3& velocity, float groundY)
	{
		if (position.y < groundY)
		{
			position.y = groundY;
			velocity.y = 0.0f;
			return true;
		}
		return false;
	}

	// 四角 と 四角
	bool RectAndRect(const Vector3& posA, const Vector3& halfA,
		const Vector3& posB, const Vector3& halfB)
	{
		return	fabsf(posB.x - posA.x) < halfA.x + halfB.x &&
				fabsf(posB.y - posA.y) < halfA.y + halfB.y &&
				fabsf(posB.z - posA.z) < halfA.z + halfB.z;
	}

	bool RectAndRect(const Vector3& posA, const Vector3& halfA,
		const Vector3& posB, const Vector3& halfB,
		Vector3& pushA)
	{
		Vector3 d = posB - posA;   // A から B へのベクトル

		// 各軸のめり込み量（正なら重なっている）
		float px = (halfA.x + halfB.x) - fabsf(d.x);
		if (px <= 0.0f) return false;
		float py = (halfA.y + halfB.y) - fabsf(d.y);
		if (py <= 0.0f) return false;
		float pz = (halfA.z + halfB.z) - fabsf(d.z);
		if (pz <= 0.0f) return false;

		// 一番浅い軸の方向にだけ押し戻す
		pushA = { 0.0f, 0.0f, 0.0f };
		if (px <= py && px <= pz)
			pushA.x = (d.x > 0.0f) ? -px : px;
		else if (py <= pz)
			pushA.y = (d.y > 0.0f) ? -py : py;
		else
			pushA.z = (d.z > 0.0f) ? -pz : pz;

		return true;
	}

	// 四角 と 丸
	bool RectAndCircle(const Vector3& rectPos, const Vector3& rectHalf,
		const Vector3& circlePos, float radius)
	{
		float closestX = Clampf(circlePos.x, rectPos.x - rectHalf.x, rectPos.x + rectHalf.x);
		float closestZ = Clampf(circlePos.z, rectPos.z - rectHalf.z, rectPos.z + rectHalf.z);

		float dx = closestX - circlePos.x;
		float dz = closestZ - circlePos.z;
		return (dx * dx + dz * dz) < radius * radius;
	}

	bool RectAndCircle(const Vector3& rectPos, const Vector3& rectHalf,
		const Vector3& circlePos, float radius,
		Vector3& pushRect)
	{
		float minX = rectPos.x - rectHalf.x, maxX = rectPos.x + rectHalf.x;
		float minZ = rectPos.z - rectHalf.z, maxZ = rectPos.z + rectHalf.z;

		// 円の中心に最も近い、矩形上の点
		float closestX = Clampf(circlePos.x, minX, maxX);
		float closestZ = Clampf(circlePos.z, minZ, maxZ);

		float dx = closestX - circlePos.x;   // 円の中心 から 最近点
		float dz = closestZ - circlePos.z;
		float distSq = dx * dx + dz * dz;

		if (distSq > radius * radius) return false;   // 当たっていない

		pushRect = { 0.0f, 0.0f, 0.0f };

		if (distSq > 0.0001f)
		{
			// 通常ケース：円の中心は矩形の外側
			float dist = sqrtf(distSq);
			float overlap = radius - dist;
			pushRect.x = (dx / dist) * overlap;   // 矩形を円から遠ざける向き
			pushRect.z = (dz / dist) * overlap;
		}
		else
		{
			// 円の中心が矩形の内側 → 一番近い辺の外へ押し出す
			float left  = circlePos.x - minX;   // 各辺までの距離
			float right = maxX - circlePos.x;
			float nearZ = circlePos.z - minZ;
			float farZ  = maxZ - circlePos.z;

			float minPen = left; int side = 0;          // 0:-x 1:+x 2:-z 3:+z
			if (right < minPen) { minPen = right; side = 1; }
			if (nearZ < minPen) { minPen = nearZ; side = 2; }
			if (farZ  < minPen) { minPen = farZ;  side = 3; }

			switch (side)
			{
			case 0: pushRect.x =  (left  + radius); break;
			case 1: pushRect.x = -(right + radius); break;
			case 2: pushRect.z =  (nearZ + radius); break;
			case 3: pushRect.z = -(farZ  + radius); break;
			}
		}
		return true;
	}
}
