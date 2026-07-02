#include "collision.h"
#include <cmath>

//=====================================================================================
// AABB vs AABB
//=====================================================================================
bool Collision::AABB(const Vector3& posA, const Vector3& scaleA,
	const Vector3& posB, const Vector3& scaleB,
	Vector3& outPushVector, bool& outIsGround)
{
	outPushVector = { 0.0f, 0.0f, 0.0f };
	outIsGround = false;

	Vector3 aMin = { posA.x - scaleA.x * 0.5f, posA.y - scaleA.y * 0.5f, posA.z - scaleA.z * 0.5f };
	Vector3 aMax = { posA.x + scaleA.x * 0.5f, posA.y + scaleA.y * 0.5f, posA.z + scaleA.z * 0.5f };

	Vector3 bMin = { posB.x - scaleB.x * 0.5f, posB.y - scaleB.y * 0.5f, posB.z - scaleB.z * 0.5f };
	Vector3 bMax = { posB.x + scaleB.x * 0.5f, posB.y + scaleB.y * 0.5f, posB.z + scaleB.z * 0.5f };

	bool overlapX = aMax.x > bMin.x && aMin.x < bMax.x;
	bool overlapY = aMax.y > bMin.y && aMin.y < bMax.y;
	bool overlapZ = aMax.z > bMin.z && aMin.z < bMax.z;

	if (!(overlapX && overlapY && overlapZ))
		return false;

	float overlapLeft = aMax.x - bMin.x;
	float overlapRight = bMax.x - aMin.x;
	float overlapBottom = aMax.y - bMin.y;
	float overlapTop = bMax.y - aMin.y;
	float overlapFront = aMax.z - bMin.z;
	float overlapBack = bMax.z - aMin.z;

	float minOverlap = overlapLeft;
	int axis = 0;

	if (overlapRight < minOverlap) { minOverlap = overlapRight;  axis = 1; }
	if (overlapBottom < minOverlap) { minOverlap = overlapBottom; axis = 2; }
	if (overlapTop < minOverlap) { minOverlap = overlapTop;    axis = 3; }
	if (overlapFront < minOverlap) { minOverlap = overlapFront;  axis = 4; }
	if (overlapBack < minOverlap) { minOverlap = overlapBack;   axis = 5; }

	switch (axis)
	{
	case 0: outPushVector.x = -minOverlap; break;
	case 1: outPushVector.x = minOverlap; break;
	case 2: outPushVector.y = -minOverlap; break;
	case 3: outPushVector.y = minOverlap; outIsGround = true; break;
	case 4: outPushVector.z = -minOverlap; break;
	case 5: outPushVector.z = minOverlap; break;
	}

	return true;
}

//=====================================================================================
// 球 vs 球
//=====================================================================================
bool Collision::Sphere(const Vector3& posA, float radiusA,
	const Vector3& posB, float radiusB)
{
	Vector3 diff = posB - posA;
	float distSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;
	float radiusSum = radiusA + radiusB;

	return distSq < (radiusSum * radiusSum);
}

//=====================================================================================
// 円柱（XZ平面のみ）
//=====================================================================================
bool Collision::Circle2D(const Vector3& posA, float radiusA,
	const Vector3& posB, float radiusB,
	Vector3& outPushVector)
{
	outPushVector = { 0.0f, 0.0f, 0.0f };

	float dx = posA.x - posB.x;
	float dz = posA.z - posB.z;
	float distSq = dx * dx + dz * dz;
	float minDist = radiusA + radiusB;

	if (distSq >= minDist * minDist || distSq <= 0.0001f)
		return false;

	float dist = sqrtf(distSq);
	float overlap = minDist - dist;
	float nx = dx / dist;
	float nz = dz / dist;

	outPushVector.x = nx * overlap;
	outPushVector.z = nz * overlap;

	return true;
}

//=====================================================================================
// 点 vs 球
//=====================================================================================
bool Collision::PointInSphere(const Vector3& point,
	const Vector3& center, float radius)
{
	Vector3 diff = center - point;
	float distSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

	return distSq < (radius * radius);
}