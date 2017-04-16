#pragma once
#include <vector>
#include "TTool.h"
#include "DPOINT.h"

class TAttach;
class TPolylineBarTool :
	public TTool
{
private:
	std::vector<DPOINT> dptHit;
	TAttach *pAttach;
public:
	TPolylineBarTool();
	~TPolylineBarTool();
	void TPolylineBarTool::OnSetCursor(HWND hWnd, UINT nFlags, POINT ptPos);
	void TPolylineBarTool::OnMouseMove(HWND hWnd, UINT nFlags, POINT ptPos);
};

