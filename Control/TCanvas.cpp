#pragma once

#include "..\Common\tchar_head.h"
#include <stdio.h>

#include "..\Common\MyString.h"

#include "..\resource.h"
#include "..\Control\TCanvas.h"
#include "..\TConfiguration.h"
#include "..\Tool\TManageTool.h"
#include "TStatus.h"
#include "..\Control\TTrackbar.h"


#include "..\Element\TShape.h"

#include "..\Draw\TDraw.h"
#include "..\global.h"

TCanvas::TCanvas(TConfiguration *pConfig, TManageTool *pManageTool, TStatus *pStatus, TTrackbar *pTrackbar, TShape *pShape) :ShowId(-1), ShowIndex(-1)
{
	bMButtonPressing = false;
	TCanvas::pConfig = pConfig;
	TCanvas::pManageTool = pManageTool;
	TCanvas::pStatus = pStatus;
	TCanvas::pTrackbar = pTrackbar;
	TCanvas::pShape = pShape;
}


TCanvas::~TCanvas()
{
}

void TCanvas::OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	m_hWnd = hWnd;
}

void TCanvas::DealMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (pManageTool->m_pCurrentTool != NULL)
	{
		pManageTool->Message(hWnd, uMsg, wParam, lParam);
	}
	if (uMsg == WM_SETCURSOR)
		pManageTool->SetCursor(hWnd, uMsg, wParam, lParam);
}

void TCanvas::OnCommand(WPARAM wParam, LPARAM lParam)
{
	//热键返回给主窗口
	SendMessage(m_hParent, WM_COMMAND, wParam, lParam);
	//win.OnCommand(wParam, lParam);
}

void TCanvas::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	DealMessage(m_hWnd, WM_KEYDOWN, wParam, lParam);
}

void TCanvas::OnLButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetFocus(hWnd);
	DealMessage(hWnd, uMsg, wParam, lParam);
	Invalidate();
}

void TCanvas::OnLButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetFocus(hWnd);
	DealMessage(hWnd, uMsg, wParam, lParam);
}

void TCanvas::OnRButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetFocus(hWnd);
	DealMessage(hWnd, uMsg, wParam, lParam);
}

void TCanvas::OnRButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetFocus(hWnd);
	DealMessage(hWnd, uMsg, wParam, lParam);
}

void TCanvas::OnHotKey(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetFocus(hWnd);
	DealMessage(hWnd, uMsg, wParam, lParam);
}

void TCanvas::OnMouseMove(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	POINT ptPos;

	ptPos.x = LOWORD(lParam);
	ptPos.y = HIWORD(lParam);

	switch (pConfig->uUnits)
	{
	case UNITS_MM:
		pStatus->SetText(IDR_STATUS_COORDINATE, TEXT("(%.3f mm,%.3f mm)"), pConfig->ScreenToReal(ptPos).x, pConfig->ScreenToReal(ptPos).y);
		break;
	case UNITS_INCH:
		pStatus->SetText(IDR_STATUS_COORDINATE, TEXT("(%.3f in,%.3f in)"), pConfig->ScreenToReal(ptPos).x, pConfig->ScreenToReal(ptPos).y);
		break;
	}

	//若中键按下，拖动坐标原点并刷新显示
	if (bMButtonPressing)
	{
		pConfig->SetOrg(pConfig->GetOrg().x + (ptPos.x - uiMoveStart.x),
			pConfig->GetOrg().y + (ptPos.y - uiMoveStart.y));
		uiMoveStart = ptPos;
	}

	DealMessage(hWnd, uMsg, wParam, lParam);

	InvalidateRect(this->m_hWnd, &(this->ClientRect), false);
}

void TCanvas::OnSetCursor(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (bMButtonPressing)
		SetCursor(LoadCursor(NULL, IDC_SIZEALL));
	else
		DealMessage(hWnd, uMsg, wParam, lParam);
}

void TCanvas::OnMButtonDown(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bMButtonPressing = true;
	uiMoveStart.x = LOWORD(lParam);
	uiMoveStart.y = HIWORD(lParam);
	PostMessage(hWnd, WM_SETCURSOR, 0, 0);
}

void TCanvas::OnMButtonUp(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	bMButtonPressing = false;
	PostMessage(hWnd, WM_SETCURSOR, 0, 0);
}

