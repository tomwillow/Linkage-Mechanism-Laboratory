#pragma once
#include <Windows.h>
#include "..\Common\String.h"

//为避免重复刷新，工具类及其派生类不得发送WM_PAINT消息，刷新动作由Canvas完成。
//class TMainWindow;
class TCanvas;
class TShape;
class TConfiguration;
class TListView;
class TTreeViewContent;
class TSolver;
class TStatus;
class TElement;
class TTool
{
private:
protected:
	//TMainWindow *pwin;
	HINSTANCE hInst;
	TShape *pShape;
	TCanvas *pCanvas;
	HWND hwndWin;
	TConfiguration *pConfig;
	TListView *pListView;
	TTreeViewContent *pTreeViewContent;
	TStatus *pStatus;
	TSolver *pSolver;
	RECT &ClientRect;
	//void TTool::RefreshTreeViewContent();
	POINT ptMouse;
	bool bShowTips;
	String sTips;
public:
	TTool();
	virtual ~TTool();
	virtual void OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)  {}
	virtual void OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)   { }
	virtual void OnLButtonUp(HWND hWnd, UINT nFlags, POINT ptPos)   { }
	virtual void OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)   { }
	virtual void OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)   { }
	virtual void OnRButtonUp(HWND hWnd, UINT uMsg, POINT ptPos){}
	virtual void OnHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam){}
	virtual void OnMouseWheel(HWND hWnd, UINT nFlags, POINT ptPos)   { }
	virtual void OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)   { }
	virtual void Draw(HDC hdc){}
	void TTool::ResetTool();
	void TTool::RefreshCanvas();
	void TTool::RefreshEquations();
	void TTool::AddTreeViewItem(TElement *pElement, int iId);
};

