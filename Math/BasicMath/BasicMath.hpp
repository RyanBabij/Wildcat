#pragma once

/* Basic math #include <Math/BasicMath/BasicMath.hpp>

Library of basic math functions that don't have external dependencies except math.h.

Cos, tan, sin.
Radians, degrees.
Rounding.
Powers.
Distances (manhattan and real).
Slopes.

*/



#include <math.h>

/* NOTE: All decimal numbers should use double datatype. */

class BasicMath
{
	public:

		// POWER OF TWO STUFF
	template <class BasicMath_T>
	static bool isPowerOfTwo (const BasicMath_T _value)
	{
		if ( _value<0 ) { return false; }

		return !(_value == 0) && !(_value & (_value - 1));
	}

	template <class BasicMath_T>
	static BasicMath_T roundUpPowerOfTwo (const BasicMath_T _value)
	{
		BasicMath_T _ret = 1;
		while (_ret < _value) _ret <<= 1;

		return _ret;

	}

	/* Basic constants and conversions. */
	static double convertRadiansToDegrees(const double radians)
	{ return radians*57.2957914; }
	static double convertDegreesToRadians (const double degrees)
	{ return degrees*0.0174532889; }

	/* cmath wrappers. We assume doubles are being used for decimal numbers. */


	/* Makes a negative value positive. */
	static int absolute(int value)
	{
		if(value<0) { return -value; }
		return value;
	}
	static double absolute (double value)
	{
		if(value<0) { return -value; }
		return value;
	}



	/* Wrappers for sin, cos, tan. */
	static double tanRadians(const double radians)
	{ return tan (radians); }
	static double tanDegrees(const double degrees)
	{ return tan (convertDegreesToRadians(degrees)); }
	static double sinRadians(const double radians)
	{ return sin (radians); }
	static double sinDegrees(const double degrees)
	{ return sin (convertDegreesToRadians(degrees)); }
	static double cosRadians(const double radians)
	{ return cos (radians); }
	static double cosDegrees(const double degrees)
	{ return cos (convertDegreesToRadians(degrees)); }
	static double aTan(const double input)
	{ return atan(input); }

	/* 0213405203 Find angle of the line. Up is 0 degrees. Right is 90 degrees. */
	/* Returns -1 degrees if points are on top of each other. Typically this should be adjusted to 0 degrees by the caller, but it's there just in case. */
	/* 0213405066 NOTE: I don't like this code but it seems to work okay. It seems that the best way to determine the angle of a line is by finding the quadrant... I'm not entirely sure, but it works, anyway. */
	template <class BasicMath_T>
	static double get2DLineAngleDegrees(const BasicMath_T _x1, const BasicMath_T _y1, const BasicMath_T _x2, const BasicMath_T _y2)
	{
		const BasicMath_T xChange = _x2-_x1;
		const BasicMath_T yChange = _y2-_y1;
		const BasicMath_T slope = yChange/xChange;

		if(xChange>0) /* Going to the right. */
		{
			if(yChange>0) /* Top right quadrant. */
			{ return (90-convertRadiansToDegrees(aTan(slope))); }
			else if(yChange<0) /* Bottom right quadrant. */
			{ return (90-convertRadiansToDegrees(atan(yChange/xChange))); }
			else /* Directly right. */
			{ return 90; }
		}
		else if(xChange<0) /* Going to the left. */
		{
			if(yChange>0) /* Top left quadrant. */
			{ return (90-convertRadiansToDegrees(atan(yChange/xChange))+180); }
			else if(yChange<0) /* Bottom left quadrant. */
			{ return (90-convertRadiansToDegrees(atan(yChange/xChange))+180); }
			else
			{ return 270; }
		}
		else /* xChange is 0 */
		{
			if(yChange>0) /* Point is going straight up. */
			{ return 0; }
			else if (yChange<0) /* Point goes straight down. */
			{ return 180; }
			else
			{ return -1; }
		}
		/* Error. */
		return -1;

	}


	static double roundNearestDecimalPlaces(double input, const int decimalPlaces)
	{
		const int multiplier = pow(10,decimalPlaces);
		return floor(input * multiplier + 0.5) / multiplier;
	}

	static int roundOff(double input)
	{
		return round (input);
		//std::cout<<input<<" rounded off to "<<floor(input + 0.5)<<".\n";
		if ( input > 0 )
		{
			return floor(input + 0.5);
		}
		else if (input < 0)
		{
			return round(input - 0.5);
		}
		return 0;
	}

	static double roundNearest(double input)
	{
		/* NOTE: Untested. */
		return floor(input + 0.5);
		/* Old code here. */
		// if (number - floor(number) < 0.5)
		// {
			// number = floor(number);
		// }
		// else
		// {
			// number = ceil(number);
		// }
		// return number;
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
	/* BUG: ? */
	//{ return abs(x1-x2)+abs(y1-y1); }
	{ return abs(x1-x2)+abs(y1-y2); }

	static bool isEven ( int n )
	{
		if ( n%2 == 0 )
		{ return true;
		} return false;
	}

	static double getSlope (const int x1, const int y1, const int x2, const int y2)
	{
		// RETURNS 0 IF NESW.
		if (y2==y1||x2==x1) { return 0; }
		return (double)(y2-y1)/(x2-x1);
	}
  
	static double getSlopeULI (const unsigned long int x1, const unsigned long int y1, const unsigned long int x2, const unsigned long int y2)
	{
		// RETURNS 0 IF NESW.
		if (y2==y1||x2==x1) { return 0; }
		return ((double)y2-y1)/((double)x2-x1);
	}
  
	static double getYIntercept (const double x1, const double y1, const double x2, const double y2)
	{
		double slope = getSlope ( x1, y1, x2, y2 );
		return ( y1 - (slope*x1) );
	}
	static double getYFromLineEquation(double m, double x, double c)
	{
		return m*x+c;
	}
	static double getXFromLineEquation(double y, double m, double c)
	{
		// y = mx + c
		// y-c = mx
		// y-c / m = x
		return (y-c)/m;
	}
};
