#include "Point2D.h"



Point2D::Point2D()
	: x(0)
{
}


Point2D::~Point2D()
{
}


Point2D::Point2D(int x, int y)
{
	this->x = x;
	this->y = y;
}


int Point2D::GetX()
{
	return x;
}


int Point2D::GetY()
{
	return y;
}
