#pragma once
#include <vector>

#include <Windows.h>

#include "TElement.h"
class TSlider :
	public TElement
{
private:
	DPOINT dpt_1;
public:
	struct LinkLine
	{
		int index1;
		int index2;
	};
	std::vector<LinkLine> vecLine;//Á¬½Ó¼¯
	TSlider();
	~TSlider();

	void TSlider::NoticeListView(TListView *pListView)override;
	virtual bool TSlider::WriteFile(HANDLE &hf, DWORD &now_pos)override;
	virtual bool TSlider::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape)override;
	const DPOINT TSlider::GetRelativePointByIndex(int PointIndexOfElement) const override;
	DPOINT TSlider::GetAbsolutePointByIndex(int PointIndexOfElement) const override;
};

