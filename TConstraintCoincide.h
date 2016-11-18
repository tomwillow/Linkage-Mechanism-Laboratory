#pragma once
#include "TElement.h"

class TListView;
class TConstraintCoincide :
	public TElement
{
public:
	EnumElementType eElementType1, eElementType2;
	int ElementId1, ElementId2;
	int Element1PointIndex, Element2PointIndex;
	TConstraintCoincide();
	~TConstraintCoincide();
	void TConstraintCoincide::NoticeListView(TListView *pListView);
};

