#pragma once
#include "Common\MyMath.h"

#include <vector>
#include <Windows.h>
#include "Common\DPOINT.h"


enum units { UNITS_PX, UNITS_MM, UNITS_INCH };
class TConfiguration
{
private:

	double DPUX, DPUY;
	double DPMX, DPMY;
	double DPIX, DPIY;

	POINT Org;
	double dProportion = 1.0;
	UINT iStyle = PS_SOLID;
	int iWidth = 1;
	COLORREF PrevRandomColor;
	std::vector<COLORREF> vecColorLib;

	COLORREF crMouseLine;//��ɫ
	COLORREF crPen, crDash, crDot;

public:
	bool bDrawSchoolLogo = true;
	bool bDrawAxes = true;
	bool bDrawReal=true;
	bool bThemeDark = true;
	bool bRandomColor = true;
	bool bDrawGrid = true;
	int FRAMEPOINT_TRANS_R=5;//��͸��״̬�ڵ�Բ�뾶
	int FRAMEPOINT_R = 6;//�ڵ�Բ�뾶
	int FRAMEPOINT_H = 20;//��-Բ�ĵ����߾���
	int FRAMEPOINT_B = 30;//�ױ߳�
	int FRAMEPOINT_SECTION_H = 10;//����������߶�
	int FRAMEPOINT_ANGLE = int(DEG2RAD(60.0));//�����λ���
	int SLIDER_B = 45;//�����
	int SLIDER_H = 30;//�����
	int BAR_R = 7;//�˼�Բ�ǰ뾶
	int CROSS_SIZE = 18;//���߳�
	int TOLERANCE = 5;
	int COLINEAR_SYMBOL_L = 10;

	double dAnglePrecision = 1e-1;
	units uUnits = UNITS_MM;
	LOGPEN logpen;//��ǰ����
	LOGPEN logpenBlack;//
	LOGPEN logpenFront;//���� �� 
	LOGPEN logpenAssist;//�����ߣ���ɫ����
	LOGPEN logpenMouseLine;//ͼ �������ߣ���ɫ����
	LOGPEN logpenGridSmall;
	LOGPEN logpenAssistLine;//�����ߣ���ɫ����
	LOGPEN logpenConstraintLine;//Լ����
	LOGPEN logpenColinearSymbol;//���߷���

	COLORREF crFront;//ǰ��ɫ����ɫʵ��
	COLORREF crBackground, crCoordinate;
	COLORREF crGridBig, crGridSmall;

	COLORREF crLink;//FramePoint ��ɫ
	//Graph
	LOGPEN logpenGraphGridSmall,logpenGraphGridBig;
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
	void TConfiguration::SetOrg(const POINT &pt)
	{
		Org = pt;
	}
	POINT TConfiguration::GetOrg() const
	{
		return Org;
	}
	void TConfiguration::SetDPU(double Proportion);
	double TConfiguration::GetProportion();
	void TConfiguration::Initial(HWND hwnd);

	double TConfiguration::ScreenToRealX(LONG x)const;
	double TConfiguration::ScreenToRealY(LONG y)const;
	DPOINT TConfiguration::ScreenToReal(POINT pt)const;

	LONG TConfiguration::RealToScreenX(double x)const;
	LONG TConfiguration::RealToScreenY(double y)const;
	POINT TConfiguration::RealToScreen(DPOINT dpt) const;

	LONG TConfiguration::LengthToScreenX(double xlen)const;
	LONG TConfiguration::LengthToScreenY(double ylen)const;
	POINT TConfiguration::LengthToScreen(DPOINT dpt)const;

	double TConfiguration::ScreenToLengthX(LONG xpixel)const;
	double TConfiguration::ScreenToLengthY(LONG ypixel)const;
	DPOINT TConfiguration::ScreenToLength(POINT pt)const;
};

