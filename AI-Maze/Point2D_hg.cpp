#include "Point2D_hg.h"



Point2D_hg::Point2D_hg()
{
}

Point2D_hg::Point2D_hg(Point2D p, Point2D targetPoint)
{
	point = p;
	h = abs(point.GetX() - targetPoint.GetX()) + abs(point.GetY() - targetPoint.GetY());// Manhattan Distance
	g = 0;
	f = h + g;
}


Point2D_hg::Point2D_hg(Point2D_hg parent, Point2D p, Point2D targetPoint)
{
	point = p;
	h = (abs(point.GetX() - targetPoint.GetX()) + abs(point.GetY() - targetPoint.GetY())) * WEIGHT;// Manhattan Distance
	g = parent.g + 1;
	f = h + g;
}


Point2D_hg::~Point2D_hg()
{
}


Point2D Point2D_hg::getPoint()
{
	return point;
}


int Point2D_hg::getF()
{
	return f;
}
