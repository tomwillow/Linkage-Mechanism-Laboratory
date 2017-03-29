#pragma once
#include "TElement.h"

class TListView;
class TConstraintCoincide :
	public TElement
{
public:
	TElement *pElement[2];
	DPOINT *pDpt[2];
	int PointIndexOfElement[2];//
	TConstraintCoincide();
	~TConstraintCoincide();
	void TConstraintCoincide::NoticeListView(TListView *pListView);
	void TConstraintCoincide::BuildpDpt();
	void TConstraintCoincide::RestorePointStyle();
	virtual bool TConstraintCoincide::WriteFile(HANDLE &hf, DWORD &now_pos);
	virtual bool TConstraintCoincide::ReadFile(HANDLE &hf, DWORD &now_pos,TShape *pShape);
};

