#pragma once
#include "Point2D.h"
class BestNode
{
public:
	BestNode();
	~BestNode();
private:
	Point2D point;
public:
	Point2D GetPoint();
private:
	double criterion;
public:
	double GetCriterion();
	BestNode(Point2D p, double c);
};

