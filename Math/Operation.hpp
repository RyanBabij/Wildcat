#ifndef MATH_OPERATION_HPP
#define MATH_OPERATION_HPP

#include <math.h>

class Operation
{
	public:
	static double round(double number)
	{
		if (number - floor(number) < 0.5)
		{
			number = floor(number);
		}
		else
		{
			number = ceil(number);
		}
		return number;
	}
	static bool isPowerOf2(int x)
	{
		return ( (x > 0) && ((x & (x - 1)) == 0) );
	}
	static unsigned int nearestPowerOf2(unsigned int value)
	{
		while(!isPowerOf2(value))
		{
			value++;
		}
		return value;
	}
	static int findManhattanDistance(const int x1, const int y1, const int x2, const int y2 )
	/* I think I found a bug, but I'm not sure. Old code is commented out. */
	{ return abs(x1-x2)+abs(y1-y2); }
	//{ return abs(x1-x2)+abs(y1-y1); }
	
	static bool isEven ( int n )
	{
		if ( n%2 == 0 )
		{ return true;
		} return false;
	}
};

#endif
