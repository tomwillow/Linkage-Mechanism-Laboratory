#pragma once
#include "DetectMemoryLeak.h"

#include "TCanvas.h"
#include "TAttach.h"
#include "TSlider.h"

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
	DPOINT dptPos = pConfig->ScreenToReal(ptNowPos);
	dptAttach = dptPos;
	AttachAxis(dptPos, pConfig->ScreenToReal(pConfig->GetOrg()));//吸附原点坐标轴
	AttachAxis(dptPos, dptCheckPos);
	if (!AttachPoint(dptPos))//后吸附线端点因为线端点更重要（覆盖掉辅助线点效果）
		AttachLine(ptNowPos);
}

void TAttach::AttachAll(POINT ptNowPos)
{
	DPOINT dptPos = pConfig->ScreenToReal(ptNowPos);
	dptAttach = dptPos;
	AttachAxis(dptPos, pConfig->ScreenToReal(pConfig->GetOrg()));//吸附原点坐标轴
	if (!AttachPoint(dptPos))//后吸附线端点因为线端点更重要（覆盖掉辅助线点效果）
		AttachLine(ptNowPos);
}


bool TAttach::AttachLine(POINT ptNowPos)
{
	bShowExtensionLine = false;
	for (auto i = pShape->Element.begin(); i != pShape->Element.end(); ++i)
	{
		switch ((*i)->eType)
		{
		case ELEMENT_REALLINE:
		case ELEMENT_BAR:
		case ELEMENT_SLIDEWAY:
			int status = TDraw::PointInRealLineOrExtension(ptNowPos, dptAttach, (TRealLine *)*i, pConfig);
			switch (status)
			{
			case 1:
				bAttachedEndpoint = false;
				bShowAttachPoint = true;
				bShowExtensionLine = true;
				ExtensionLine->SetPoint(((TRealLine *)*i)->ptBegin, dptAttach);
				pAttachElement = *i;
				return true;
			case 2:
				bAttachedEndpoint = false;
				bShowAttachPoint = true;
				bShowExtensionLine = true;
				ExtensionLine->SetPoint(((TRealLine *)*i)->ptEnd, dptAttach);
				pAttachElement = *i;
				return true;
			case 0:
				bAttachedEndpoint = false;
				bShowAttachPoint = true;
				bShowExtensionLine = true;
				ExtensionLine->SetPoint({ 0, 0 }, { 0, 0 });
				pAttachElement = *i;
				return true;
			}
		}
	}
	return false;
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

//读取pShape中的RealLine进行吸附，使用前应设置dptAttach为当前点
bool TAttach::AttachPoint(DPOINT dptPos)
{
	bAttachedEndpoint = false;
	bShowAttachPoint = false;
	pAttachElement = NULL;
	for (int i = 0; i < pShape->Element.size(); i++)
	{
		eAttachElementType = pShape->Element[i]->eType;
		switch (pShape->Element[i]->eType)
		{
		case ELEMENT_BAR:
		case ELEMENT_REALLINE:
		case ELEMENT_SLIDEWAY:
		{
			TRealLine *pRealLine = (TRealLine *)(pShape->Element[i]);
			//吸附起点
			if (DPTisApproached(dptPos, pRealLine->ptBegin, 10))
			{
				bAttachedEndpoint = true;
				bShowAttachPoint = true;
				pAttachElement = pRealLine;
				iAttachElementPointIndex = 0;
				dptAttach = pRealLine->ptBegin;
				return true;
			}

			//吸附终点
			if (DPTisApproached(dptPos, pRealLine->ptEnd, 10))
			{
				bAttachedEndpoint = true;
				bShowAttachPoint = true;
				pAttachElement = pRealLine;
				iAttachElementPointIndex = 1;
				dptAttach = pRealLine->ptEnd;
				return true;
			}
			break;
		}
		case ELEMENT_FRAMEPOINT:
		{
			TFramePoint *pFramePoint = (TFramePoint *)(pShape->Element[i]);
			//吸附机架点
			if (DPTisApproached(dptPos, pFramePoint->dpt, 10))
			{
				bAttachedEndpoint = true;
				bShowAttachPoint = true;
				pAttachElement = pFramePoint;
				iAttachElementPointIndex = 0;
				dptAttach = pFramePoint->dpt;
				return true;
			}
			break;
		}
		case ELEMENT_SLIDER:
		{
			TSlider *pSlider = (TSlider *)(pShape->Element[i]);
			for (auto iter = pSlider->vecDpt.begin(); iter != pSlider->vecDpt.end(); ++iter)
			{
				if (DPTisApproached(dptPos, TDraw::GetAbsolute(*iter, pSlider->dpt, pSlider->angle), 10))
				{
					bAttachedEndpoint = true;
					bShowAttachPoint = true;
					pAttachElement = pSlider;
					iAttachElementPointIndex = iter - pSlider->vecDpt.begin();
					dptAttach = TDraw::GetAbsolute(*iter, pSlider->dpt, pSlider->angle);
					return true;
				}
			}
			break;
		}
		//不吸附端点的元素
		case CONSTRAINT_COLINEAR:
		case CONSTRAINT_COINCIDE:
			break;
		default:
			assert(0);
			break;
		}
	}
	return false;
}

//两点屏幕距离小于distance
bool TAttach::DPTisApproached(DPOINT dpt1, DPOINT dpt2, int distance)
{
	if (abs(pConfig->LengthToScreenX(dpt1.x - dpt2.x)) < distance &&
		abs(pConfig->LengthToScreenY(dpt1.y - dpt2.y)) < distance)
		return true;
	else
		return false;
}