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
	case VK_SPACE:
	case VK_RETURN:
		::SendMessage(hParent, WM_KEYDOWN, wParam, lParam);
		return false;
	default:
		::PostMessage(hParent, WM_KEYDOWN, wParam, lParam);//发送父窗口，先添加字符再由父窗口处理
		return true;
	}
	return true;
}

bool TLineEdit::OnChar(WPARAM wParam, LPARAM lParam)
{
	return true;
}