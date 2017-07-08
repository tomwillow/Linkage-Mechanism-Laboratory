#pragma once
#include "TElement.h"

class TListView;
class TConstraintCoincide :
	public TElement
{
private:
	//const DPOINT *pDpt[2];
public:
	TElement *pElement[2];
	int PointIndexOfElement[2];//
	TConstraintCoincide();
	~TConstraintCoincide();
	virtual const TCHAR * TConstraintCoincide::GetElementTypeName(TCHAR name[]) override;//得到类型名称
	void TConstraintCoincide::NoticeListView(TListView *pListView) override;
	virtual bool TConstraintCoincide::WriteFile(HANDLE &hf, DWORD &now_pos) override;
	virtual bool TConstraintCoincide::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape) override;

	void TConstraintCoincide::BuildpDpt_inner(int i);
	void TConstraintCoincide::BuildpDpt();
	void TConstraintCoincide::RestorePointStyle();
	DPOINT TConstraintCoincide::GetLinkDpt(int index);
};

