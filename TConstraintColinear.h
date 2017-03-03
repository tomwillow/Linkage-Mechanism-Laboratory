#pragma once
#include "TElement.h"
class TConstraintColinear :
	public TElement
{
public:
	TConstraintColinear();
	~TConstraintColinear();
	TElement *pElement1, *pElement2;
};

