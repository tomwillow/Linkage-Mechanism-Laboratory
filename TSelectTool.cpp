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
	iPickRealLineIndex = -1;
}

//由TTool的虚析构函数重载
TSelectTool::~TSelectTool()
{
	if (iPickRealLineIndex != -1)
	{
		//恢复线型
		pShape->RealLine[iPickRealLineIndex].logpenStyleShow.lopnStyle = pShape->RealLine[iPickRealLineIndex].logpenStyle.lopnStyle;

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
		if (iPickRealLineIndex != -1)
		{
			pShape->DeleteRealLine(iPickRealLineIndex);
			iPickRealLineIndex = -1;
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

bool TSelectTool::PickRealLine(POINT ptPos, TRealLine &RealLine)
{
	POINT pt1 = pConfig->RealToScreen(RealLine.ptBegin);
	POINT pt2 = pConfig->RealToScreen(RealLine.ptEnd);
	double length = TDraw::Distance(pt1, pt2);
	double length1 = TDraw::Distance(ptPos, pt1);
	double length2 = TDraw::Distance(ptPos, pt2);

	if (length1 + length2 - length <=0.5)//容差
		return true;
	else
		return false;
}

void TSelectTool::OnLButtonDown(HWND hWnd, UINT nFlags, POINT ptPos)
{
	//若之前已经拾取得到线，则恢复线型。（若再次拾取下面将变更线型，若不是则线型将在此恢复）
	if (iPickRealLineIndex != -1)
	{
		pShape->RealLine[iPickRealLineIndex].logpenStyleShow.lopnStyle = pShape->RealLine[iPickRealLineIndex].logpenStyle.lopnStyle;
	}

	//遍历所有线
	for (int i = 0; i < pShape->RealLine.size(); i++)
	{
		if (PickRealLine(ptPos,pShape->RealLine[i]))//发现拾取
		{
			iPickRealLineIndex = i;

			//暂存当前线型并更改
			pShape->RealLine[iPickRealLineIndex].logpenStyleShow.lopnStyle = PS_DOT;

			//通知TreeView选中

			//通知ListView更新
			pShape->RealLine[iPickRealLineIndex].NoticeListView(pListView);
			return;
		}
	}

	for (int i = 0; i < pShape->FramePoint.size(); i++)
	{
		if (TDraw::PointInFramePoint(pConfig->RealToScreen(pShape->FramePoint[i].dpt), ptPos))
		{
		}
	}

	//没有拾得
	iPickRealLineIndex = -1;

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
	if (iPickRealLineIndex != -1)
	{

		//画拾取方格
		TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(pShape->RealLine[iPickRealLineIndex].ptBegin));
		TDraw::DrawPickSquare(hdc, pConfig->RealToScreen(pShape->RealLine[iPickRealLineIndex].ptEnd));

		//由于线型变化，且画线位于工具绘制之前，所以需要刷新一次
		::InvalidateRect(pCanvas->m_hWnd, &(pCanvas->ClientRect), FALSE);
	}
}