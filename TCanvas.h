#pragma once
#include "TWindow.h"
#include "TDraw.h"

class TConfiguration;
class TManageTool;
class TStatus;
class TTrackbar;
class TShape;
class TCanvas:public TWindow
{
private:
	TDraw Draw;
	TConfiguration *pConfig; 
	TManageTool *pManageTool;
	TStatus *pStatus;
	TTrackbar *pTrackbar;
	TShape *pShape;
	bool bMButtonPressing;
	POINT uiMoveStart;
	void TCanvas::OnCommand(WPARAM wParam, LPARAM lParam) override;

	int ShowId, ShowIndex;
public:
	TCanvas::TCanvas(TConfiguration *pConfig, TManageTool *pManageTool, TStatus *pStatus, TTrackbar *pTrackbar, TShape *pShape);
	~TCanvas();
	void TCanvas::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam) override;
	void DealMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnKeyDown(WPARAM wParam, LPARAM lParam) override;
	void OnLButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void OnRButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void OnRButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)override;
	void OnHotKey(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)override;

	void OnMButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void OnMButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void OnSetCursor(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void OnDraw(HDC hdc) override;

	void TCanvas::SetShowIdAndIndex(int id, int index);
};