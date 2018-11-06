#pragma once
#include "..\Common\DetectMemoryLeak.h"

#include "..\Control\TCanvas.h"
#include "..\Tool\TAttach.h"
#include "..\Element\TSlider.h"

#include "..\Element\TFramePoint.h"
#include "..\Element\TRealLine.h"
#include "..\Element\TPolylineBar.h"

TAttach::TAttach(TCanvas *pCanvas, TShape *pShape, TConfiguration *pConfig)
{
	TAttach::pCanvas = pCanvas;
	TAttach::pShape = pShape;
	TAttach::pConfig = pConfig;

	iIvoryLine = 0;

	//设置极轴线
	bShowXAssist = false;
	bShowYAssist = false;

	XAssistLine = new TRealLine;
	YAssistLine = new TRealLine;
	XAssistLine->SetStyle(pConfig->logpenAssistLine);
	YAssistLine->SetStyle(pConfig->logpenAssistLine);

	//设置延长线
	bShowExtensionLine = false;
	ExtensionLine = new TRealLine;
	ExtensionLine->SetStyle(pConfig->logpenAssistLine);

	//设置捕捉点
	bAttachedEndpoint = true;
	bShowAttachPoint = false;
	pAttachElement = NULL;
	iAttachElementPointIndex = -1;

	bAttachedEndpointSelf = false;
}


TAttach::~TAttach()
{
	delete XAssistLine;
	delete YAssistLine;

	delete ExtensionLine;
}

void TAttach::InitialLine(POINT ptPos)
{
	XAssistLine->ptBegin = pConfig->ScreenToReal(ptPos);
	XAssistLine->ptEnd = pConfig->ScreenToReal(ptPos);
	YAssistLine->ptBegin = pConfig->ScreenToReal(ptPos);
	YAssistLine->ptEnd = pConfig->ScreenToReal(ptPos);
}

void TAttach::Draw(HDC hdc)
{

	if (bShowAttachPoint)
	{
		TDraw::DrawCross(hdc, pConfig->RealToScreen(dptAttach), pConfig->CROSS_SIZE, pConfig->logpenFront);
	}
	//画X辅助线
	if (bShowXAssist)
	{
		TDraw::DrawRealLine(hdc, *XAssistLine, pConfig);//
		TDraw::DrawCross(hdc, pConfig->RealToScreen(dptAttach), pConfig->CROSS_SIZE, pConfig->logpenFront);
	}

	//画Y辅助线
	if (bShowYAssist)
	{
		TDraw::DrawRealLine(hdc, *YAssistLine, pConfig);//
		TDraw::DrawCross(hdc, pConfig->RealToScreen(dptAttach), pConfig->CROSS_SIZE, pConfig->logpenFront);
	}

	if (bShowExtensionLine)
	{
		TDraw::DrawRealLine(hdc, *ExtensionLine, pConfig);//
	}

}

void TAttach::AttachAll(POINT ptNowPos, DPOINT dptCheckPos)
{
	dptAttach = pConfig->ScreenToReal(ptNowPos);

	AttachAxis(dptAttach, pConfig->ScreenToReal(pConfig->GetOrg()));//吸附原点坐标轴
	AttachAxis(dptAttach, dptCheckPos);

//后吸附线端点因为线端点更重要（覆盖掉辅助线点效果）

	//越靠上越优先
	if (!AttachPointSelf(dptAttach))
		if (!AttachPoint(dptAttach))
			if (!AttachLine_inner(dptAttach))
				if (!AttachLineSelf(dptAttach))
				return;
}

void TAttach::AttachAll(POINT ptNowPos)
{
	dptAttach = pConfig->ScreenToReal(ptNowPos);
	AttachAxis(dptAttach, pConfig->ScreenToReal(pConfig->GetOrg()));//吸附原点坐标轴

//后吸附线端点因为线端点更重要（覆盖掉辅助线点效果）

	//越靠上越优先
	if (!AttachPointSelf(dptAttach))
		if (!AttachPoint(dptAttach))
			if (!AttachLine_inner(dptAttach))
				if (!AttachLineSelf(dptAttach))
			return;
}


