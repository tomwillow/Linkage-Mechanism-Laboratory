#pragma once
#include "TDraw.h"

#include "TFramePoint.h"

TDraw::TDraw()
{
}


TDraw::~TDraw()
{
}

//画直线
void TDraw::DrawLine(HDC hdc, POINT ptFirstPos, POINT ptSecondPos)
{
	::MoveToEx(hdc, ptFirstPos.x, ptFirstPos.y, NULL);
	::LineTo(hdc, ptSecondPos.x, ptSecondPos.y);
}

void TDraw::DrawLine(HDC hdc, TLine Line)
{
	HPEN hPen;
	hPen = ::CreatePenIndirect(&Line.logpenStyle);
	::SelectObject(hdc, hPen);
	DrawLine(hdc, Line.ptBegin, Line.ptEnd);

	::DeleteObject(hPen);
}

void TDraw::DrawElement(HDC hdc, TElement *Element, TConfiguration *pConfig)
{
	switch (Element->eType)
	{
	case ELEMENT_REALLINE:
		DrawRealLine(hdc, ((TRealLine *)Element)->ptBegin, ((TRealLine *)Element)->ptEnd, ((TRealLine *)Element)->logpenStyleShow, pConfig);
		break;
	case ELEMENT_FRAMEPOINT:
		DrawFramePoint(hdc, ((TFramePoint *)Element)->dpt, ((TFramePoint *)Element)->logpenStyleShow, pConfig);
		break;
	case ELEMENT_BAR:
		DrawBar(hdc, ((TBar *)Element)->ptBegin, ((TBar *)Element)->ptEnd, ((TBar *)Element)->logpenStyleShow, pConfig);
		break;
	case CONSTRAINT_COINCIDE:
		TConstraintCoincide *temp = (TConstraintCoincide *)Element;
		POINT p1, p2;
		//得到p1,p2
		//if (temp->Element1PointIndex!=2)

		//比较p1,p2，距离大则画虚线
		break;
	}
}

