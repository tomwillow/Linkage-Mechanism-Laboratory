#pragma once
#include "TControl.h"
class TCheckBox :
	public TControl
{
private:
	HICON hIconChecked, hIconUnChecked;
	HBITMAP hBitmapChecked, hBitmapUnChecked;
public:
	TCheckBox();
	~TCheckBox();
	void TCheckBox::SetChecked(bool bChecked);
	void TCheckBox::SetCheckedAndBitmap(bool bChecked);
	bool TCheckBox::GetChecked();

	void TCheckBox::LoadCheckedIcon(HINSTANCE hInst, UINT uiIconId);
	void TCheckBox::LoadUnCheckedIcon(HINSTANCE hInst, UINT uiIconId);
	void TCheckBox::CreateIconCheckBox(HINSTANCE hInst, HWND hParent, int x, int y,int width,int height, UINT id);
	void TCheckBox::SetIconIsChecked();
	void TCheckBox::SetIconIsUnChecked();

	void TCheckBox::LoadCheckedBitmap(HINSTANCE hInst, UINT uiBitmapId);
	void TCheckBox::LoadUnCheckedBitmap(HINSTANCE hInst, UINT uiBitmapId);
	void TCheckBox::CreateBitmapCheckBox(HINSTANCE hInst, HWND hParent, int x, int y, int width, int height, UINT id);
	void TCheckBox::SetBitmapIsChecked();
	void TCheckBox::SetBitmapIsUnChecked();

};

