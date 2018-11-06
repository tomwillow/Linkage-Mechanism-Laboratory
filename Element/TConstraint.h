#pragma once
#include "..\Element\TElement.h"
class TConstraint :
	public TElement
{
public:
	TElement *pElement[2];
public:
	TConstraint();
	virtual ~TConstraint();
	virtual bool TConstraint::WriteFile(HANDLE &hf, DWORD &now_pos)override;
	virtual bool TConstraint::ReadFile(HANDLE &hf, DWORD &now_pos, TShape *pShape)override;
};

