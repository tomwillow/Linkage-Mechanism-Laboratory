#pragma once
#include "String.h"
#include <initializer_list>

class DPOINT
{
public:
	double x;
	double y;
	DPOINT();
	~DPOINT();
	DPOINT(double x, double y);

	DPOINT& operator=(const DPOINT &dpt);
	DPOINT& operator=(std::initializer_list<double> init_list);
	DPOINT& operator+(DPOINT &dpt);
	DPOINT& operator-(DPOINT &dpt);
};

Ofstream& operator<<(Ofstream& out, DPOINT &dpt);