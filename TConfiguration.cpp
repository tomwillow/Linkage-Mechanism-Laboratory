#include "TConfiguration.h"


TConfiguration::TConfiguration()
{

}


TConfiguration::~TConfiguration()
{
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

	uUnits = UNITS_MM;
	Org = { 0, 0 };
	dProportion = 1.0;
	SetDPU(dProportion);

	iStyle = PS_SOLID;
	iWidth = 1;

	crBackground = RGB(33,40,48);
	crPen = RGB(255, 255, 255);
	crDash = RGB(0, 200, 0);
	crDot = RGB(0, 200, 0);

	crCoordinate = RGB(100, 100, 100);

	crGridBig = RGB(51, 57, 73);
	crGridSmall = RGB(39, 45, 56);

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

DPOINT TConfiguration::ScreenToReal(POINT pt)
{
	DPOINT dpt;
		dpt.x = double(pt.x - Org.x) / DPUX;
		dpt.y = -double(pt.y - Org.y) / DPUY;
	return dpt;
}

double TConfiguration::ScreenToRealX(LONG x)
{
		return double(x - Org.x) / DPUX;
}

double TConfiguration::ScreenToRealY(LONG y)
{
		return -double(y - Org.y) / DPUY;
}

LONG TConfiguration::RealToScreenX(double x)
{
		return (LONG)(x*DPUX + Org.x);
}

LONG TConfiguration::RealToScreenY(double y)
{
		return (LONG)(-y*DPUY + Org.y);
}
POINT TConfiguration::RealToScreen(DPOINT dpt)
{
	POINT pt;
		pt.x = (LONG)(dpt.x*DPUX + Org.x);
		pt.y = (LONG)(-dpt.y*DPUY + Org.y);
	return pt;
}

LONG TConfiguration::LengthToScreenX(double xlen)
{
	return (LONG)(xlen*DPUX);
}

LONG TConfiguration::LengthToScreenY(double ylen)
{
	return (LONG)(ylen*DPUY);
}

POINT TConfiguration::LengthToScreen(DPOINT dpt)
{
	POINT pt;
	pt.x = (LONG)(dpt.x*DPUX);
	pt.y = (LONG)(dpt.y*DPUY);
	return pt;
}

double TConfiguration::ScreenToLengthX(LONG xpixel)
{
	return (double)(xpixel)/DPUX;
}

double TConfiguration::ScreenToLengthY(LONG ypixel)
{
	return (double)(ypixel)/DPUY;
}

DPOINT TConfiguration::ScreenToLength(POINT pt)
{
	DPOINT dpt;
	pt.x = (double)(pt.x)/DPUX;
	pt.y = (double)(pt.y)/DPUY;
	return dpt;
}
