#include "..\Element\TDriver.h"

#include "..\Control\TListView.h"

TDriver::TDriver()
{
	eType = DRIVER;
	eClass = ELEMENT_CLASS_DRIVER;
	
}


TDriver::~TDriver()
{
}

const String TDriver::GetElementTypeName()//得到类型名称
{
	return TEXT("驱动");
}

bool TDriver::WriteFile(HANDLE &hf, DWORD &now_pos)
{
	TElement::WriteFile(hf, now_pos);

	WriteFileString(hf, sExprLeft, now_pos);
	WriteFileString(hf, sExprRight, now_pos);

	if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0)
		return false;
	else
		return true;
}

bool TDriver::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape)
{
	TElement::ReadFile(hf, now_pos, pShape);

	ReadFileString(hf, sExprLeft, now_pos);
	ReadFileString(hf, sExprRight, now_pos);

	if (GetLastError() != 0)
		return false;
	else
		return true;
}

void TDriver::NoticeListView(TListView *pListView)
{
	pListView->DeleteAllItems();

	pListView->id = id;
	pListView->AddAttributeItem(TEXT("ID"), CTRLTYPE_NULL, NULL, TEXT("%d"), id);
	pListView->AddAttributeItem(TEXT("Left"), CTRLTYPE_EDIT, NULL, sExprLeft.c_str());
	pListView->AddAttributeItem(TEXT("Right"), CTRLTYPE_EDIT, NULL, sExprRight.c_str());
}