bool TAttach::AttachLine_Element_inner(DPOINT dptNowPos, DPOINT dptAbsolute1, DPOINT dptAbsolute2, const TElement *pElement, int PointIndex[2],
	const TConfiguration *pConfig)
{

	//计算dptNowPos在dpt1-dpt2线上的位置
	int status = TDraw::PointInRealLineOrExtension(dptNowPos, dptAttach, dptAbsolute1,dptAbsolute2, pConfig);
	if (status == -1)
		return false;

	//已捕捉
	bAttachedEndpoint = false;
	bShowAttachPoint = true;
	bShowExtensionLine = true;
	pAttachElement = const_cast<TElement *>(pElement);

	iAttachLinePointIndex[0] = PointIndex[0];
	iAttachLinePointIndex[1] = PointIndex[1];

	switch (status)
	{
	case 1://-1都不在 0在线段上 1在pt1一侧延长线 2在pt2一侧延长线
		ExtensionLine->SetPoint(dptAbsolute1, dptAttach);
		return true;
	case 2:
		ExtensionLine->SetPoint(dptAbsolute2, dptAttach);
		return true;
	case 0:
		ExtensionLine->SetPoint({ 0, 0 }, { 0, 0 });
		return true;
	}
	return false;
}

bool TAttach::AttachLineByRelativeVecPt(DPOINT dptNowPos, TElement *pElement, const std::vector<DPOINT> &vecdptRelative, const TConfiguration *pConfig)
{
	std::vector<DPOINT> vecdptAbsolute;
	TDraw::GetAbsoluteReal(vecdptAbsolute, vecdptRelative, pElement->dpt, pElement->angle);

	return AttachLineByAbsoluteVecPt(dptNowPos, pElement,vecdptAbsolute);
}

//以绝对坐标vecdptAbsolute顺序构成的线段，检测ptNowPos是否位于线上，并设置参数：
//bAttachedEnepoint
//bShowAttachPoint
//bShowExtensionLine  ExtensionLine
//iAttachLinePointIndex[0] iAttachLinePointIndex[1]
bool TAttach::AttachLineByAbsoluteVecPt(DPOINT dptNowPos,TElement *pElement,const std::vector<DPOINT> &vecdptAbsolute)
{
	if (vecdptAbsolute.empty()) return false;

	bShowExtensionLine = false;
	pAttachElement = NULL;

	for (auto pt = vecdptAbsolute.begin(); pt != vecdptAbsolute.end() - 1; ++pt)
	{
		int iPointIndex[2] = { pt - vecdptAbsolute.begin(), pt + 1 - vecdptAbsolute.begin() };
		if (AttachLine_Element_inner(dptNowPos, *pt, *(pt + 1),pElement, iPointIndex, pConfig))
		{
			pAttachElement = pElement;
			return true;
		}
	}
	return false;
}

void TAttach::AttachLine(POINT ptNowPos)
{
	dptAttach = pConfig->ScreenToReal(ptNowPos);
	AttachLine_inner(dptAttach);
}

bool TAttach::AttachLine_inner(DPOINT dptNowPos)
{
	bShowExtensionLine = false;
	pAttachElement = NULL;

	for (auto pElement:pShape->Element)
	{
		if (pElement->IsAttached(dptNowPos,this,pConfig))
			return true;
	}
	return false;
}

bool TAttach::AttachLineSelf(DPOINT dptNowPos)
{
	return AttachLineByAbsoluteVecPt(dptNowPos, nullptr, vecdpt);
}

