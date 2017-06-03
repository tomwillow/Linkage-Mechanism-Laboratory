#pragma once
#include "TControl.h"
class TListBox :
	public TControl
{
public:
	TListBox();
	~TListBox();
	void TListBox::AddString(const TCHAR sz[]);
	int TListBox::GetCurSel();
	int TListBox::DeleteCurSel();
	int TListBox::DeleteString(int ItemIndex);
};

