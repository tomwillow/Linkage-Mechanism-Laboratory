#pragma once
#include <stack>
#include <vector>
#include <map>
#include <set>
#include "TTool.h"
#include "String.h"

#include "DPOINT.h"

class TAttach;
class TRealLine;
class TElement;
class TSelectTool:public TTool
{
protected:
	enum enumState{NONE,MOVE_SEL,DRAG_SEL} eState;
	enum enumSelectMode{ SELECT_MOVE, SELECT_DRAG };
	enumSelectMode eMode;
private:
	TAttach *pAttach;
	TCHAR *Cursor;

	bool bSelCross;//交选
	bool bShowSelRect;//显示选择框
	//std::vector<POINT> vecptHit;
	RECT rcSelect;
	POINT ptDrag;//拖动起点，中途变更为上一次拖动点

	class TPickedData
	{
		using DataType = std::set<TElement *>;
		DataType data;
	public:
		DataType::size_type size(){ return data.size(); }
		DataType::iterator begin(){ return data.begin(); }
		DataType::iterator end(){ return data.end(); }
		void clear(){ data.clear(); }
		TElement* front(){ return *data.begin(); }
		void push_back(TElement *pElement){ data.insert(pElement); }
		DataType& self(){ return data; }
	};
	//std::vector<TElement*> PickedElements;
	TPickedData PickedElements;

	void TSelectTool::Draw(HDC hdc);
	void OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam) override;
	void OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos) override;
	void OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos) override;
	void OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos) override;
	void OnLButtonUp(HWND hWnd, UINT nFlags, POINT ptPos) override;
	void OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos) override;
	void OnRButtonUp(HWND hWnd, UINT uMsg, POINT ptPos)override;
	void OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam)override;

	void TSelectTool::CancelTreeViewAndListView();
	void TSelectTool::SelectNull();
	void TSelectTool::Select(std::vector<TElement*> &vecSel);
	void TSelectTool::Select(const TElement *pElement, bool ChooseTreeView, bool UpdateListView);
	void TSelectTool::SelectAll();
public:
	TSelectTool();
	~TSelectTool(); 
	bool TSelectTool::CanBeDriver(TElement *&pElementFront);
	void TSelectTool::SelectById(int id,bool ChooseTreeView, bool UpdateListView);
};

