#pragma once
#include "TElement.h"
class TConstraintColinear :
	public TElement
{
public:
	TConstraintColinear();
	~TConstraintColinear();
	TElement *pElement[2];
	int PointBeginIndexOfElement[2];//Pi点 Pj点 序号（vecDpt序号）
	int PointEndIndexOfElement[2];//Qi点 Qj点 序号（vecDpt序号）
	virtual const TCHAR * TConstraintColinear::GetElementTypeName(TCHAR name[]) override;//得到类型名称
	void TConstraintColinear::NoticeListView(TListView *pListView);
	virtual bool TConstraintColinear::WriteFile(HANDLE &hf, DWORD &now_pos);
	virtual bool TConstraintColinear::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape);
	void TConstraintColinear::GetLinkDpt(int iLinkIndex,DPOINT &dpt1,DPOINT &dpt2)const;
};

