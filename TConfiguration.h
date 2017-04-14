#pragma once
#include "MyMath.h"

#include <vector>
#include <Windows.h>
#include "DPOINT.h"


enum units { UNITS_PX, UNITS_MM, UNITS_INCH };
class TConfiguration
{
private:

	double DPUX, DPUY;
	double DPMX, DPMY;
	double DPIX, DPIY;

	POINT Org;
	double dProportion = 1.0;
	int iStyle = PS_SOLID;
	int iWidth = 1;
	COLORREF PrevRandomColor;
	std::vector<COLORREF> vecColorLib;

	COLORREF crMouseLine;//黑色
	COLORREF crPen, crDash, crDot;

public:
	bool bDrawReal=true;
	bool bThemeDark = true;
	bool bRandomColor = true;
	bool bDrawGrid;
	int FRAMEPOINT_TRANS_R=5;//半透明状态节点圆半径
	int FRAMEPOINT_R = 6;//节点圆半径
	int FRAMEPOINT_H = 20;//高-圆心到地线距离
	int FRAMEPOINT_B = 30;//底边长
	int FRAMEPOINT_SECTION_H = 10;//剖面线区域高度
	int FRAMEPOINT_ANGLE = int(DEG2REG(60.0));//三角形弧度
	int SLIDER_B = 45;//滑块宽
	int SLIDER_H = 30;//滑块高
	int BAR_R = 7;//杆件圆角半径
	int CROSS_SIZE = 18;//×边长

	double dAnglePrecision = 1e-1;
	units uUnits = UNITS_MM;
	LOGPEN logpen;//当前画笔
	LOGPEN logpenFront;//画× 字 
	LOGPEN logpenAssist;//辅助线：白色虚线
	LOGPEN logpenMouseLine;//图 鼠标跟踪线：黑色虚线
	LOGPEN logpenGridSmall;
	LOGPEN logpenAssistLine;//辅助线：绿色虚线

	COLORREF crFront;//前景色：白色实线
	COLORREF crBackground, crCoordinate;
	COLORREF crGridBig, crGridSmall;

	COLORREF crLink;//FramePoint 黑色
	//Graph
	COLORREF crGraphBackground, crGraphGridBig, crGraphGridSmall;

	TConfiguration();
	~TConfiguration();
	void TConfiguration::SetTheme(bool bDark);

	LOGPEN TConfiguration::GetRandomColorLogpen();
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
	POINT TConfiguration::RealToScreen(DPOINT dpt) const;

	LONG TConfiguration::LengthToScreenX(double xlen);
	LONG TConfiguration::LengthToScreenY(double ylen);
	POINT TConfiguration::LengthToScreen(DPOINT dpt);

	double TConfiguration::ScreenToLengthX(LONG xpixel);
	double TConfiguration::ScreenToLengthY(LONG ypixel);
	DPOINT TConfiguration::ScreenToLength(POINT pt);
};

