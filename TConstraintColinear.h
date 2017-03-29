#pragma once
#include "TElement.h"
class TConstraintColinear :
	public TElement
{
public:
	TConstraintColinear();
	~TConstraintColinear();
	TElement *pElement[2];
	void TConstraintColinear::NoticeListView(TListView *pListView);
	virtual bool TConstraintColinear::WriteFile(HANDLE &hf, DWORD &now_pos);
	virtual bool TConstraintColinear::ReadFile(HANDLE &hf, DWORD &now_pos,TShape *pShape);
};

