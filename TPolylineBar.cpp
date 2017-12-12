#include "tchar_head.h"
#include "MyMath.h"
#include "TPolylineBar.h"

#include "TAttach.h"
#include "TConfiguration.h"
#include "TDraw.h"
#include "TListView.h"

TPolylineBar::TPolylineBar()
{
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_POLYLINEBAR;
	eClass = ELEMENT_CLASS_NORMAL;

}


TPolylineBar::~TPolylineBar()
{
}

const String TPolylineBar::GetElementTypeName()//得到类型名称
{
	return TEXT("多段杆");
}

void TPolylineBar::NoticeListView(TListView *pListView)
{
	TElement::NoticeListView(pListView);

	TCHAR buffer[16];

	pListView->AddAttributeItem(TEXT("原点"), CTRLTYPE_COOR_EDIT, &dpt, TEXT("%.3f,%.3f"), dpt.x, dpt.y);
	pListView->AddAttributeItem(TEXT("角度"), CTRLTYPE_ANGLE_VALUE_EDIT, &angle, TEXT("%f"), RAD2DEG(angle));

	for (size_t i = 0; i < vecDpt.size(); ++i)
	{
		_stprintf(buffer, TEXT("P%d"), i);
		pListView->AddAttributeItem(buffer, CTRLTYPE_COOR_EDIT, &(vecDpt[i]), TEXT("%.3f,%.3f"), vecDpt[i].x, vecDpt[i].y);
	}
}

void TPolylineBar::Draw(HDC hdc, const TConfiguration* pConfig)
{
	TDraw::DrawPolylineBar(hdc, this, pConfig);
	//if (bDrawSquare) DrawPickSquare(hdc, pConfig);
}

void TPolylineBar::DrawPickSquare(HDC hdc, const TConfiguration* pConfig)
{
	for (auto &Dpt: vecDpt)
		//画拾取方格
		TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(TDraw::GetAbsolute(Dpt,dpt, angle)));
}

bool TPolylineBar::Picked(const POINT &ptPos, const TConfiguration *pConfig)
{
	return TDraw::PointInPolylineBar(ptPos, this, pConfig);
}

bool TPolylineBar::InSelWindow(RECT rect, const TConfiguration *pConfig)
{	
	//相对坐标转为绝对坐标
	std::vector<POINT> vecpt;
	TDraw::GetAbsoluteScreen(vecpt, vecDpt,dpt, angle, pConfig);

	return VecPointInRect(rect, vecpt);
}

bool TPolylineBar::InSelCross(RECT rect, const TConfiguration *pConfig)
{	
	//相对坐标转为绝对坐标
	std::vector<POINT> vecpt;
	TDraw::GetAbsoluteScreen(vecpt, vecDpt, dpt, angle, pConfig);

	return VecPointCrossRect(rect, vecpt);
}

bool TPolylineBar::IsAttached(DPOINT dptNowPos, TAttach *pAttach, const TConfiguration *pConfig)
{
	return pAttach->AttachLineByRelativeVecPt(dptNowPos, this, vecDpt, pConfig);
}

bool TPolylineBar::PointIsAttached(DPOINT dptNowPos, TAttach *pAttach, const TConfiguration *pConfig)
{
	for (auto iter = vecDpt.begin(); iter !=vecDpt.end(); ++iter)
	{
		if (pAttach->AttachPointByElement(dptNowPos, TDraw::GetAbsolute(*iter, dpt, angle), iter - vecDpt.begin(), this, pConfig))
			return true;
	}
	return false;
}