void TCanvas::OnMouseWheel(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	POINT ptPos;
	ptPos.x = LOWORD(lParam);
	ptPos.y = HIWORD(lParam);

	//WM_MOUSEWHEEL事件获得的是相对于屏幕的绝对坐标
	ScreenToClient(hWnd, &ptPos);

	//首先取得鼠标位置到原点的实际距离，在比例变动后以相同的实际距离推断新的原点位置。
	//变动前后的依据是实际距离不变。
	double xlen = pConfig->ScreenToLengthX(ptPos.x - pConfig->GetOrg().x);
	double ylen = pConfig->ScreenToLengthY(ptPos.y - pConfig->GetOrg().y);

	if (zDelta > 0)
		pConfig->SetDPU(pTrackbar->GetNextValue());
	else
		pConfig->SetDPU(pTrackbar->GetPrevValue());

	pConfig->SetOrg(ptPos.x - pConfig->LengthToScreenX(xlen), ptPos.y - pConfig->LengthToScreenY(ylen));

	pTrackbar->SetPosByValue(pConfig->GetProportion(), 1e-6);

	//因为原来也是0，SetPos后也是0，SetPos不激发NOTIFY，所以手动发
	PostMessage(m_hParent, WM_NOTIFY, MAKELONG(IDR_TRACKBAR, 0), 0);

	DealMessage(hWnd, uMsg, wParam, lParam);
	Invalidate();
}

//逆时针
void GetArcDPOINT(std::vector<DPOINT> &result,DPOINT dptO, double r, double angle1, double angle2, double all_step) 
{
	double dx = (angle2 - angle1) / all_step;
	double t = angle1, x = 0, y = 0;
	for (int i = 0; i <= all_step; ++i)
	{
		x = r*cos(t) + dptO.x;
		y = r*sin(t) + dptO.y;
		result.push_back({ x, y });
		t += dx;
	}
}

DPOINT pol2cart(double theta, double r)
{
	double x = r*cos(theta);
	double y = r*sin(theta);
	return{ x, y };
}

void DrawGear(HDC hdc,int m, int z, double x,double alphaDEG, DPOINT dpt,double ang,const LOGPEN &logpen,const TConfiguration *pConfig)
{
	using Vector = std::vector<double>;
	bool error = false;

	HPEN hPen = CreatePenIndirect(&logpen);
	HBRUSH hBrush =(HBRUSH)::GetStockObject(NULL_BRUSH);
	SelectObject(hdc, hPen);
	SelectObject(hdc, hBrush);

	double ha = 1, c = (m>=1)?0.25:0.35; //齿顶高系数; 顶隙系数; 变位系数
	double alpha = DEG2RAD(alphaDEG); //压力角
		
	double	r = z*m / 2; //分度圆半径
	double	ra = r + (ha + x)*m; //齿顶圆半径
	double	rf = r - (ha + c - x)*m; //齿根圆半径
	if (rf < 0) error = true;//变位系数过小（-x过大），导致齿根圆半径为负

	double	rb = r*cos(alpha); //基圆半径

	//TDraw::DrawCircle(hdc, pConfig->RealToScreen(dpt), pConfig->LengthToScreenX(ra));
	//TDraw::DrawCircle(hdc, pConfig->RealToScreen(dpt), pConfig->LengthToScreenX(rb));
	//TDraw::DrawCircle(hdc, pConfig->RealToScreen(dpt), pConfig->LengthToScreenX(rf));

	double p = M_PI*m; //齿距
	double s = p / 2 + 2 * x*m*tan(alpha); //齿厚

	double beta1 = involute(acos(rb / r));
	double beta2 = s / (2 * r);
	double beta = beta1 + beta2;

	double rr = abs(rb - rf) / 2;//粗取圆角半径
	double mu = M_PI / z - beta;
	double rr1 = rf*sin(mu) / (1 - sin(mu));//两圆角相交时的半径
	double rr2 = abs(rb - rf);//基圆至齿根距离
	if (rr > min(rr1, rr2)) rr = min(rr1, rr2);

	std::vector<DPOINT> vecdpt;

	//开始计算
	//第-2段 圆角 = 圆角5镜像
	double HE = rr, OH = rf + rr;
	double gama = asin(HE / OH);
	double epsilon = M_PI / 2 - gama - beta;
	double Hx = OH*sin(epsilon);
	double Hy = OH*cos(epsilon);
	double angle1 = M_PI + gama + beta;
	double angle2 = angle1 + (M_PI / 2 - gama);

	int pt_count__2 = pConfig->LengthToScreenX(rr)/2;//圆角总点数
	if (pt_count__2 < 2) pt_count__2 = 2;

	std::vector<DPOINT> vecdpt5;
	GetArcDPOINT(vecdpt5, { Hx, Hy }, rr, angle1, angle2, pt_count__2-1);
	for (auto it = vecdpt5.begin(); it != vecdpt5.end(); ++it)
	{
		vecdpt.push_back({ it->x, -it->y });
	}

	//第-1段 直线

	//第1段 渐开线
	int step_count = pConfig->LengthToScreenX(ra - rb)/4; if (step_count < 2) step_count = 2;
	double alphaK = 0, alphaK_end = acos(rb / ra),  dx = alphaK_end /step_count ;

	if (alphaK_end > beta) error = true;//变位系数过大，齿顶渐开线相交

	for (int step = 0; step <= step_count;++step)
	{
		double thetaK1 = involute(alphaK) - beta;
		double rK1 = rb / cos(alphaK);
		vecdpt.push_back(pol2cart(thetaK1, rK1));

		alphaK += dx;
	}

	//第2段 齿顶圆
	double thetaba = involute(acos(rb / ra));
	double thetaa = beta - thetaba;

	step_count = pConfig->LengthToScreenX(thetaa * 2 * ra);//半径*弧度=弧长
	if (step_count < 2) step_count = 2;
	GetArcDPOINT(vecdpt, { 0, 0 }, ra, -thetaa, thetaa, step_count);

	//第3段 渐开线 = 渐开线1镜像
	std::vector<DPOINT> vecdpt3(vecdpt.begin()+pt_count__2, vecdpt.end() - step_count);
	for (auto &it = vecdpt3.rbegin(); it != vecdpt3.rend(); ++it)
	{
		it->y = -it->y;
		vecdpt.push_back(*it);
	}

	//第4段 直线

	//第5段 圆角
	vecdpt.insert(vecdpt.end(), vecdpt5.rbegin(),vecdpt5.rend());

	//第6段 齿根圆弧
	double phi = 2 * M_PI / z - 2 * beta - 2 * gama;

	step_count = pConfig->LengthToScreenX(ra*phi);//半径*弧度=弧长
	GetArcDPOINT(vecdpt, { 0, 0 }, rf, beta+gama, beta+gama+phi, step_count);

	//阵列
	std::vector<DPOINT> vecdpt_all;// (vecdpt.begin(), vecdpt.end());
	dx = 2 * M_PI / z;
	double now_angle = 0;
	for (int i = 0; i < z; ++i)
	{
		for (auto &dpt : vecdpt)
		{
			double x = dpt.x*cos(now_angle) - dpt.y*sin(now_angle);
			double y = dpt.x*sin(now_angle) + dpt.y*cos(now_angle);
			vecdpt_all.push_back({ x, y });
		}
		now_angle += dx;
	}

	//转为屏幕坐标
	std::vector<POINT> vecpt;
	TDraw::GetAbsoluteScreen(vecpt, vecdpt_all, dpt, ang, pConfig);//在这里旋转
	Polygon(hdc, vecpt.data(), vecpt.size());

	DeleteObject(hPen);
	DeleteObject(hBrush);
}


