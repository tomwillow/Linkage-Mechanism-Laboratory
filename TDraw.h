#pragma once
#include <vector>
#include <Windows.h>

#include "DPOINT.h"

class TConfiguration;
class TElement;
class TBar;
class TLine;
class TRealLine;
class TSlideway;
class TSlider;
class TFramePoint;
class TConstraintCoincide;
class TConstraintColinear;
class TPolylineBar;
class TDraw
{
public:
	TDraw();
	~TDraw();

	static void TDraw::MoveByDelta(POINT apt[], int apt_num, long dx, long dy);
	static void TDraw::MoveByDelta(std::vector<POINT> &vecpt, long dx, long dy);
	static void TDraw::Move(POINT apt[], int apt_num, double angle, double dist);
	static void TDraw::Rotate(POINT apt[], int apt_num, int Ox, int Oy, double theta);
	static void TDraw::MirrorX(POINT apt[], int apt_num, int Oy);
	static void TDraw::GetBoundingBox(POINT apt[], int apt_num, RECT *rect, bool YPlusIsUP);
	static void TDraw::GetBoundingBox(std::vector<DPOINT> &vecdpt, RECT *rect);
	static void TDraw::GetBoundingBox(std::vector<POINT> &vecpt, RECT *rect);
	static double TDraw::Distance(DPOINT pt1, DPOINT pt2);
	static double TDraw::Distance(POINT pt1, POINT pt2);
	static double TDraw::DistanceScreen(const DPOINT &dpt1, const DPOINT &dpt2, const TConfiguration *pConfig);
	static DPOINT TDraw::GetAbsolute(const DPOINT &dpt, const DPOINT &Org, double angle);
	static void TDraw::GetAbsoluteReal(std::vector<DPOINT> &vecdptResult, const std::vector<DPOINT> &vecdpt, const DPOINT &Org, double angle);
	static void TDraw::GetAbsoluteScreen(std::vector<POINT> &vecptResult, const std::vector<DPOINT> &vecdpt, const DPOINT &Org, double angle, const TConfiguration *pConfig);
	static DPOINT TDraw::GetRelative(const DPOINT &dpt, const DPOINT &Org, double angle);
	static bool TDraw::GetIntersection(const POINT &ptL1Begin, const POINT &ptL1End, const POINT &ptL2Begin, const  POINT &ptL2End, POINT &ptIntersection);
	static bool TDraw::GetIntersection(const DPOINT &dptL1Begin, const DPOINT &dptL1End, const DPOINT &dptL2Begin, const  DPOINT &dptL2End, DPOINT &dptIntersection);

	static RECT TDraw::GetMarginRect(RECT rect, int margin);
	static void TDraw::SetMarginRect(RECT *rect, int margin);
	static POINT TDraw::GetCenter(const POINT &pt1,const POINT &pt2);
	static DPOINT TDraw::GetCenter(const DPOINT &pt1,const DPOINT &pt2);
	static bool TDraw::ShowConstraintCoincideDotLine(TElement *element, const TConfiguration *pConfig);
	static bool TDraw::ShowConstraintColinearDotLine(const TConstraintColinear *pColinear, POINT &ptCenter1, POINT &ptCenter2, const TConfiguration *pConfig);

	static COLORREF TDraw::GetBrighterColor(COLORREF cr);

	static void TDraw::DrawElement(HDC hdc, TElement *Element, TConfiguration *pConfig);
	static void TDraw::DrawFramePoint(HDC hdc, TFramePoint *pFramePoint, TConfiguration *Config);
	static void TDraw::DrawBar(HDC hdc, TBar *Bar, TConfiguration *Config);
	static void TDraw::DrawBarSimple(HDC hdc, TBar *Bar, TConfiguration *Config);
	static void TDraw::DrawBarTranslucent(HDC hdc, TBar *pBar, TConfiguration *pConfig);
	static void TDraw::DrawBarTranslucent(HDC hdc, POINT &ptBegin, POINT &ptEnd, double angle, unsigned char alpha, LOGPEN logpen, TConfiguration *pConfig);
	static void TDraw::DrawPolylineBar(HDC hdc, TPolylineBar *PolylineBar, TConfiguration *pConfig);
	static void TDraw::DrawPolylineBarSimple(HDC hdc, TPolylineBar *pPolylineBar, TConfiguration *pConfig);
	static void TDraw::DrawPolylineBarTranslucent(HDC hdc, TPolylineBar *pPolylineBar, TConfiguration *pConfig);
	static void TDraw::DrawRealLine(HDC hdc, TRealLine &RealLine, TConfiguration *Config);
	static void TDraw::DrawRealLine(HDC hdc, DPOINT ptBegin, DPOINT ptEnd, LOGPEN logpen,const TConfiguration *Config);
	static void TDraw::DrawSlideway(HDC hdc, TSlideway *Slideway, TConfiguration *Config);
	static void TDraw::DrawSlider(HDC hdc, TSlider *pSlider, TConfiguration *pConfig);
	static void TDraw::DrawConstraintCoincide(HDC hdc, DPOINT dpt0, DPOINT dpt1,const LOGPEN &logpen, const TConfiguration *pConfig);
	static void TDraw::DrawConstraintCoincide(HDC hdc, TConstraintCoincide *pCoincide, TConfiguration *pConfig);
	static void TDraw::DrawConstraintColinear_inner(HDC hdc, const POINT &pt1, const POINT &pt2,LOGPEN logpen, const TConfiguration *pConfig);
	static void TDraw::DrawConstraintColinear(HDC hdc, TConstraintColinear *pColinear, TConfiguration *pConfig);