void TDraw::DrawBar(HDC hdc, DPOINT dptBegin, DPOINT dptEnd, LOGPEN logpen, TConfiguration *pConfig)
{
	HPEN hPen;
	HBRUSH hBrush;
	hPen = ::CreatePenIndirect(&logpen);
	hBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
	::SelectObject(hdc, hPen);
	::SelectObject(hdc, hBrush);

	POINT ptBegin, ptEnd;
	ptBegin = pConfig->RealToScreen(dptBegin);
	ptEnd = pConfig->RealToScreen(dptEnd);
	DrawCircle(hdc, ptBegin, FRAMEPOINT_R);
	DrawCircle(hdc, ptEnd, FRAMEPOINT_R);

	POINT p1, p2;
	double theta1 = GetAngleFromPointScreen(ptBegin, ptEnd);
	double theta2 = GetAngleFromPointScreen(ptEnd, ptBegin);
	p1 = { ptBegin.x + FRAMEPOINT_R*cos(theta1), ptBegin.y - FRAMEPOINT_R*sin(theta1) };
	p2 = { ptEnd.x + FRAMEPOINT_R*cos(theta2), ptEnd.y - FRAMEPOINT_R*sin(theta2) };
	DrawLine(hdc, p1, p2);

	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

void TDraw::DrawRealLine(HDC hdc,TRealLine &RealLine, TConfiguration *Config)
{
	DrawRealLine(hdc, RealLine.ptBegin, RealLine.ptEnd, RealLine.logpenStyleShow, Config);
}

void TDraw::DrawRealLine(HDC hdc, DPOINT ptBegin,DPOINT ptEnd,LOGPEN logpen,TConfiguration *Config)
{
	HPEN hPen;

	hPen = ::CreatePenIndirect(&logpen);
	::SelectObject(hdc, hPen);
	DrawLine(hdc, Config->RealToScreen(ptBegin), Config->RealToScreen(ptEnd));

	::DeleteObject(hPen);
}

bool TDraw::PointInRgn(POINT *ptRgn,int RgnCount, POINT pt)
{
	//double sum=0,angle,p1, p2;
	//double angledeg = 0;
	//for (int i = 0; i < RgnCount-1; i++)
	//{
	//	p1 = GetAngleFromPointScreen(pt, ptRgn[i]);
	//	p2 = GetAngleFromPointScreen(pt, ptRgn[i + 1]);
	//	angle = p2 - p1;
	//	if (angle > M_PI) angle = 2 * M_PI - angle;
	//	if (angle < -M_PI) angle = 2 * M_PI + angle;
	//	angledeg = angle / M_PI * 180;
	//	sum += angle;
	//}
	//p1 = GetAngleFromPointScreen(pt, ptRgn[RgnCount - 1]);
	//p2 = GetAngleFromPointScreen(pt, ptRgn[0]);
	//angle = p2 - p1;
	//if (angle > M_PI) angle = 2 * M_PI - angle;
	//if (angle < -M_PI) angle = 2 * M_PI + angle;
	//sum += angle;

	//if (abs(sum) < 0.001)//sum==0
	//	return false;//在多边形外
	//else
	//	return true;
	int   i, j = RgnCount - 1;
	bool  oddNodes = false;

	for (i = 0; i<RgnCount; i++) 
	{
		if ((ptRgn[i].y< pt.y && ptRgn[j].y >= pt.y
			|| ptRgn[j].y<pt.y && ptRgn[i].y >= pt.y)
			&& (ptRgn[i].x <= pt.x || ptRgn[j].x <= pt.x)) 
		{
			oddNodes ^= (ptRgn[i].x + (pt.y - ptRgn[i].y) / (ptRgn[j].y - ptRgn[i].y)*(ptRgn[j].x - ptRgn[i].x)<pt.x);
		}
		j = i;
	}
	return oddNodes;
}

bool TDraw::PointInFramePoint(POINT ptFramePoint, POINT pt)
{
	POINT FramePointRgn[6];
	FramePointRgn[0] = { ptFramePoint.x - FRAMEPOINT_R, ptFramePoint.y - FRAMEPOINT_R };
	FramePointRgn[1] = { ptFramePoint.x + FRAMEPOINT_R, ptFramePoint.y - FRAMEPOINT_R };
	FramePointRgn[5] = { ptFramePoint.x-FRAMEPOINT_B/2, ptFramePoint.y+FRAMEPOINT_H };
	FramePointRgn[2] = { ptFramePoint.x + FRAMEPOINT_B / 2, ptFramePoint.y + FRAMEPOINT_H };
	FramePointRgn[4] = { ptFramePoint.x - FRAMEPOINT_B / 2, ptFramePoint.y+FRAMEPOINT_H+FRAMEPOINT_SECTION_H };
	FramePointRgn[3] = { ptFramePoint.x + FRAMEPOINT_B / 2, ptFramePoint.y + FRAMEPOINT_H + FRAMEPOINT_SECTION_H };
	
	return PointInRgn(FramePointRgn, 6, pt);
}

//画圆 没有样式
void TDraw::DrawCircle(HDC hdc, POINT pt, int r)
{
	::Ellipse(hdc, pt.x - r, pt.y - r, pt.x + r, pt.y + r);
}

//画机架点
void TDraw::DrawFramePoint(HDC hdc,DPOINT dpt, LOGPEN logpen, TConfiguration *Config)
{
	HPEN hPen;
	HBRUSH hBrush;
	hPen = ::CreatePenIndirect(&logpen);
	hBrush = (HBRUSH)::GetStockObject(NULL_BRUSH);
	::SelectObject(hdc, hPen);
	::SelectObject(hdc, hBrush);

	POINT ptO = Config->RealToScreen(dpt);
	//画圆
	::Ellipse(hdc, ptO.x - FRAMEPOINT_R, ptO.y - FRAMEPOINT_R, ptO.x + FRAMEPOINT_R, ptO.y + FRAMEPOINT_R);

	//设置圆下方的两个点
	POINT ptA1, ptB1;
	ptA1 = { ptO.x - FRAMEPOINT_R*sin(FRAMEPOINT_ANGLE / 2), ptO.y + FRAMEPOINT_R*cos(FRAMEPOINT_ANGLE / 2) };
	ptB1 = { ptO.x + FRAMEPOINT_R*sin(FRAMEPOINT_ANGLE / 2), ptA1.y };

	//设置三角形的底边两个点
	POINT ptA2, ptB2;
	ptA2 = { ptO.x - FRAMEPOINT_H*tan(FRAMEPOINT_ANGLE / 2), ptO.y + FRAMEPOINT_H };
	ptB2 = { ptO.x + FRAMEPOINT_H*tan(FRAMEPOINT_ANGLE / 2), ptA2.y };
	DrawLine(hdc, ptA1, ptA2);
	DrawLine(hdc, ptB1, ptB2);

	//设置地面线
	POINT ptH1, ptH2;
	ptH1 = { ptO.x - FRAMEPOINT_B / 2, ptO.y + FRAMEPOINT_H };
	ptH2 = { ptO.x + FRAMEPOINT_B / 2, ptH1.y };
	DrawLine(hdc, ptH1, ptH2);

	//画剖面线
	DrawSection(hdc, ptH1.x, ptH1.y, ptH2.x, ptH1.y + FRAMEPOINT_SECTION_H, 6, 45);

	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

//画剖面线：d为垂直间距，angleDEG为角度
void TDraw::DrawSection(HDC hdc, int x1,int y1,int x2,int y2,int d, int angleDEG)
{
	double angle = angleDEG / 180.0*M_PI;
	int dx= d / sin(angle);
	int h = y2-y1;
	POINT pt1,pt2;
	pt1 = { x1 + dx, y1 };
	pt2 = { pt1.x - h / tan(angle), y2 };

	HRGN hRgn;
	hRgn = ::CreateRectRgn(x1,y1,x2,y2);
	::SelectClipRgn(hdc, hRgn);

	while (pt2.x < x2)
	{
		DrawLine(hdc, pt1, pt2);
		pt1.x += dx;
		pt2.x += dx;
	}
	
	::SelectClipRgn(hdc, NULL);
	::DeleteObject(hRgn);
}

//将点集以指定原点旋转theta，传入点以Y方向向上为正
void TDraw::Rotate(POINT apt[], int apt_num, int Ox, int Oy, double theta)
{
	int x, y;
	for (int i = 0; i < apt_num; i++)
	{
		x = apt[i].x;
		y = apt[i].y;

		//先平移到原点
		x -= Ox;
		y -= Oy;

		//旋转后再平移回原位置
		apt[i].x =(LONG)( x*cos(theta) - y*sin(theta) + Ox);
		apt[i].y =(LONG)( x*sin(theta) + y*cos(theta) + Oy);
	}
}

//返回pt相对于原点ptO的角度，传入点以Y方向向上为正
double GetAngleFromPoint(POINT ptO, POINT pt)
{
	double angle=0;
	if (((pt.x - ptO.x) > 0) && ((pt.y - ptO.y) >= 0))//第1象限[0,PI/2)
		angle = atan(double(pt.y - ptO.y) / (pt.x - ptO.x));
	if (((pt.x - ptO.x) <= 0) && ((pt.y - ptO.y) > 0)) //第2象限[PI/2,PI)
		angle = M_PI / 2 + atan(double(ptO.x - pt.x) / (pt.y - ptO.y));
	if (((pt.x - ptO.x) < 0) && ((pt.y - ptO.y) <= 0))//第3象限[PI,PI*3/2)
		angle = M_PI + atan(double(ptO.y - pt.y) / (ptO.x - pt.x));
	if (((pt.x - ptO.x) >= 0) && ((pt.y - ptO.y) < 0))//第4象限[PI*3/2,2*PI)
		angle = 3.0 / 2.0 * M_PI + atan(double(pt.x - ptO.x) / (ptO.y - pt.y));
	return angle;
}

//返回pt相对于原点ptO的角度，传入点以Y方向向上为正
double TDraw::GetAngleFromPointReal(DPOINT ptO, DPOINT pt)
{
	double angle=0;
	if (((pt.x - ptO.x) >= 0) && ((pt.y - ptO.y) >= 0))//第1象限[0,PI/2)
		angle = atan(double(pt.y - ptO.y) / (pt.x - ptO.x));
	if (((pt.x - ptO.x) <= 0) && ((pt.y - ptO.y) > 0)) //第2象限[PI/2,PI)
		angle = M_PI / 2 + atan(double(ptO.x - pt.x) / (pt.y - ptO.y));
	if (((pt.x - ptO.x) < 0) && ((pt.y - ptO.y) <= 0))//第3象限[PI,PI*3/2)
		angle = M_PI + atan(double(ptO.y - pt.y) / (ptO.x - pt.x));
	if (((pt.x - ptO.x) >= 0) && ((pt.y - ptO.y) < 0))//第4象限[PI*3/2,2*PI)
		angle = 3.0 / 2.0 * M_PI + atan(double(pt.x - ptO.x) / (ptO.y - pt.y));
	return angle;
}

//以屏幕点返回角度，传入点以Y方向向下为正
double TDraw::GetAngleFromPointScreen(POINT pt0, POINT pt)
{
	pt0.y = -pt0.y;
	pt.y = -pt.y;
	return GetAngleFromPoint(pt0, pt);
}

//以X方向为对称轴镜像
void TDraw::MirrorX(POINT apt[], int apt_num, int Oy)
{
	for (int i = 0; i < apt_num; i++)
	{
		apt[i].y = Oy - (apt[i].y - Oy);
	}
}

//将点集apt向angle方向移动dist距离，apt为屏幕坐标
void TDraw::Move(POINT apt[],int apt_num, double angle, double dist)
{
	for (int i = 0; i < apt_num; i++)
	{
		apt[i].y = -apt[i].y;
		apt[i].x += dist*cos(angle);
		apt[i].y += dist*sin(angle);
		apt[i].y = -apt[i].y;
	}
}

//画箭头：没有样式
//始末点均为屏幕坐标
void TDraw::DrawArrow(HDC hdc, POINT ptBegin, POINT ptEnd, int length, int width)
{
	POINT apt[3];
	int aptNum= 3;
	//先计算指向终点，方向为X轴向箭头坐标，再旋转theta度。
	apt[0] = ptEnd;
	apt[1].x = ptEnd.x - length;
	apt[1].y = ptEnd.y + width / 2;
	apt[2].x = ptEnd.x - length;
	apt[2].y = ptEnd.y - width / 2;

	//由于获取角及旋转函数使用笛卡尔坐标系，故先翻转屏幕坐标，计算完后再翻转回去。
	TDraw::MirrorX(&ptBegin, 1, 0);
	TDraw::MirrorX(&ptEnd, 1, 0);
	TDraw::MirrorX(apt, aptNum, 0);

	TDraw::Rotate(apt, aptNum, ptEnd.x, ptEnd.y, GetAngleFromPoint(ptBegin, ptEnd));

	TDraw::MirrorX(&ptBegin, 1, 0);
	TDraw::MirrorX(&ptEnd, 1, 0);
	TDraw::MirrorX(apt, aptNum, 0);

	Polygon(hdc, apt, aptNum);
}

//画坐标轴：附带样式
void TDraw::DrawAxes(HDC hdc, int Ox, int Oy, COLORREF crColor)
{
	int AxesLen = 40;
	POINT ptO;
	POINT ptXEnd, ptYEnd;
	ptO.x = Ox; ptO.y = Oy;
	ptXEnd.x = Ox + AxesLen; ptXEnd.y = Oy;
	ptYEnd.x = Ox; ptYEnd.y = Oy - AxesLen;

	HBRUSH hBrush = CreateSolidBrush(crColor);
	HPEN hPen = CreatePen(PS_SOLID, 1, crColor);
	SelectObject(hdc, hBrush);
	SelectObject(hdc, hPen);
	TDraw::DrawArrow(hdc, ptO, ptXEnd, 20, 6);
	TDraw::DrawLine(hdc, ptO, ptXEnd);
	TDraw::DrawArrow(hdc, ptO, ptYEnd, 20, 6);
	TDraw::DrawLine(hdc, ptO, ptYEnd);
	DeleteObject(hBrush);
	DeleteObject(hPen);
}

//画X形
void TDraw::DrawCross(HDC hdc, POINT pt, int size, LOGPEN Style)
{
	HPEN hPen;

	hPen = CreatePenIndirect(&Style);
	SelectObject(hdc, hPen);

	::MoveToEx(hdc, pt.x - size / 2, pt.y - size / 2, NULL);
	::LineTo(hdc, pt.x + size / 2, pt.y + size / 2);
	::MoveToEx(hdc, pt.x + size / 2, pt.y - size / 2, NULL);
	::LineTo(hdc, pt.x - size / 2, pt.y + size / 2);

	::DeleteObject(hPen);
}

//填充矩形
void TDraw::FillRect(HDC hdc, RECT *rect, COLORREF crColor)
{
	HBRUSH hBrush;
	hBrush = ::CreateSolidBrush(crColor);
	::SelectObject(hdc, hBrush);
	::FillRect(hdc, rect, hBrush);
	::DeleteObject(hBrush);
}

//画网格
void TDraw::DrawGrid(HDC hdc, RECT rect, TConfiguration *Config)
{
	int minGrid = 10;
	int maxGrid = 40;

	DPOINT realGrid = {2,2};//真实网格大小2mm
	POINT screenGrid = Config->LengthToScreen(realGrid);

	while (screenGrid.x<minGrid || screenGrid.x>maxGrid)
	{
		if (screenGrid.x < minGrid)
		{
			realGrid.x *= 2;
			realGrid.y *= 2;
		}
		if (screenGrid.x > maxGrid)
		{
			realGrid.x /= 2;
			realGrid.y /= 2;
		}
		screenGrid = Config->LengthToScreen(realGrid);
	}

	//原点对小格大小取余，得到网格初始偏移量
	int xOffset = Config->GetOrg().x % screenGrid.x;
	int yOffset = Config->GetOrg().y % screenGrid.y;

	//原点减去网格偏移量，除以格子大小得到到原点的格数。再对5取余，得到原点到左侧最近大格格数。
	//用格数取余若得到xBigOffset则应画大格。
	int xBigOffset = ((Config->GetOrg().x - xOffset) / screenGrid.x) % 5;
	if (xBigOffset < 0) xBigOffset = 5 + xBigOffset;
	int yBigOffset = ((Config->GetOrg().y - yOffset) / screenGrid.y) % 5;
	if (yBigOffset < 0) yBigOffset = 5 + yBigOffset;

	int x, y;
	x = xOffset;
	y = yOffset;
	TLine xLine;
	TLine yLine;
	xLine.SetStyle(PS_SOLID, 1, Config->crGridSmall);
	yLine.SetStyle(PS_SOLID, 1, Config->crGridSmall);
	for (int i = 0; i < rect.right / screenGrid.x; i++)
	{
		xLine.ptBegin = { x, rect.top };
		xLine.ptEnd = { x, rect.bottom };
		if (i % 5 == xBigOffset)
			xLine.logpenStyle.lopnColor = Config->crGridBig;
		else
			xLine.logpenStyle.lopnColor = Config->crGridSmall;
		TDraw::DrawLine(hdc, xLine);
		x += screenGrid.x;
	}

	for (int i = 0; i < rect.bottom / screenGrid.y; i++)
	{
		yLine.ptBegin = { rect.left,y  };
		yLine.ptEnd = { rect.right, y };
		if (i % 5 == yBigOffset)
			yLine.logpenStyle.lopnColor = Config->crGridBig;
		else
			yLine.logpenStyle.lopnColor = Config->crGridSmall;
		TDraw::DrawLine(hdc, yLine);
		y += screenGrid.y;
	}
}

double TDraw::Distance(POINT pt1, POINT pt2)
{
	return sqrt(double((pt1.x - pt2.x)*(pt1.x - pt2.x) + (pt1.y - pt2.y)*(pt1.y - pt2.y)));
}

void TDraw::DrawRect(HDC hdc, RECT &rect,LOGPEN &logpen)
{
	HPEN hPen = ::CreatePenIndirect(&logpen);
	::SelectObject(hdc, hPen);
	::Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
	::DeleteObject(hPen);
}

void TDraw::DrawPickSquare(HDC hdc,POINT pt)
{
	LOGPEN logpen;
	logpen.lopnStyle = PS_SOLID;
	logpen.lopnWidth = { 1, 0 };
	logpen.lopnColor = RGB(101, 101, 101);

	const int size=10;
	RECT rect = { pt.x - size / 2, pt.y - size / 2, pt.x + size / 2, pt.y + size / 2 };
	DrawRect(hdc, rect, logpen);
	rect = { rect.left + 1, rect.top + 1, rect.right - 1, rect.bottom - 1 };
	FillRect(hdc, &rect, RGB(0, 127, 255));
}

void TDraw::ClientPosToScreen(HWND hWnd, POINT *pt)
{
	RECT rect;
	::GetWindowRect(hWnd, &rect);
	pt->x += rect.left;
	pt->y += rect.top;
}

void TDraw::GetMarginRect(RECT *rect, int margin)
{
	rect->left += margin;
	rect->top += margin;
	rect->right -= margin;
	rect->bottom -= margin;
}

RECT TDraw::GetMarginRect(RECT rect, int margin)
{
	RECT rc=rect;
	GetMarginRect(&rc, margin);
	return rc;
}

void TDraw::DrawTextAdvance(HDC hdc, TCHAR text[], RECT *rect, long FontSize, int FontWeight, unsigned long color, const TCHAR FontName[], UINT format)
{
	long lfHeight;
	HFONT hf;
	lfHeight = -MulDiv(FontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	hf = CreateFont(lfHeight, 0, 0, 0, FontWeight, 0, 0, 0, 0, 0, 0, 0, 0, FontName);
	SelectObject(hdc, hf);
	SetTextColor(hdc, color);
	DrawText(hdc, text, -1, rect, format);
	DeleteObject(hf);
}

void TDraw::DrawSystemFontText(HDC hdc, TCHAR text[], RECT &rect, COLORREF color, UINT format)
{
	DrawTextAdvance(hdc, text, &rect, 9, 400, color, TEXT("宋体"), format);
}