#include "TSelectTool.h"

#include "TDraw.h"
#include "TMainWindow.h"
#include "TConfiguration.h"

extern TMainWindow win;
TSelectTool::TSelectTool()
{
	pShape = &(win.m_Shape);
	pConfig = &(win.m_Configuration);
	pCanvas = &(win.Canvas);
	pListView = &(win.RightWindow.ListView);
	iPickIndex = -1;
}

//由TTool的虚析构函数重载
TSelectTool::~TSelectTool()
{
	if (iPickIndex != -1)
	{
		//恢复线型
		pShape->Element[iPickIndex].logpenStyleShow.lopnStyle = pShape->Element[iPickIndex].logpenStyle.lopnStyle;

		//通知TreeView取消选中

		//通知ListView更新
		pListView->DeleteAllItems();
	}
}


void TSelectTool::OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
	case VK_DELETE:
		if (iPickIndex != -1)
		{
			pShape->DeleteElement(iPickIndex);
			iPickIndex = -1;
			//通知TreeView取消选中

			//通知ListView更新
			pListView->DeleteAllItems();
		}
		return;
	}
}

void TSelectTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_ARROW));
}


void TSelectTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
}

bool TSelectTool::PickRealLine(POINT ptPos, TElement &RealLine)
{
	if (RealLine.eType == ELEMENT_REALLINE)
	{
		POINT pt1 = pConfig->RealToScreen(RealLine.ptBegin);
		POINT pt2 = pConfig->RealToScreen(RealLine.ptEnd);
		double length = TDraw::Distance(pt1, pt2);
		double length1 = TDraw::Distance(ptPos, pt1);
		double length2 = TDraw::Distance(ptPos, pt2);

		if (length1 + length2 - length <= 0.5)//容差
			return true;
		else
			return false;
	}
}

void TSelectTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	//若之前已经拾取得到线，则恢复线型。（若再次拾取下面将变更线型，若不是则线型将在此恢复）
	if (iPickIndex != -1)
	{
		pShape->Element[iPickIndex].logpenStyleShow.lopnStyle = pShape->Element[iPickIndex].logpenStyle.lopnStyle;
		iPickIndex = -1;
	}

	//遍历所有
	for (int i = 0; i < pShape->Element.size(); i++)
	{
		switch (pShape->Element[i].eType)
		{
		case ELEMENT_REALLINE:
			if (PickRealLine(ptPos, pShape->Element[i]))//发现拾取
			{
				iPickIndex = i;
			}
			break;
		case ELEMENT_FRAMEPOINT:
			if (TDraw::PointInFramePoint(pConfig->RealToScreen(pShape->Element[i].dpt), ptPos))
			{
				iPickIndex = i;
			}
			break;
		}
		if (iPickIndex != -1)
		{
			//暂存当前线型并更改
			pShape->Element[iPickIndex].logpenStyleShow.lopnStyle = PS_DOT;

			//通知TreeView选中

			//通知ListView更新
			pShape->Element[iPickIndex].NoticeListView(pListView);
			return;
		}

	}

	//没有拾得

	//通知TreeView取消选中

	//通知ListView更新
	pListView->DeleteAllItems();
}

void TSelectTool::OnRButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{

}

//由祖先插入WM_PAINT事件中进行绘制
void TSelectTool::Draw(HDC hdc)
{
	if (iPickIndex != -1)
	{
		switch (pShape->Element[iPickIndex].eType)
		{
		case ELEMENT_REALLINE:
			//画拾取方格
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(pShape->Element[iPickIndex].ptBegin));
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(pShape->Element[iPickIndex].ptEnd));
			//由于线型变化，且画线位于工具绘制之前，所以需要刷新一次
			::InvalidateRect(pCanvas->m_hWnd, &(pCanvas->ClientRect), FALSE);
			break;

		case ELEMENT_FRAMEPOINT:
			//画拾取方格
			TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(pShape->Element[iPickIndex].dpt));
			//由于线型变化，且画线位于工具绘制之前，所以需要刷新一次
			::InvalidateRect(pCanvas->m_hWnd, &(pCanvas->ClientRect), FALSE);
			break;
		}

	}
}