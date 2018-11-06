#pragma once
#include "..\Element\TElement.h"
#include "..\Common\String.h"

class TListView;
class TDriver :
	public TElement
{
protected:
	virtual void TDriver::Draw(HDC hdc, const TConfiguration* pConfig) {}
	virtual void TDriver::DrawPickSquare(HDC hdc, const TConfiguration* pConfig){}
public:
	String sExprLeft;
	String sExprRight;
	TDriver();
	~TDriver();
	bool TDriver::WriteFile(HANDLE &hf, DWORD &now_pos) override;
	bool TDriver::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape) override;
	virtual void TDriver::NoticeListView(TListView *pListView) override;
	virtual const String TDriver::GetElementTypeName() override;//得到类型名称
	virtual void TDriver::ChangePos(DPOINT dptDelta)override{}
};

