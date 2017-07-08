#include "tchar_head.h"
#include "MyMath.h"
#include "TPolylineBar.h"

#include "TListView.h"

TPolylineBar::TPolylineBar()
{
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_POLYLINEBAR;

	CanBeDragged = true;
}


TPolylineBar::~TPolylineBar()
{
}

const TCHAR * TPolylineBar::GetElementTypeName(TCHAR name[])//得到类型名称
{
	return _tcscpy(name, TEXT("多段杆"));
}

void TPolylineBar::NoticeListView(TListView *pListView)
{
	TElement::NoticeListView(pListView);

	TCHAR buffer[16];

	pListView->AddAttributeItem(TEXT("原点"), CTRLTYPE_COOR_EDIT, &dpt, TEXT("%.3f,%.3f"), dpt.x, dpt.y);
	pListView->AddAttributeItem(TEXT("角度"), CTRLTYPE_ANGLE_VALUE_EDIT, &angle, TEXT("%f"), REG2DEG(angle));

	for (size_t i = 0; i < vecDpt.size(); ++i)
	{
		_stprintf(buffer, TEXT("P%d"), i);
		pListView->AddAttributeItem(buffer, CTRLTYPE_COOR_EDIT, &(vecDpt[i]), TEXT("%.3f,%.3f"), vecDpt[i].x, vecDpt[i].y);
	}
}