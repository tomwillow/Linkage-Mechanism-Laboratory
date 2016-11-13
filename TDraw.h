#pragma once
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "TLine.h"
#include "TRealLine.h"
#include "TConfiguration.h"

class TFramePoint;
class TDraw
{
public:
	TDraw();
	~TDraw();

	static void TDraw::Move(POINT apt[], int apt_num, double angle, double dist);
	static void Rotate(POINT apt[], int apt_num, int Ox, int Oy, double theta);
	static void TDraw::MirrorX(POINT apt[], int apt_num, int Oy);
	static double TDraw::Distance(POINT pt1, POINT pt2);

	static void TDraw::DrawRect(HDC hdc, RECT &rect, LOGPEN &logpen);
	static void TDraw::DrawPickSquare(HDC hdc, POINT pt);
	static void DrawLine(HDC hdc, POINT ptFirstPos, POINT ptSecondPos);
	static void TDraw::DrawLine(HDC hdc, TLine Line);
	static void TDraw::DrawFramePoint(HDC hdc, TFramePoint FramePoint, TConfiguration *Config);
	static void TDraw::DrawRealLine(HDC hdc, TRealLine RealLine, TConfiguration *Config);
	static void TDraw::DrawCross(HDC hdc, POINT pt, int size, LOGPEN Style);
	static void TDraw::DrawArrow(HDC hdc, POINT ptBegin, POINT ptEnd, int length, int width);
	static void TDraw::DrawAxes(HDC hdc, int Ox, int Oy, COLORREF crColor);
	static void TDraw::FillRect(HDC hdc, RECT *rect, COLORREF crColor);
	static void TDraw::DrawGrid(HDC hdc, RECT rect, TConfiguration *Config);

	static void TDraw::DrawSection(HDC hdc, int x1, int y1, int x2, int y2, int d, int angleDEG);
	static double TDraw::GetAngleFromPointReal(DPOINT ptO, DPOINT pt);
	static double TDraw::GetAngleFromPointScreen(POINT pt0, POINT pt);
	static void TDraw::ClientPosToScreen(HWND hWnd, POINT *pt);
};

