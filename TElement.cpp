#include "TElement.h"

#include "TListView.h"
TElement::TElement()
{
}


TElement::~TElement()
{
}

TCHAR * TElement::GetLineStyleName(UINT linestyle, TCHAR name[])
{
	switch (linestyle)
	{
	case PS_SOLID:
		_tcscpy(name, TEXT("实线"));
		break;
	case PS_DASH:
		_tcscpy(name, TEXT("线段"));
		break;
	case PS_DOT:
		_tcscpy(name, TEXT("虚线"));
		break;
	case PS_DASHDOT:
		_tcscpy(name, TEXT("点划线"));
		break;
	case PS_DASHDOTDOT:
		_tcscpy(name, TEXT("双点划线"));
		break;
	}
	return name;
}