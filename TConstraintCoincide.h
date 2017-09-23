#pragma once
#include "TConstraint.h"

class TListView;
class TConstraintCoincide :
	public TConstraint
{
private:
public:
	int PointIndexOfElement[2];//
	void TConstraintCoincide::BuildpDpt();
	void TConstraintCoincide::BuildpDpt_inner(int i);
	TConstraintCoincide();
	~TConstraintCoincide();
	virtual const String TConstraintCoincide::GetElementTypeName() override;//得到类型名称
	void TConstraintCoincide::NoticeListView(TListView *pListView) override;
	virtual bool TConstraintCoincide::WriteFile(HANDLE &hf, DWORD &now_pos) override;
	virtual bool TConstraintCoincide::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape) override;
	virtual void TConstraintCoincide::Draw(HDC hdc, const TConfiguration* pConfig) override;
	virtual void TConstraintCoincide::DrawPickSquare(HDC hdc, const TConfiguration* pConfig) override;
	virtual void TConstraintCoincide::ChangePos(DPOINT dptDelta)override{}
	virtual bool TConstraintCoincide::Picked(const POINT &ptPos, const TConfiguration *pConfig)override;

	void TConstraintCoincide::RestorePointStyle();
	DPOINT TConstraintCoincide::GetLinkDpt(int index);

	//TConstraintCoincide& TConstraintCoincide::operator=(const TConstraintCoincide &element);
};

