#pragma once
#include <Windows.h>

#include "TEdit.h"
#include "TTransfer.h"

class TLineEdit:public TEdit
{
private:
	HWND hParent;
public:
	TLineEdit();
	~TLineEdit();
	void TLineEdit::Load(HWND hParent);
	bool TLineEdit::OnKeyDown(WPARAM wParam, LPARAM lParam);
	bool TLineEdit::OnChar(WPARAM wParam, LPARAM lParam);
};