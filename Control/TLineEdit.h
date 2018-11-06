#pragma once
#include <Windows.h>

#include "..\Control\TEdit.h"

class TLineEdit:public TEdit
{
public:
	TLineEdit();
	~TLineEdit();
	bool TLineEdit::OnKeyDown(WPARAM wParam, LPARAM lParam);
};