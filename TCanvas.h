#pragma once
#include "KWindow.h"

class TCanvas:public KWindow
{
private:
	TConfiguration *Config;
	bool bMButtonPressing;
	UINT uiMoveStartX, uiMoveStartY;
public:
	TCanvas();
	~TCanvas();
	void TCanvas::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
	void DealMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnKeyDown(WPARAM wParam, LPARAM lParam);
	void OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnRButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnMButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnMButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnSetCursor(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void TCanvas::OnCommand(WPARAM wParam, LPARAM lParam);
	void OnDraw(HDC hdc);
};