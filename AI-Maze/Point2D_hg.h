#include "Point2D.h"
#include <math.h>

#pragma once

#define WEIGHT 3

class Point2D_hg
{
public:
	Point2D_hg::Point2D_hg();
	Point2D_hg::Point2D_hg(Point2D p, Point2D targetPoint);
	Point2D_hg::Point2D_hg(Point2D_hg parent, Point2D p, Point2D targetPoint);
	~Point2D_hg();
private:
	Point2D point;
	int h;
	int g;
	int f;
public:
	Point2D Point2D_hg::getPoint();
	int getF();
};

