#pragma once
#include "TElement.h"

class TListView;
class TConstraintCoincide :
	public TElement
{
public:
	TElement *pElement[2];
	DPOINT *pDpt[2];
	int PointIndexOfElement[2];//0:»ú¼Ü 1,2:Ïß
	TConstraintCoincide();
	~TConstraintCoincide();
	void TConstraintCoincide::NoticeListView(TListView *pListView);
	void TConstraintCoincide::BuildpDpt();
	void TConstraintCoincide::RestorePointStyle();
};

