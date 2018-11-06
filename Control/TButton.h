#pragma once
#include "TControl.h"
class TButton :
	public TControl
{
public:
	TButton();
	~TButton();

	void TButton::CreateButton(HWND hParent, UINT uId, TCHAR text[], HINSTANCE hInst);
};

