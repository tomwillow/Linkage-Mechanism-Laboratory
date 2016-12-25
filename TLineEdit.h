#pragma once
#include <Windows.h>

#include "TEdit.h"

class TLineEdit:public TEdit
{
private:
public:
	TLineEdit();
	~TLineEdit();
	bool TLineEdit::OnKeyDown(WPARAM wParam, LPARAM lParam);
	bool TLineEdit::OnChar(WPARAM wParam, LPARAM lParam);
};