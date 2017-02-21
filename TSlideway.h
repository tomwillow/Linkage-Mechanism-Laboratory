#pragma once
#include "TRealLine.h"
class TSlideway :
	public TRealLine
{
public:
	int ShadowQuadrant;
	int ShadowLength;
	TSlideway();
	~TSlideway();
	TSlideway& TSlideway::operator=(TRealLine &realline);
};

