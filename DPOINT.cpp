#include <initializer_list>
#include "DPOINT.h"


DPOINT::DPOINT()
{
	DPOINT::x = 0;
	DPOINT::y = 0;
}

DPOINT::DPOINT(double x, double y)
{
	DPOINT::x = x;
	DPOINT::y = y;
}


DPOINT::~DPOINT()
{
}

DPOINT& DPOINT::operator=(DPOINT &dpt)
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