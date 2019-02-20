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
  
  This needs to be converted to template. Setting default of int will make life easier.
  However such objects must still be declared: HasXY <> xy;
  We could probably put geometry functions here like distanceTo etc.
  
*/


//template <class HASXY_T = int>
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
		{ setCoordinates(_x,_y); }
		inline void setXY (HasXY xy)
		{ setCoordinates(xy.x,xy.y); }
		inline void setXY (HasXY* xy)
		{ setCoordinates(xy->x,xy->y); }

	
	// DISTANCE FUNCTION
  
  
};

// Temporary until I feel like converting all instances of HasXY to account for templates.

template <class HASXY_T = int>
class HasXY2
{
	public:
	HASXY_T x;
	HASXY_T y;
	
	HasXY2(const HASXY_T _x = 0, const HASXY_T _y = 0)
	{
		x=_x;
		y=_y;
	}
  
  //OPERATORS
  
  inline bool operator==(const HasXY2& rhs)
  {
    return (x == rhs.x && y == rhs.y);
  }
  inline bool operator>(const HasXY2& rhs)
  {
    return (x+y > rhs.x+rhs.y);
  }
  inline bool operator>=(const HasXY2& rhs)
  {
    return (x+y >= rhs.x+rhs.y);
  }
  inline bool operator<(const HasXY2& rhs)
  {
    return (x+y < rhs.x+rhs.y);
  }
  inline bool operator<=(const HasXY2& rhs)
  {
    return (x+y <= rhs.x+rhs.y);
  }
	
	void setCoordinates(const HASXY_T _x, const HASXY_T _y)
	{
		x=_x;
		y=_y;
	}
		void setXY(HASXY_T _x, HASXY_T _y)
		{ setCoordinates(_x,_y); }
		inline void setXY (HasXY xy)
		{ setCoordinates(xy.x,xy.y); }
		inline void setXY (HasXY* xy)
		{ setCoordinates(xy->x,xy->y); }

	
	// DISTANCE FUNCTION
};


#endif

