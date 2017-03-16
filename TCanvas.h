#pragma once
#include "TWindow.h"

class TConfiguration;
class TCanvas:public TWindow
{
private:
	TConfiguration *Config;
	bool bMButtonPressing;
	UINT uiMoveStartX, uiMoveStartY;
	void TCanvas::OnCommand(WPARAM wParam, LPARAM lParam) override;
public:
	TCanvas();
	~TCanvas();
	void TCanvas::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam) override;
	void DealMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnKeyDown(WPARAM wParam, LPARAM lParam) override;
	void OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void OnRButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void OnMButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void OnMButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void OnSetCursor(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
	void OnDraw(HDC hdc) override;
};