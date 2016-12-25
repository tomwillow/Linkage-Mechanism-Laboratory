#pragma once
#include "TLineEdit.h"


TLineEdit::TLineEdit()
{
}


TLineEdit::~TLineEdit()
{
}

bool TLineEdit::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_ESCAPE:
		::SendMessage(hParent, WM_KEYDOWN, wParam, lParam);
		return false;
	case VK_SPACE:
	case VK_RETURN:
		::SendMessage(hParent, WM_KEYDOWN, wParam, lParam);
		return false;
	}
	return true;
}

bool TLineEdit::OnChar(WPARAM wParam, LPARAM lParam)
{
	return true;
}