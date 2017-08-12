#pragma once
#ifndef WILDCAT_INTERFACE_HASXY_HASXY_HPP
#define WILDCAT_INTERFACE_HASXY_HASXY_HPP

	// TRACEABLE CONSOLE OUTPUT MACRO.
#undef FILEID
#define FILEID "WILDCAT_INTERFACE_HASXY_HASXY_HPP"
#undef SAY
#define SAY(x) std::cout << FILEID << ":" << __LINE__ << ":" << x << "\n"

/*
	#include <Interface/HasXY.hpp>
	This class is able to return an x and y. You can implement it or simply use this class to make a coordinate object.
*/

class HasXY
{
	public:
	int x;
	int y;
	
	HasXY(const int _x=0, const int _y=0)
	{
		x=_x;
		y=_y;
	}
	
	void setCoordinates(const int _x, const int _y)
	{
		x=_x;
		y=_y;
	}
	void setXY(const int _x, const int _y)
	{
		x=_x;
		y=_y;
	}
	
	// DISTANCE FUNCTION
};


#endif
