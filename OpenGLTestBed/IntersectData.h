#pragma once
class IntersectData
{
public:
	IntersectData(const bool doesIntersect, const float distance);
	bool getDoesIntersect();
	float getDistance();
	~IntersectData();
private:
	bool doesIntersect;
	float distance;
};