	static void TDraw::DrawArc(HDC hdc, const POINT &pt, int r, const POINT &pt1, const POINT &pt2, bool bAlwaysDrawAngleBetween);
	static void TDraw::DrawPie(HDC hdc, const POINT &pt, int r, const POINT &pt1, const POINT &pt2,bool bAlwaysDrawAngleBetween);
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

	static void TDraw::CalcBarLineEndpoint(POINT &ptBegin, POINT &ptEnd, int distBegin, int distEnd);
	static void TDraw::CalcPolylineBarRgn(HRGN &hRgn, const std::vector<DPOINT> &vecDpt, DPOINT dpt, double angle, const TConfiguration *pConfig);
	static void TDraw::CalcPolylineBarRgn(HRGN &hRgn, TPolylineBar *pPolylineBar, const TConfiguration *pConfig);
	static void TDraw::CalcBarRectCoor(POINT ptResult[4], const POINT &ptBegin, const POINT &ptEnd, double angle, int width);
	static void TDraw::CalcSliderRectCoor(POINT aptResult[4], const POINT &pt, double angle, const TConfiguration *pConfig);
	static void TDraw::FillRect(HDC hdc, RECT *rect, COLORREF crColor);
	static void TDraw::DrawGrid(HDC hdc, RECT rect, TConfiguration *Config, COLORREF crGridBig, COLORREF crGridSmall);
	static void TDraw::DrawTips(HDC hdc, POINT &ptMouse, const TCHAR text[], TConfiguration *pConfig);
	static void TDraw::DrawSystemFontText(HDC hdc,const TCHAR text[], RECT &rect, COLORREF color, UINT format);
	static void TDraw::DrawTextAdvance(HDC hdc,const TCHAR text[], RECT *rect, long FontSize, int FontWeight, unsigned long color, const TCHAR FontName[], UINT format);

	static void TDraw::DrawSection(HDC hdc, int x1, int y1, int x2, int y2, int d, double angleDEG);
	static void TDraw::DrawSection(HDC hdc, POINT apt[], int apt_num, int d, double angleDEG);
	static double TDraw::GetAngleFromPointReal(DPOINT ptO, DPOINT pt);
	static double TDraw::GetAngleBetweenPointReal(const DPOINT &pt1, const DPOINT &ptO, const DPOINT &pt2);
	static double TDraw::GetAngleBetweenPointScreen(const POINT &pt1, const POINT &ptO, const POINT &pt2);
	static double TDraw::GetAngleFromPointScreen(POINT pt0, POINT pt);
	static void TDraw::ClientPosToScreen(HWND hWnd, POINT *pt);
	static POINT TDraw::DPOINT2POINT(DPOINT &dpt, double x_min, double x_max, double y_min, double y_max, RECT &rect);
	static DPOINT TDraw::POINT2DPOINT(POINT &pt, double x_min, double x_max, double y_min, double y_max, RECT &rect);

	//拾取系列
	static bool TDraw::PointInPolylineBar(POINT ptPos, TPolylineBar *pPolylineBar,const TConfiguration *pConfig);
	static bool TDraw::PointInFramePoint(POINT ptFramePoint, POINT pt, const TConfiguration *pConfig);
	static bool TDraw::PointInRgn(POINT *ptRgn, int RgnCount, POINT pt);
	static bool TDraw::PointInRealLine(POINT ptPos, TRealLine *pRealLine,const TConfiguration *pConfig);
	static bool TDraw::PointInSlider(POINT ptPos, TSlider *pSlider, const TConfiguration *pConfig);
	static bool TDraw::PointInRealLine(const POINT &ptPos,const POINT &ptBegin,const POINT &ptEnd, const TConfiguration *pConfig);
	static bool TDraw::PointInRealLine(const POINT &ptPos,const DPOINT &dptBegin,const DPOINT &dptEnd, const TConfiguration *pConfig);
	static int TDraw::PointInRealLineOrExtension(const DPOINT &dptPos, DPOINT &dptIntersection, const DPOINT dptBegin, const DPOINT dptEnd,const TConfiguration *pConfig);
	static bool TDraw::PickConstraintColinear(POINT ptPos, TConstraintColinear *pColinear,const TConfiguration *pConfig);
	static bool TDraw::PickConstraintCoincide(POINT ptPos, TElement *element,const TConfiguration *pConfig);

	//透明处理
	static void TDraw::StartTranslucent(HDC &hBitmapDC, HBITMAP &hBitmap, VOID *&pvBits, long left, long top, long width, long height, bool bNeedDrawBlack);
	static void TDraw::StartTranslucent(HDC &hBitmapDC, HBITMAP &hBitmap, VOID *&pvBits, const RECT &rect, bool bNeedDrawBlack);
	static void TDraw::EndTranslucent(HDC &hdc, HDC &hBitmapDC, HBITMAP &hBitmap, VOID *&pvBits, long left, long top, long width, long height, BYTE alpha, bool bNeedDrawBlack);
	static void TDraw::EndTranslucent(HDC &hdc, HDC &hBitmapDC, HBITMAP &hBitmap, VOID *&pvBits, const RECT &rect, BYTE alpha, bool bNeedDrawBlack);
};

