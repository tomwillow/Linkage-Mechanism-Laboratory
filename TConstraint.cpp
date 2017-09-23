#include "TConstraint.h"
#include "TShape.h"

TConstraint::TConstraint()
{
	pElement[0] = nullptr;
	pElement[1] = nullptr;

}


TConstraint::~TConstraint()
{
}

bool TConstraint::WriteFile(HANDLE &hf, DWORD &now_pos)
{
	if (!TElement::WriteFile(hf, now_pos))
		return false;

	::WriteFile(hf, &(pElement[0]->id), sizeof(pElement[0]->id), &now_pos, NULL);
	now_pos += sizeof(pElement[0]->id);
	::WriteFile(hf, &(pElement[1]->id), sizeof(pElement[1]->id), &now_pos, NULL);
	now_pos += sizeof(pElement[1]->id);

	if (GetLastError() != ERROR_ALREADY_EXISTS && GetLastError() != 0)
		return false;
	else
		return true;
}

bool TConstraint::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape)
{
	if (!TElement::ReadFile(hf, now_pos, pShape))
		return false;

	int id0, id1;
	::ReadFile(hf, &id0, sizeof(id0), &now_pos, NULL);
	now_pos += sizeof(id0);
	::ReadFile(hf, &id1, sizeof(id1), &now_pos, NULL);
	now_pos += sizeof(id1);

	pElement[0] = pShape->GetElementById(id0);
	pElement[1] = pShape->GetElementById(id1);

	if (GetLastError() != 0)
		return false;
	else
		return true;
}