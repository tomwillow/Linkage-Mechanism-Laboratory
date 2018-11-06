#pragma once
#include "TControl.h"
class TStatic :
	public TControl
{
public:
	TStatic();
	~TStatic();

	void TStatic::CreateStatic(HWND hParent, UINT uId, TCHAR text[], HINSTANCE hInst);
};

