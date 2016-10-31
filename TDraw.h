#pragma once
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "TLine.h"
#include "TRealLine.h"
#include "TConfiguration.h"

class TDraw
{
public:
	TDraw();
	~TDraw();

	static void Rotate(POINT apt[], int apt_num, int Ox, int Oy, double theta);
	static void TDraw::MirrorX(POINT apt[], int apt_num, int Oy);
	static void DrawLine(HDC hdc, POINT ptFirstPos, POINT ptSecondPos);
	static void TDraw::DrawLine(HDC hdc, TLine Line);
	static void TDraw::DrawRealLine(HDC hdc, TRealLine RealLine, TConfiguration *Config);
	static void DrawCross(HDC hdc, POINT pt, int size, TLine Style);
	static void DrawArrow(HDC hdc, POINT ptBegin, POINT ptEnd, int length, int width);
	static void TDraw::DrawAxes(HDC hdc, int Ox, int Oy, COLORREF crColor);
	static void TDraw::FillRect(HDC hdc, RECT *rect, COLORREF crColor);
	static void TDraw::DrawGrid(HDC hdc, RECT rect, TConfiguration *Config);
	static void TDraw::Move(POINT apt[], int apt_num, double angle, double dist);
	static double TDraw::GetAngleFromPointReal(DPOINT ptO, DPOINT pt);
	static double TDraw::GetAngleFromPointScreen(POINT pt0, POINT pt);
};

