#include <initializer_list>
#include "DPOINT.h"


DPOINT::DPOINT() :x(0), y(0)
{
}

DPOINT::DPOINT(double x, double y) : x(x), y(y)
{
}


DPOINT::~DPOINT()
{
}

DPOINT& DPOINT::operator=(const DPOINT &dpt)
{
	x = dpt.x;
	y = dpt.y;
	return *this;
}

DPOINT& DPOINT::operator=(std::initializer_list<double> init_list)
{
	if (init_list.size() == 2)
	{
		x = *init_list.begin();
		y = *(init_list.begin() + 1);
	}
	return *this;
}

DPOINT& DPOINT::operator+(DPOINT &dpt)
{
	x += dpt.x;
	y += dpt.y;
	return *this;
}

DPOINT& DPOINT::operator-(DPOINT &dpt)
{
	x -= dpt.x;
	y -= dpt.y;
	return *this;
}

Ofstream& operator<<(Ofstream& out, DPOINT &dpt)
{
	out << dpt.x << dpt.y;
	return out;
}