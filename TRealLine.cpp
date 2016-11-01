#include "TRealLine.h"


TRealLine::TRealLine()
{
	TRealLine::dLength = 0;
}

TRealLine::~TRealLine()
{

}

void TRealLine::CalcLength()
{
	dLength = sqrt(pow(double(ptEnd.x - ptBegin.x), 2) + pow(double(ptEnd.y - ptBegin.y), 2));
}

void TRealLine::SetPoint(DPOINT ptBegin, DPOINT ptEnd)
{
	TRealLine::ptBegin = ptBegin;
	TRealLine::ptEnd = ptEnd;
}

void TRealLine::SetPoint(DPOINT ptBegin, double dLength, double dAngle)
{
	TRealLine::ptBegin = ptBegin;
	TRealLine::ptEnd.x = ptBegin.x+dLength*cos(dAngle);
	TRealLine::ptEnd.y = ptBegin.y+dLength*sin(dAngle);
}