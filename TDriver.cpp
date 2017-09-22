#include "TDriver.h"

#include "TListView.h"

TDriver::TDriver()
{
	eType = DRIVER;
	
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

	size_t sLeftSize = sExprLeft.length()+1;
	::WriteFile(hf, &(sLeftSize), sizeof(sLeftSize), &now_pos, NULL);
	now_pos += sizeof(sLeftSize);

	::WriteFile(hf, (sExprLeft.c_str()), sLeftSize*sizeof(TCHAR), &now_pos, NULL);
	now_pos += sLeftSize;

	size_t sRightSize = sExprRight.length()+1;
	::WriteFile(hf, &(sRightSize), sizeof(sRightSize), &now_pos, NULL);
	now_pos += sizeof(sRightSize);

	::WriteFile(hf, (sExprRight.c_str()), sRightSize*sizeof(TCHAR), &now_pos, NULL);
	now_pos += sRightSize;

	if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0)
		return false;
	else
		return true;
}

bool TDriver::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape)
{
	TElement::ReadFile(hf, now_pos, pShape);

	TCHAR *temp;

	size_t sLeftSize;
	::ReadFile(hf, &(sLeftSize), sizeof(sLeftSize), &now_pos, NULL);
	now_pos += sizeof(sLeftSize);
	temp = new TCHAR[sLeftSize];
	::ReadFile(hf, temp, sLeftSize*sizeof(TCHAR), &now_pos, NULL);
	now_pos += sLeftSize;
	sExprLeft = temp;
	delete[] temp;

	size_t sRightSize;
	::ReadFile(hf, &(sRightSize), sizeof(sRightSize), &now_pos, NULL);
	now_pos += sizeof(sRightSize);
	temp = new TCHAR[sRightSize];
	::ReadFile(hf, temp, sRightSize*sizeof(TCHAR), &now_pos, NULL);
	now_pos += sRightSize;
	sExprRight = temp;
	delete[] temp;

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