#pragma once
#include "..\Common\MyString.h"
#include <initializer_list>

class DPOINT
{
public:
	double x;
	double y;
	DPOINT();
	~DPOINT();
	DPOINT(double x, double y);

	DPOINT& DPOINT::operator=(const DPOINT &dpt);
	DPOINT& DPOINT::operator=(std::initializer_list<double> init_list);
	DPOINT& DPOINT::operator+(DPOINT &dpt);
	DPOINT& DPOINT::operator-(DPOINT &dpt);
	DPOINT& DPOINT::operator+=(DPOINT &dpt);
	DPOINT& DPOINT::operator-=(DPOINT &dpt);
};

Ofstream& operator<<(Ofstream& out, DPOINT &dpt);