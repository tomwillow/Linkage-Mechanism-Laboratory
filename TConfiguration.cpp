#pragma once
#include "TConfiguration.h"

#include <time.h>

TConfiguration::TConfiguration()
{
	vecColorLib.push_back(RGB(85, 160, 77));
	vecColorLib.push_back(RGB(83, 98, 164));
	vecColorLib.push_back(RGB(157, 78, 104));
	vecColorLib.push_back(RGB(164, 139, 84));
	PrevRandomColor = vecColorLib[0];

	crDot = RGB(0, 255, 0);
	crLink = RGB(0, 0, 0);
	crMouseLine = RGB(0, 0, 0);

	//Graph
	crGraphBackground = RGB(240, 240, 240);
	crGraphGridBig = RGB(200, 200, 200);
	crGraphGridSmall = RGB(220, 220, 220);
}


TConfiguration::~TConfiguration()
{
}

void TConfiguration::SetTheme(bool bDark)
{
	if (bDark)
	{//深色
		crFront = RGB(255, 255, 255);

		crPen = RGB(255, 255, 255);
		crBackground = RGB(33, 40, 48);//RGB(255,255,255);//
		crDash = RGB(0, 200, 0);
		crCoordinate = RGB(100, 100, 100);

		crGridBig = RGB(51, 57, 73);
		crGridSmall = RGB(39, 45, 56);
	}
	else
	{//浅色

		crFront = RGB(0, 0, 0);

		crPen = RGB(0, 0, 0);
		crBackground = RGB(255, 255, 230);//RGB(33, 40, 48);
		crDash = RGB(0, 200, 0);
		crCoordinate = RGB(100, 100, 100);

		crGridBig = RGB(220, 220, 220);
		crGridSmall = RGB(240, 240, 240);
	}

	logpenBlack = { PS_SOLID, { 1, 0 }, 0 };

	logpen = { iStyle, { iWidth, 0 }, crPen };
	logpenFront = { PS_SOLID, { 1, 0 }, crFront };
	logpenAssist = { PS_DOT, { 1, 0 }, crFront };
	logpenMouseLine = { PS_DOT, { 1, 0 }, crMouseLine };
	logpenGridSmall = { PS_SOLID, { 1, 0 }, crGridSmall };
	logpenAssistLine = { PS_DOT, { 1, 0 }, crDot };

	logpenConstraintLine = { PS_DOT, { 1, 0 }, crFront };
	logpenColinearSymbol = { PS_SOLID, { 5, 0 }, crLink };

	logpenGraphGridBig = { PS_SOLID, { 1, 0 }, crGraphGridBig };
	logpenGraphGridSmall = { PS_SOLID, { 1, 0 }, crGraphGridSmall };
}

void TConfiguration::Initial(HWND hwnd)
{
	HDC hdc;
	hdc = GetDC(hwnd);
	DPMX = GetSystemMetrics(SM_CXSCREEN) / GetDeviceCaps(hdc, HORZSIZE);
	DPMY = GetSystemMetrics(SM_CYSCREEN) / GetDeviceCaps(hdc, VERTSIZE);

	DPIX = DPMX*25.4;
	DPIY = DPMY*25.4;

	ReleaseDC(hwnd, hdc);

	Org = { 0, 0 };

	SetDPU(dProportion);

	//SetBrightStyle();
	bThemeDark = true;
	SetTheme(bThemeDark);

}

LOGPEN TConfiguration::GetRandomColorLogpen()
{
	srand((int)time(0));
	COLORREF color;
	do
	{
		color = vecColorLib[rand() % vecColorLib.size()];
	} while (color == PrevRandomColor);
	PrevRandomColor = color;

	return{ iStyle, { iWidth, 0 }, color };
}

void TConfiguration::SetDPU(double Proportion)
{
	if (Proportion > 100 || Proportion < 1e-3)
		return;
	dProportion = Proportion;
	DPUX = dProportion;
	DPUY = dProportion;
	switch (uUnits)
	{
	case UNITS_MM:
		DPUX *= DPMX;
		DPUY *= DPMY;
		break;
	case UNITS_INCH:
		DPUX *= DPIX;
		DPUY *= DPIY;
		break;
	}
}

double TConfiguration::GetProportion()
{
	return dProportion;
}

DPOINT TConfiguration::ScreenToReal(POINT pt) const
{
	DPOINT dpt;
	dpt.x = double(pt.x - Org.x) / DPUX;
	dpt.y = -double(pt.y - Org.y) / DPUY;
	return dpt;
}

double TConfiguration::ScreenToRealX(LONG x)const
{
	return double(x - Org.x) / DPUX;
}

double TConfiguration::ScreenToRealY(LONG y)const
{
	return -double(y - Org.y) / DPUY;
}

LONG TConfiguration::RealToScreenX(double x)const
{
	return (LONG)(x*DPUX + Org.x);
}

LONG TConfiguration::RealToScreenY(double y)const
{
	return (LONG)(-y*DPUY + Org.y);
}
POINT TConfiguration::RealToScreen(DPOINT dpt) const
{
	POINT pt;
	pt.x = (LONG)(dpt.x*DPUX + Org.x);
	pt.y = (LONG)(-dpt.y*DPUY + Org.y);
	return pt;
}

LONG TConfiguration::LengthToScreenX(double xlen)const
{
	return (LONG)(xlen*DPUX);
}

LONG TConfiguration::LengthToScreenY(double ylen)const
{
	return (LONG)(ylen*DPUY);
}

POINT TConfiguration::LengthToScreen(DPOINT dpt) const
{
	return{ (LONG)(dpt.x*DPUX), (LONG)(dpt.y*DPUY) };
}

double TConfiguration::ScreenToLengthX(LONG xpixel)const
{
	return (double)(xpixel) / DPUX;
}

double TConfiguration::ScreenToLengthY(LONG ypixel)const
{
	return (double)(ypixel) / DPUY;
}

DPOINT TConfiguration::ScreenToLength(POINT pt)const
{
	return{ pt.x / DPUX, pt.y / DPUY };
}