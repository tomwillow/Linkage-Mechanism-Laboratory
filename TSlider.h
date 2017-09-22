#pragma once
#include <vector>

#include <Windows.h>

#include "TElement.h"
class TSlider :
	public TElement
{
private:
	DPOINT dpt_1;
protected:
	virtual bool TSlider::InSelWindow(RECT rect, const TConfiguration *pConfig)override;
	virtual bool TSlider::InSelCross(RECT rect, const TConfiguration *pConfig)override;
public:
	struct LinkLine
	{
		int index1;
		int index2;
	};
	std::vector<LinkLine> vecLine;//连接集
	TSlider();
	~TSlider();

	void TSlider::NoticeListView(TListView *pListView)override;
	virtual bool TSlider::WriteFile(HANDLE &hf, DWORD &now_pos)override;
	virtual bool TSlider::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape)override;
	const DPOINT TSlider::GetRelativePointByIndex(int PointIndexOfElement) const override;
	virtual const String TSlider::GetElementTypeName() override;//得到类型名称
	virtual void TSlider::Draw(HDC hdc, const TConfiguration* pConfig) override;
	virtual void TSlider::DrawPickSquare(HDC hdc, const TConfiguration* pConfig) override;
	DPOINT TSlider::GetAbsolutePointByIndex(int PointIndexOfElement) const override;
	virtual bool TSlider::Picked(const POINT &ptPos, const TConfiguration *pConfig)override;
};

