#pragma once
#include "TEdit.h"
#include "enumCtrlType.h"

class TElement;
class TListViewEdit :
	public TEdit
{
private:
	void TListViewEdit::ChangeSource();
public:
	TElement *pElement;
	enumCtrlType eCtrlType;
	void *pContent;
	TCHAR *Text;
	int ListItemIndex;
	TListViewEdit();
	~TListViewEdit();
	bool TListViewEdit::OnKillFocus(WPARAM wParam, LPARAM lParam) override;
	bool TListViewEdit::OnChar(WPARAM wParam, LPARAM lParam) override;
	void CDECL TListViewEdit::SetText(TCHAR szFormat[], ...);
	void TListViewEdit::GetText(TCHAR text[]);
};

