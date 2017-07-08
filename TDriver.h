#pragma once
#include "TElement.h"
#include "String.h"

class TListView;
class TDriver :
	public TElement
{
public:
	String sExprLeft;
	String sExprRight;
	TDriver();
	~TDriver();
	bool TDriver::WriteFile(HANDLE &hf, DWORD &now_pos) override;
	bool TDriver::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape) override;
	virtual void TDriver::NoticeListView(TListView *pListView) override;
	virtual const TCHAR * TDriver::GetElementTypeName(TCHAR name[]) override;//得到类型名称
};

