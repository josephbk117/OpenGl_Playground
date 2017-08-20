#pragma once
#include <GLM\glm.hpp>
#include "IntersectData.h"
using namespace glm;

class BoundingSphere
{
public:
	BoundingSphere(const vec3& center, const float& radius);
	const vec3 getCenter();
	const float getRadius();
	IntersectData intersectBoundingSphere(const BoundingSphere& other);
	~BoundingSphere();
private:
	vec3 center;
	float radius;
};


