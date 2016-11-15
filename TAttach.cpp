#include "TAttach.h"


TAttach::TAttach(HWND hCanvas, TShape *shape, TConfiguration *config)
{
	TAttach::hCanvas = hCanvas;
	TAttach::Shape = shape;
	TAttach::Config = config;

	iIvoryLine = 0;

	bShowXAssist = false;
	bShowYAssist = false;

	XAssistLine = new TRealLine;
	YAssistLine = new TRealLine;
	XAssistLine->SetStyle(PS_DOT, 1, Config->crDot);
	YAssistLine->SetStyle(PS_DOT, 1, Config->crDot);

	bAttachPoint = false;
}


TAttach::~TAttach()
{
	delete XAssistLine;
	delete YAssistLine;
}

void TAttach::InitialLine(POINT ptPos)
{
	XAssistLine->ptBegin = Config->ScreenToReal(ptPos);
	XAssistLine->ptEnd = Config->ScreenToReal(ptPos);
	YAssistLine->ptBegin = Config->ScreenToReal(ptPos);
	YAssistLine->ptEnd = Config->ScreenToReal(ptPos);
}

//not use
void TAttach::Load(HWND hCanvas, TShape *shape, TConfiguration *config)
{
}

void TAttach::Draw(HDC hdc)
{

	if (bAttachPoint)
	{
		TDraw::DrawCross(hdc, Config->RealToScreen(dptAttach), 18, { PS_SOLID, { 1, 0 }, Config->crPen });
	}
	//画X辅助线
	if (bShowXAssist)
	{
		TDraw::DrawRealLine(hdc, *XAssistLine, Config);//
		TDraw::DrawCross(hdc, Config->RealToScreen(dptAttach), 18, { PS_SOLID, { 1, 0 }, Config->crPen });
	}

	//画Y辅助线
	if (bShowYAssist)
	{
		TDraw::DrawRealLine(hdc, *YAssistLine, Config);//
		TDraw::DrawCross(hdc, Config->RealToScreen(dptAttach), 18, { PS_SOLID, { 1, 0 }, Config->crPen });
	}

}

void TAttach::AttachAll(POINT ptNowPos, DPOINT dptCheckPos)
{
	DPOINT dptPos = Config->ScreenToReal(ptNowPos);
	dptAttach = dptPos;
	AttachAxis(dptPos, Config->ScreenToReal(Config->GetOrg()));
	AttachAxis(dptPos, dptCheckPos);
	AttachPoint(dptPos);//后吸附线端点因为线端点更重要（覆盖掉辅助线点效果）
}

void TAttach::AttachAll(POINT ptNowPos)
{
	DPOINT dptPos = Config->ScreenToReal(ptNowPos);
	dptAttach = dptPos;
	AttachAxis(dptPos, Config->ScreenToReal(Config->GetOrg()));
	AttachPoint(dptPos);//后吸附线端点因为线端点更重要（覆盖掉辅助线点效果）
}

//检查NowPos是否靠近CheckPos的极轴，使用前应设置dptAttach
bool TAttach::AttachAxis(DPOINT dptNowPos, DPOINT dptCheckPos)
{

	iIvoryLine = 0;
	RECT CanvasClientRect;
	::GetClientRect(hCanvas, &CanvasClientRect);
	//检测X轴辅助线
	XAssistLine->ptBegin = dptCheckPos;
	if (abs(Config->LengthToScreenY(dptNowPos.y - dptCheckPos.y)) < 10)//当前点和起始点y轴偏差10像素
	{
		bShowXAssist = true;

		//设置辅助线x,y坐标
		if (dptNowPos.x - XAssistLine->ptBegin.x > 0)//当前位置位于辅助线起始点右边
		{
			XAssistLine->ptEnd.x = Config->ScreenToRealX(CanvasClientRect.right);
			iIvoryLine = 1;
		}
		else
		{
			XAssistLine->ptEnd.x = Config->ScreenToRealX(CanvasClientRect.left);
			iIvoryLine = 3;
		}
		XAssistLine->ptEnd.y = XAssistLine->ptBegin.y;

		//dptAttach.x = dptCheckPos.x;
		dptAttach.y = XAssistLine->ptEnd.y;//将当前点吸附到坐标轴
	}
	else
	{
		bShowXAssist = false;
	}

	//检测Y轴辅助线
	YAssistLine->ptBegin = dptCheckPos;
	if (abs(Config->LengthToScreenX(dptNowPos.x - dptCheckPos.x)) < 10)
	{
		bShowYAssist = true;
		if (dptNowPos.y - YAssistLine->ptBegin.y > 0)//当前位置位于辅助线起始点上边
		{
			YAssistLine->ptEnd.y = Config->ScreenToRealY(CanvasClientRect.top);
			iIvoryLine = 2;
		}
		else
		{
			YAssistLine->ptEnd.y = Config->ScreenToRealY(CanvasClientRect.bottom);
			iIvoryLine = 4;
		}
		YAssistLine->ptEnd.x = YAssistLine->ptBegin.x;

		//dptAttach.y = dptCheckPos.y;
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

//读取Shape中的RealLine进行吸附，使用前应设置dptAttach
void TAttach::AttachPoint(DPOINT dptPos)
{
	bAttachPoint = false;
	for (int i = 0; i < Shape->Element.size(); i++)
	{
		switch (Shape->Element[i].eType)
		{
		case ELEMENT_REALLINE:
			//吸附起点
			if (DPTisApproached(dptPos, Shape->Element[i].ptBegin, 10))
			{
				bAttachPoint = true;
				dptAttach = Shape->Element[i].ptBegin;
				return;
			}

			//吸附终点
			if (DPTisApproached(dptPos, Shape->Element[i].ptEnd, 10))
			{
				bAttachPoint = true;
				dptAttach = Shape->Element[i].ptEnd;
				return;
			}
			break;

		case ELEMENT_FRAMEPOINT:
			//吸附机架点
			if (DPTisApproached(dptPos, Shape->Element[i].dpt, 10))
			{
				bAttachPoint = true;
				dptAttach = Shape->Element[i].dpt;
				return;
			}
			break;
		}
	}
}

bool TAttach::DPTisApproached(DPOINT dpt1, DPOINT dpt2, int distance)
{
	if (abs(Config->LengthToScreenX(dpt1.x - dpt2.x)) < distance &&
		abs(Config->LengthToScreenY(dpt1.y - dpt2.y)) < distance)
		return true;
	else
		return false;
}