#include "IntersectData.h"

IntersectData::IntersectData(const bool doesIntersect, const float distance)
{
	this->doesIntersect = doesIntersect;
	this->distance = distance;
}

bool IntersectData::getDoesIntersect()
{
	return doesIntersect;
}

float IntersectData::getDistance()
{
	return distance;
}

IntersectData::~IntersectData()
{
}
