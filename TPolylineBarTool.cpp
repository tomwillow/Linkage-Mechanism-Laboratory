
#include "TPolylineBarTool.h"
#include "TAttach.h"

TPolylineBarTool::TPolylineBarTool()
{
	pAttach = new TAttach(pCanvas, pShape, pConfig);
}


TPolylineBarTool::~TPolylineBarTool()
{
	delete pAttach;
}

void TPolylineBarTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos)
{
	::SetCursor(::LoadCursor(NULL, IDC_CROSS));
}

void TPolylineBarTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos)
{
	ptMouse = ptPos;
	pAttach->AttachAll(ptPos);

	switch (dptHit.size())
	{
	case 0:
		break;
	}
}