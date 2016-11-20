#pragma once
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>

#include "TBar.h"
#include "TLine.h"
#include "TRealLine.h"
#include "TConfiguration.h"

class TFramePoint;
class TDraw
{
#define FRAMEPOINT_R 6//半径
#define FRAMEPOINT_H 20//高-圆心到地线距离
#define FRAMEPOINT_B 30//底边长
#define FRAMEPOINT_SECTION_H 10//剖面线区域高度
#define FRAMEPOINT_ANGLE 60.0 / 180.0 * M_PI//三角形角度
public:
	TDraw();
	~TDraw();

	static void TDraw::Move(POINT apt[], int apt_num, double angle, double dist);
	static void Rotate(POINT apt[], int apt_num, int Ox, int Oy, double theta);
	static void TDraw::MirrorX(POINT apt[], int apt_num, int Oy);
	static double TDraw::Distance(POINT pt1, POINT pt2);
	static bool TDraw::PointInFramePoint(POINT ptFramePoint, POINT pt);
	static bool TDraw::PointInRgn(POINT *ptRgn, int RgnCount, POINT pt);
	static RECT TDraw::GetMarginRect(RECT rect, int margin);
	static void TDraw::GetMarginRect(RECT *rect, int margin);

	static void TDraw::DrawElement(HDC hdc, TElement *Element, TConfiguration *pConfig);
	static void TDraw::DrawFramePoint(HDC hdc, DPOINT dpt, LOGPEN logpen, TConfiguration *Config);
	static void TDraw::DrawBar(HDC hdc, DPOINT dptBegin, DPOINT dptEnd, LOGPEN logpen, TConfiguration *Config);
	static void TDraw::DrawRealLine(HDC hdc, TRealLine &RealLine, TConfiguration *Config);
	static void TDraw::DrawRealLine(HDC hdc, DPOINT ptBegin, DPOINT ptEnd, LOGPEN logpen, TConfiguration *Config);

	static void TDraw::DrawRect(HDC hdc, RECT &rect, LOGPEN &logpen);
	static void TDraw::DrawPickSquare(HDC hdc, POINT pt);
	static void DrawLine(HDC hdc, POINT ptFirstPos, POINT ptSecondPos);
	static void TDraw::DrawLine(HDC hdc, TLine Line);

	static void TDraw::DrawCircle(HDC hdc, POINT pt, int r);
	static void TDraw::DrawCross(HDC hdc, POINT pt, int size, LOGPEN Style);
	static void TDraw::DrawArrow(HDC hdc, POINT ptBegin, POINT ptEnd, int length, int width);
	static void TDraw::DrawAxes(HDC hdc, int Ox, int Oy, COLORREF crColor);
	static void TDraw::FillRect(HDC hdc, RECT *rect, COLORREF crColor);
	static void TDraw::DrawGrid(HDC hdc, RECT rect, TConfiguration *Config);
	static void TDraw::DrawSystemFontText(HDC hdc, TCHAR text[], RECT &rect, COLORREF color, UINT format);
	static void TDraw::DrawTextAdvance(HDC hdc, TCHAR text[], RECT *rect, long FontSize, int FontWeight, unsigned long color, const TCHAR FontName[], UINT format);

	static void TDraw::DrawSection(HDC hdc, int x1, int y1, int x2, int y2, int d, int angleDEG);
	static double TDraw::GetAngleFromPointReal(DPOINT ptO, DPOINT pt);
	static double TDraw::GetAngleFromPointScreen(POINT pt0, POINT pt);
	static void TDraw::ClientPosToScreen(HWND hWnd, POINT *pt);
};

