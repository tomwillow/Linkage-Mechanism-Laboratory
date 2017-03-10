#pragma once
#include "MyMath.h"
#include <Windows.h>

#include "TBar.h"
#include "TLine.h"
#include "TRealLine.h"
#include "TSlideway.h"
#include "TConstraintCoincide.h"

#include "TConfiguration.h"

class TSlider;
class TFramePoint;
class TDraw
{
#define FRAMEPOINT_R 6//节点圆半径
#define FRAMEPOINT_H 20//高-圆心到地线距离
#define FRAMEPOINT_B 30//底边长
#define FRAMEPOINT_SECTION_H 10//剖面线区域高度
#define FRAMEPOINT_ANGLE 60.0 / 180.0 * M_PI//三角形角度
#define SLIDER_B 45
#define SLIDER_H 30
public:
	TDraw();
	~TDraw();

	static void TDraw::Move(POINT apt[], int apt_num, double angle, double dist);
	static void TDraw::Rotate(POINT apt[], int apt_num, int Ox, int Oy, double theta);
	static void TDraw::MirrorX(POINT apt[], int apt_num, int Oy);
	static void TDraw::GetBoundingBox(POINT apt[], int apt_num, RECT *rect, bool YPlusIsUP);
	static double TDraw::Distance(POINT pt1, POINT pt2);
	static double TDraw::DistanceScreen(const DPOINT &dpt1, const DPOINT &dpt2, TConfiguration *pConfig);
	static DPOINT TDraw::GetAbsolute(const DPOINT &dpt, const DPOINT &Org, double angle);
	static DPOINT TDraw::GetRelative(const DPOINT &dpt, const DPOINT &Org, double angle);
	static bool TDraw::GetIntersection(const POINT &ptL1Begin, const POINT &ptL1End, const POINT &ptL2Begin, const  POINT &ptL2End, POINT &ptIntersection);
	static bool TDraw::GetIntersection(const DPOINT &dptL1Begin, const DPOINT &dptL1End, const DPOINT &dptL2Begin, const  DPOINT &dptL2End, DPOINT &dptIntersection);

	static bool TDraw::PointInFramePoint(POINT ptFramePoint, POINT pt);
	static bool TDraw::PointInRgn(POINT *ptRgn, int RgnCount, POINT pt);
	static RECT TDraw::GetMarginRect(RECT rect, int margin);
	static void TDraw::GetMarginRect(RECT *rect, int margin);
	static POINT TDraw::GetCenter(POINT &pt1, POINT &pt2);
	static bool TDraw::ShowConstraintCoincideDotLine(TElement *element, TConfiguration *pConfig);

	static void TDraw::DrawElement(HDC hdc, TElement *Element, TConfiguration *pConfig);
	static void TDraw::DrawFramePoint(HDC hdc, TFramePoint *pFramePoint, TConfiguration *Config);
	static void TDraw::DrawBar(HDC hdc, TBar *Bar, TConfiguration *Config);
	static void TDraw::DrawRealLine(HDC hdc, TRealLine &RealLine, TConfiguration *Config);
	static void TDraw::DrawRealLine(HDC hdc, DPOINT ptBegin, DPOINT ptEnd, LOGPEN logpen, TConfiguration *Config);
	static void TDraw::DrawSlideway(HDC hdc, TSlideway *Slideway, TConfiguration *Config);
	static void TDraw::DrawSlider(HDC hdc, TSlider *pSlider, TConfiguration *pConfig);
	static void TDraw::DrawConstraintCoincide(HDC hdc, TConstraintCoincide *pCoincide, TConfiguration *pConfig);

	static void TDraw::DrawPie(HDC hdc, const POINT &pt, int r, const POINT &pt1, const POINT &pt2, const LOGPEN &logpen, const COLORREF &crColor);
	static void TDraw::DrawRect(HDC hdc, RECT &rect, LOGPEN &logpen);
	static void TDraw::DrawPickSquare(HDC hdc, POINT pt);
	static void TDraw::DrawLine(HDC hdc, POINT ptFirstPos, POINT ptSecondPos);
	static void TDraw::DrawLine(HDC hdc, TLine Line);
	static void TDraw::DrawPolyline(HDC hdc, const POINT *apt, int count, LOGPEN &logpen);

	static void TDraw::DrawCircle(HDC hdc, POINT pt, int r);
	static void TDraw::DrawCircle(HDC hdc, POINT pt, int r, LOGPEN logpen);
	static void TDraw::DrawCross(HDC hdc, POINT pt, int size, LOGPEN Style);
	static void TDraw::DrawArrow(HDC hdc, POINT ptBegin, POINT ptEnd, int length, int width);
	static void TDraw::DrawAxes(HDC hdc, int Ox, int Oy, COLORREF crColor);
	static void TDraw::CalcSliderRectCoor(POINT aptResult[4], const POINT &pt, double angle);
	static void TDraw::FillRect(HDC hdc, RECT *rect, COLORREF crColor);
	static void TDraw::DrawGrid(HDC hdc, RECT rect, TConfiguration *Config);
	static void TDraw::DrawTips(HDC hdc, POINT &ptMouse, const TCHAR text[], TConfiguration *pConfig);
	static void TDraw::DrawSystemFontText(HDC hdc,const TCHAR text[], RECT &rect, COLORREF color, UINT format);
	static void TDraw::DrawTextAdvance(HDC hdc,const TCHAR text[], RECT *rect, long FontSize, int FontWeight, unsigned long color, const TCHAR FontName[], UINT format);

	static void TDraw::DrawSection(HDC hdc, int x1, int y1, int x2, int y2, int d, int angleDEG);
	static void TDraw::DrawSection(HDC hdc, POINT apt[], int apt_num, int d, int angleDEG);
	static double TDraw::GetAngleFromPointReal(DPOINT ptO, DPOINT pt);
	static double TDraw::GetAngleBetweenPointReal(const DPOINT &pt1,const DPOINT &ptO,const DPOINT &pt2);
	static double TDraw::GetAngleFromPointScreen(POINT pt0, POINT pt);
	static void TDraw::ClientPosToScreen(HWND hWnd, POINT *pt);
	static POINT TDraw::DPOINT2POINT(DPOINT &dpt, double x_min, double x_max, double y_min, double y_max, RECT &rect);
	static DPOINT TDraw::POINT2DPOINT(POINT &pt, double x_min, double x_max, double y_min, double y_max, RECT &rect);
	static bool TDraw::PointInRealLine(POINT ptPos, TRealLine *pRealLine, TConfiguration *pConfig);
	static bool TDraw::PointInSlider(POINT ptPos, TSlider *pSlider, TConfiguration *pConfig);
	static bool TDraw::PointInRealLine(POINT &ptPos, DPOINT &dptBegin, DPOINT &dptEnd, TConfiguration *pConfig);
	static int TDraw::PointInRealLineOrExtension(const POINT &ptPos, DPOINT &ptIntersection, const TRealLine *pRealLine, TConfiguration *pConfig);
};