void TCanvas::OnDraw(HDC hdc)
{
	SetBkMode(hdc, TRANSPARENT);

	//填充背景
	TDraw::FillRect(hdc, &ClientRect, pConfig->crBackground);

	//画网格
	if (pConfig->bDrawGrid)
		TDraw::DrawGrid(hdc, ClientRect, pConfig->GetOrg(), pConfig->crGridBig, pConfig->crGridSmall, pConfig);

	//画校徽
	if (pConfig->bDrawSchoolLogo)
		Draw.DrawLogo(m_hInst,IDB_PNG_SCHOOL,TEXT("PNG"),hdc,ClientRect);

#if (defined _STUDENT) || (defined _TEACHER)
	//写学号信息
	std::String s;
	s << TEXT("班级：") + sStudentClass;
	s << TEXT("\r\n\r\n姓名：") + sStudentName;
	s << TEXT("\r\n\r\n学号：") + sStudentNumber;
#endif
#ifdef _TEACHER
	s << TEXT("\r\n\r\n成绩：") + sStudentScore;
#endif
#if (defined _STUDENT) || (defined _TEACHER)
	POINT pt = { ClientRect.left, ClientRect.bottom };
	TDraw::DrawTips(hdc, pt, ClientRect,s.c_str(), pConfig);
#endif

	if (pConfig->bDrawAxes)//画坐标原点
		TDraw::DrawAxes(hdc, pConfig->GetOrg().x, pConfig->GetOrg().y, pConfig->crCoordinate);

	//图形绘制
	for (auto pElement : pShape->Element)
	{
		pElement->Draw(hdc, pConfig);

		if (pElement->id == ShowId)
		{
			String sId;
			sId+=TEXT("id:")+To_string(ShowId);
			sId+=TEXT(" pt") +To_string(ShowIndex);
			
			TDraw::DrawAdjustedText(hdc, pConfig->RealToScreen(pElement->GetAbsolutePointByIndex(ShowIndex)), ClientRect, sId.c_str(),10,false, pConfig);
		}
	}

	//工具类绘制
	if (pManageTool->m_pCurrentTool != NULL)
		pManageTool->m_pCurrentTool->Draw(hdc);//工具在使用中的图形绘制交由工具类执行

	//DrawGear(hdc, 2, 42, 0, 20, { 0, 0 },0, pConfig->logpen, pConfig);
}

void TCanvas::SetShowIdAndIndex(int id, int index)
{
	ShowId = id;
	ShowIndex = index;
}