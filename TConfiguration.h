#pragma once
#include <Windows.h>
#include "DPOINT.h"

enum units { UNITS_PX, UNITS_MM, UNITS_INCH };
class TConfiguration
{
private:
	POINT Org;
	double dProportion;
public:
	double dAnglePrecision;
	units uUnits;
	int iMapMode;
	int iStyle, iWidth;
	COLORREF crBackground, crPen,crDash,crDot,crCoordinate;
	COLORREF crGridBig, crGridSmall;
	double DPUX,DPUY;
	double DPMX, DPMY;
	double DPIX, DPIY;
	TConfiguration();
	~TConfiguration();
	void TConfiguration::SetOrg(LONG x, LONG y)
	{
		Org.x = x;
		Org.y = y;
	}
	POINT TConfiguration::GetOrg()
	{
		return Org;
	}
	void TConfiguration::SetDPU(double Proportion);
	double TConfiguration::GetProportion();
	void TConfiguration::Initial(HWND hwnd);

	double TConfiguration::ScreenToRealX(LONG x);
	double TConfiguration::ScreenToRealY(LONG y);
	DPOINT TConfiguration::ScreenToReal(POINT pt);

	LONG TConfiguration::RealToScreenX(double x);
	LONG TConfiguration::RealToScreenY(double y);
	POINT TConfiguration::RealToScreen(DPOINT dpt);

	LONG TConfiguration::LengthToScreenX(double xlen);
	LONG TConfiguration::LengthToScreenY(double ylen);
	POINT TConfiguration::LengthToScreen(DPOINT dpt);

	double TConfiguration::ScreenToLengthX(LONG xpixel);
	double TConfiguration::ScreenToLengthY(LONG ypixel);
	DPOINT TConfiguration::ScreenToLength(POINT pt);
};

