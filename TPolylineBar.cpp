#include "tchar_head.h"
#include "MyMath.h"
#include "TPolylineBar.h"

#include "TListView.h"

TPolylineBar::TPolylineBar()
{
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_POLYLINEBAR;
}


TPolylineBar::~TPolylineBar()
{
}

void TPolylineBar::NoticeListView(TListView *pListView)
{
	TElement::NoticeListView(pListView);

	TCHAR buffer[16];

	pListView->AddAttributeItem(TEXT("Ô­µã"), CTRLTYPE_COOR_EDIT, &dpt, TEXT("%.3f,%.3f"), dpt.x, dpt.y);
	pListView->AddAttributeItem(TEXT("½Ç¶È"), CTRLTYPE_ANGLE_VALUE_EDIT, &angle, TEXT("%f"), REG2DEG(angle));

	for (size_t i = 0; i < vecDpt.size(); ++i)
	{
		_stprintf(buffer, TEXT("P%d"), i);
		pListView->AddAttributeItem(buffer, CTRLTYPE_COOR_EDIT, &(vecDpt[i]), TEXT("%.3f,%.3f"), vecDpt[i].x, vecDpt[i].y);
	}
}