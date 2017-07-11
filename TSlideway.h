#pragma once
#include "TRealLine.h"
class TSlideway :
	public TRealLine
{
public:
	int ShadowQuadrant;
	int ShadowLength;
	int SlidewayStyle;
	TSlideway();
	~TSlideway();
	TSlideway& TSlideway::operator=(const TSlideway &Slideway);
	TSlideway& TSlideway::operator=(const TRealLine &RealLine);
	virtual bool TSlideway::WriteFile(HANDLE &hf, DWORD &now_pos) override;
	virtual bool TSlideway::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape) override;
	virtual void TSlideway::NoticeListView(TListView *pListView) override;
	virtual const TCHAR * TSlideway::GetElementTypeName(TCHAR name[]) override;//得到类型名称
	virtual void TSlideway::Draw(HDC hdc, const TConfiguration* pConfig) override;
};

