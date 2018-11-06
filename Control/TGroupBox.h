#pragma once
#include "TControl.h"
class TGroupBox :
	public TControl
{
public:
	TGroupBox();
	~TGroupBox();

	void TGroupBox::CreateGroupBox(HWND hParent, UINT uId, TCHAR text[], HINSTANCE hInst);
};