//检查NowPos是否靠近以CheckPos为原点的极轴，使用前应设置dptAttach为当前点
bool TAttach::AttachAxis(DPOINT dptNowPos, DPOINT dptCheckPos)
{

	iIvoryLine = 0;
	RECT CanvasClientRect;
	::GetClientRect(pCanvas->m_hWnd, &CanvasClientRect);
	//检测X轴辅助线
	XAssistLine->ptBegin = dptCheckPos;
	if (abs(pConfig->LengthToScreenY(dptNowPos.y - dptCheckPos.y)) < 10)//当前点和起始点y轴偏差10像素
	{
		bShowXAssist = true;

		//设置辅助线x,y坐标
		if (dptNowPos.x - XAssistLine->ptBegin.x > 0)//当前位置位于辅助线起始点右边
		{
			XAssistLine->ptEnd.x = pConfig->ScreenToRealX(CanvasClientRect.right);
			iIvoryLine = 1;
		}
		else
		{
			XAssistLine->ptEnd.x = pConfig->ScreenToRealX(CanvasClientRect.left);
			iIvoryLine = 3;
		}
		XAssistLine->ptEnd.y = XAssistLine->ptBegin.y;

		//dptAttach.x = dptNowPos.x;
		dptAttach.y = XAssistLine->ptEnd.y;//将当前点吸附到坐标轴
	}
	else
	{
		bShowXAssist = false;
	}

	//检测Y轴辅助线
	YAssistLine->ptBegin = dptCheckPos;
	if (abs(pConfig->LengthToScreenX(dptNowPos.x - dptCheckPos.x)) < 10)
	{
		bShowYAssist = true;
		if (dptNowPos.y - YAssistLine->ptBegin.y > 0)//当前位置位于辅助线起始点上边
		{
			YAssistLine->ptEnd.y = pConfig->ScreenToRealY(CanvasClientRect.top);
			iIvoryLine = 2;
		}
		else
		{
			YAssistLine->ptEnd.y = pConfig->ScreenToRealY(CanvasClientRect.bottom);
			iIvoryLine = 4;
		}
		YAssistLine->ptEnd.x = YAssistLine->ptBegin.x;

		//dptAttach.y = dptNowPos.y;
		dptAttach.x = YAssistLine->ptEnd.x;//吸附
	}
	else
	{
		bShowYAssist = false;
	}

	if (bShowXAssist || bShowYAssist)
		return true;
	else
		return false;
}

bool TAttach::AttachPointByElement(DPOINT dptNowPos,DPOINT &dpt,int iPointIndex,const TElement *pElement, const TConfiguration *pConfig)
{
	if (DPTisApproached(dptNowPos,dpt))
	{
		bAttachedEndpoint = true;
		bShowAttachPoint = true;
		pAttachElement =const_cast<TElement *>(pElement);
		iAttachElementPointIndex = iPointIndex;
		dptAttach = dpt;
		return true;
	}
	return false;
}

//读取pShape中的RealLine进行吸附，使用前应设置dptAttach为当前点
bool TAttach::AttachPoint(DPOINT dptPos)
{
	bAttachedEndpoint = false;
	bShowAttachPoint = false;
	pAttachElement = NULL;
	for (auto pElement: pShape->Element)
	{
		if (pElement->PointIsAttached(dptPos, this, pConfig))
			return true;
	}

	return false;
}

//吸附vecdpt，使用前应设置dptAttach为当前点
bool TAttach::AttachPointSelf(DPOINT dptPos)
{
	bAttachedEndpointSelf = false;
	bShowAttachPoint = false;
	for (auto dpt : vecdpt)
	{
		if (DPTisApproached(dptPos, dpt))
		{
			bAttachedEndpointSelf = true;
			bShowAttachPoint = true;
			dptAttach = dpt;
			return true;
		}
	}
	return false;
}

//两点屏幕距离小于distance
bool TAttach::DPTisApproached(DPOINT dpt1, DPOINT dpt2)
{
	if (abs(pConfig->LengthToScreenX(dpt1.x - dpt2.x)) < iAttachPixel &&
		abs(pConfig->LengthToScreenY(dpt1.y - dpt2.y)) < iAttachPixel)
		return true;
	else
		return false;
}