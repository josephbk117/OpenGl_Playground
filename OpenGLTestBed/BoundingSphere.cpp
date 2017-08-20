#include "BoundingSphere.h"
#include <iostream>

BoundingSphere::BoundingSphere(const vec3 & center, const float & radius)
{
	this->center = center;
	this->radius = radius;
}

const vec3 BoundingSphere::getCenter()
{
	return this->center;
}

const float BoundingSphere::getRadius()
{
	return this->radius;
}

IntersectData BoundingSphere::intersectBoundingSphere(const BoundingSphere& other)
{
	float radiusDistance = radius + other.radius;
	float centerDistance = (other.center - center).length();

	std::cout << "Other radius = " << other.radius << std::endl;
	std::cout << "center distance = " << centerDistance << std::endl;

	return IntersectData(centerDistance < radiusDistance, centerDistance - radiusDistance);
}

BoundingSphere::~BoundingSphere()
{
}
