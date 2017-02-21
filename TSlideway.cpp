#pragma once
#include <tchar.h>
#include <stdio.h>

#include "TSlideway.h"


TSlideway::TSlideway()
{
	TSlideway::dLength = 0;
	_tcscpy(Name, TEXT(""));
	eType = ELEMENT_SLIDEWAY;

	ShadowQuadrant = 4;
	ShadowLength = 40;
}


TSlideway::~TSlideway()
{
}

TSlideway& TSlideway::operator=(TRealLine &realline)
{
	this->id = realline.id;
	this->available = realline.available;
	this->angle = realline.angle;
	this->dAngle = realline.dAngle;
	this->dLength = realline.dLength;
	this->ptBegin = realline.dpt;
	this->ptEnd = realline.ptEnd;
	this->logpenStyle = realline.logpenStyle;
	this->logpenStyleShow = realline.logpenStyleShow;
	return *this;
}