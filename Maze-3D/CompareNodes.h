#pragma once
#include "BestNode.h"
class CompareNodes
{
public:
	CompareNodes();
	~CompareNodes();
	bool operator () (BestNode &b1, BestNode &b2)
	{
		if (b1.GetCriterion() < b2.GetCriterion())
			return true;
		else return false;
	}
};

