#pragma once
#include "MyMath.h"

#include <Windows.h>
#include "DPOINT.h"


enum units { UNITS_PX, UNITS_MM, UNITS_INCH };
class TConfiguration
{
private:
	POINT Org;
	double dProportion;
public:
	int FRAMEPOINT_R = 6;//节点圆半径
	int FRAMEPOINT_H = 20;//高-圆心到地线距离
	int FRAMEPOINT_B = 30;//底边长
	int FRAMEPOINT_SECTION_H = 10;//剖面线区域高度
	int FRAMEPOINT_ANGLE = 60.0 / 180.0 * M_PI;//三角形角度
	int SLIDER_B = 45;
	int SLIDER_H = 30;
	bool bDrawReal;
	double dAnglePrecision;
	units uUnits;
	int iMapMode;
	LOGPEN logpen;
	int iStyle, iWidth;
	COLORREF crBackground, crPen,crDash,crDot,crCoordinate;
	COLORREF crGridBig, crGridSmall;

	//Graph
	COLORREF crGraphBackground,crGraphGridBig, crGraphGridSmall;
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

