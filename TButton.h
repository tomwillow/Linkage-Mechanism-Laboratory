#pragma once
#include "TControl.h"
class TButton :
	public TControl
{
private:
	HICON hIconChecked, hIconUnChecked;
	HBITMAP hBitmapChecked, hBitmapUnChecked;
public:
	TButton();
	~TButton();
	void TButton::SetChecked(bool bChecked);
	void TButton::SetCheckedAndBitmap(bool bChecked);
	bool TButton::GetChecked();

	void TButton::LoadCheckedIcon(HINSTANCE hInst, UINT uiIconId);
	void TButton::LoadUnCheckedIcon(HINSTANCE hInst, UINT uiIconId);
	void TButton::CreateIconCheckBox(HINSTANCE hInst, HWND hParent, int x, int y, UINT id);
	void TButton::SetIconIsChecked();
	void TButton::SetIconIsUnChecked();

	void TButton::LoadCheckedBitmap(HINSTANCE hInst, UINT uiBitmapId);
	void TButton::LoadUnCheckedBitmap(HINSTANCE hInst, UINT uiBitmapId);
	void TButton::CreateBitmapCheckBox(HINSTANCE hInst, HWND hParent, int x, int y, UINT id);
	void TButton::SetBitmapIsChecked();
	void TButton::SetBitmapIsUnChecked();

};

