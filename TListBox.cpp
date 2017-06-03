#include "TListBox.h"


TListBox::TListBox()
{
}


TListBox::~TListBox()
{
}

void TListBox::AddString(const TCHAR sz[])
{
	SendMessage(m_hWnd, LB_ADDSTRING, 0, (LPARAM)sz);
}

int TListBox::GetCurSel()
{
	return SendMessage(m_hWnd, LB_GETCURSEL, 0, 0);
}

int TListBox::DeleteCurSel()
{
	return DeleteString(GetCurSel());
}

int TListBox::DeleteString(int ItemIndex)
{
	return SendMessage(m_hWnd, LB_DELETESTRING, ItemIndex, 0);
}