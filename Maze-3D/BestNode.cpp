#include "BestNode.h"



BestNode::BestNode()
	: point(0,0)
	, criterion(0)
{
}


BestNode::~BestNode()
{
}


Point2D BestNode::GetPoint()
{
	return point;
}


double BestNode::GetCriterion()
{
	return criterion;
}


BestNode::BestNode(Point2D p, double c)
{
	point = p;
	criterion = c;
}
