#include "TDraw.h"


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

void TDraw::DrawRealLine(HDC hdc, TRealLine RealLine,TConfiguration *Config)
{
	HPEN hPen;

	hPen = ::CreatePenIndirect(&RealLine.logpenStyle);
	::SelectObject(hdc, hPen);
	DrawLine(hdc, Config->RealToScreen(RealLine.ptBegin), Config->RealToScreen(RealLine.ptEnd));

	::DeleteObject(hPen);
}

//将点集以指定原点旋转theta，Y方向向上为正
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

//返回pt相对于原点ptO的角度，Y方向向上为正
double GetAngleFromPoint(POINT ptO, POINT pt)
{
	double angle;
	if (((pt.x - ptO.x) > 0) && ((pt.y - ptO.y) >= 0))//第1象限[0,PI/2)
		angle = atan(double(pt.y - ptO.y) / (pt.x - ptO.x));
	if (((pt.x - ptO.x) <= 0) && ((pt.y - ptO.y) > 0)) //第2象限[PI/2,PI)
		angle = M_PI / 2 + atan(double(ptO.x - pt.x) / (pt.y - ptO.y));
	if (((pt.x - ptO.x) < 0) && ((pt.y - ptO.y) <= 0))//第3象限[PI,PI*3/2)
		angle = M_PI + atan(double(ptO.y - pt.y) / (ptO.x - pt.x));
	if (((pt.x - ptO.x) >= 0) && ((pt.y - ptO.y) < 0))//第4象限[PI*3/2,2*PI)
		angle = 3 / 2 * M_PI + atan(double(pt.x - ptO.x) / (ptO.y - pt.y));
	return angle;
}

//返回pt相对于原点ptO的角度，Y方向向上为正
double TDraw::GetAngleFromPointReal(DPOINT ptO, DPOINT pt)
{
	double angle;
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

//以屏幕点返回角度，Y方向向下为正
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
	int xOffset = Config->Org.x % screenGrid.x;
	int yOffset = Config->Org.y % screenGrid.y;

	//原点减去网格偏移量，除以格子大小得到到原点的格数。再对5取余，得到原点到左侧最近大格格数。
	//用格数取余若得到xBigOffset则应画大格。
	int xBigOffset = ((Config->Org.x - xOffset) / screenGrid.x) % 5;
	if (xBigOffset < 0) xBigOffset = 5 + xBigOffset;
	int yBigOffset = ((Config->Org.y - yOffset) / screenGrid.y) % 5;
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

void TDraw::ClientPosToScreen(HWND hWnd, POINT *pt)
{
	RECT rect;
	::GetWindowRect(hWnd, &rect);
	pt->x += rect.left;
	pt->y += rect.top;
}