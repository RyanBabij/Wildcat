#pragma once

/* #include <Math/Geometry/Geometry.hpp>
Math dealing with points and stuff.

2D points
2D lines
Rotation
Shape: Square
Shape
2D Quad/quadrilateral

Math Vector

*/

#include <iostream>
#include <cmath>

#include <Math/BasicMath/BasicMath.hpp>
#include <Container/Vector/Vector.hpp>

class GeometryFunctions
{
	public:
	/* DEPRECATED: Moved to BasicMath.hpp */
	static double convertRadiansToDegrees(const double radians)
	{ return radians*57.2957914; }
	/* DEPRECATED: Moved to BasicMath.hpp */
	static double convertDegreesToRadians (const double degrees)
	{ return degrees*0.0174532889; }
	
};


template <class T>
class Point2D
{
	public:
	T x,y;
	Point2D (const T _x, const T _y)
	{
		x=_x; y=_y;
	}
	Point2D ()
	{ x=0; y=0; }
	void set (const T _x, const T _y)
	{
		x=_x; y=_y;
	}
	void set (const Point2D <T> p)
	{
		x=p.x;
		y=p.y;
	}
	
	
	void print()
	{
		std::cout<<"("<<x<<","<<y<<")";
	}
	
	/* Returns the distance from this point to the target point. */
	double distanceTo( Point2D <T> * target )
	{
	
		const T xChange = x - target->x;
		const T yChange = y - target->y;
		
		
		return BasicMath::absolute(sqrt((xChange*xChange)+(yChange*yChange)));
		//return magnitude;
	
	
	}
	
	/* Rotate this point using the passed point as the origin, with degrees for rotation. */
	void rotateThisAboutPointDegrees ( const Point2D <T> * point, const double degrees )
	{ rotateThisAboutPointRadians(point,GeometryFunctions::convertDegreesToRadians(degrees)); }
	/* Rotate this point using the passed point as the origin, with radians for rotation. */
	void rotateThisAboutPointRadians ( const Point2D <T> * point, const double radians )
	{
		/* Calculate offset from origin. */
		/* BUGFIX: Replaced 'const int originX' with 'const double originX'. */
		const double originX = x-point->x;
		const double originY = y-point->y;
		/* Rotate offset about origin. */
		x = (originX*cos(radians))+(originY*sin(radians));
		y = (originY*cos(radians))-(originX*sin(radians));
		/* Move point back to non-origin offset position, keeping rotation. */
		x += point->x;
		y += point->y;
	}
	/* Get slope using this point as first point, and passed point as second point. */
	T getSlope(Point2D* point2)
	{
		return ((point2->y-y)/(point2->x-x));
	}
	
	/* Returns the distance from the point (0,0). */
	T getDistanceFromOrigin()
	{
		return sqrt((x*x)+(y*y));
	}
	
};



template <class T>
class Square
{
	public:
	/* P1 and P3 must be diagonals. P2 and P4 must be diagonals. */
	Point2D <T> p1;
	Point2D <T> p2;
	Point2D <T> p3;
	Point2D <T> p4;
	
	/* Constructor providing 2 points. Square will be upright. */
	Square (const T _x1, const T _y1, const T _x2, const T _y2)
	{
		p1.set(_x1,_y1);
		p2.set(_x2,_y1);
		p3.set(_x2,_y2);
		p4.set(_x1,_y2);
	
	}
	/* Constructor providing 4 points. */
	// Square ()
	// {
	// }
	
	void rotateAboutCenterDegrees(const int rotationDegrees)
	{
		/* Calculate center point of square, using diagonal points. */
		const Point2D <T> centerPoint ((p1.x>=p3.x ? p3.x+((p1.x-p3.x)/2) : p1.x+((p3.x-p1.x)/2)),(p1.y>=p3.y ? p3.y+((p1.y-p3.y)/2) : p1.y+((p3.y-p1.y)/2)));
		/* Rotate each point about the center point. */
		p1.rotateThisAboutPointDegrees(&centerPoint,rotationDegrees);
		p2.rotateThisAboutPointDegrees(&centerPoint,rotationDegrees);
		p3.rotateThisAboutPointDegrees(&centerPoint,rotationDegrees);
		p4.rotateThisAboutPointDegrees(&centerPoint,rotationDegrees);
	}
	
};

template <class T>
class Line2D
{
	public:
	Point2D <T> p1;
	Point2D <T> p2;
	enum { COINCIDENT, PARALLEL, INTERSECTING, NOT_INTERSECTING };
	
	Line2D ( Point2D <T> _p1, Point2D <T> _p2)
	{
		p1.set(_p1);
		p2.set(_p2);
	}
	
	Line2D ( const T x1, const T y1, const T x2, const T y2 )
	{
		p1.set(x1,y1);
		p2.set(x2,y2);
	}
	Line2D () {}
	
	void setCoordinates (const T x1, const T y1, const T x2, const T y2 )
	{
		p1.set(x1,y1);
		p2.set(x2,y2);
	}
	
	bool collidesWith(Line2D <T> * line)
	{
		// if(intersectsWith(line)==NOT_INTERSECTING)
		if(intersectsWith(line)==INTERSECTING)
		{ return true; }
		//return true;
		return false;
	}
	
	int intersectsWith(Line2D <T> * line)
	{
		const float denominator = ((line->p2.y-line->p1.y)*(p2.x-p1.x))-((line->p2.x-line->p1.x)*(p2.y-p1.y));
		const float numerator1 = ((line->p2.x-line->p1.x)*(p1.y-line->p1.y))-((line->p2.y-line->p1.y)*(p1.x-line->p1.x));
		const float numerator2 = ((p2.x-p1.x)*(p1.y-line->p1.y))-((p2.y-p1.y)*(p1.x-line->p1.x));
		
		if ( denominator == 0 )
		{
			if ( numerator1 == 0 && numerator2 == 0 )
			{
				// coincident (they are on top of each other )
				//std::cout<<"Coincident\n";
				return COINCIDENT;
			}
			//std::cout<<"Parallel\n";
			return PARALLEL;
		}
		
		const float ua = numerator1 / denominator;
		const float ub = numerator2 / denominator;
		
		if(ua >= 0 && ua <= 1 && ub >= 0 && ub <= 1)
		{
			// Get the intersection point.
			//intersection.x_ = begin_.x_ + ua*(end_.x_ - begin_.x_);
			//intersection.y_ = begin_.y_ + ua*(end_.y_ - begin_.y_);
			//std::cout<<"Intersecting\n";
			return INTERSECTING;
		}
		//std::cout<<"Not Intersecting\n";
		return NOT_INTERSECTING;
	}
	
	T getXChange()
	{ return (p2.x-p1.x); }
	T getYChange()
	{ return (p2.y-p1.y); }
	
	double getSlope()
	{ return (((double)p2.x-p1.x)/(p2.y-p1.y));}
	/* Note: Needs to be adjusted for quadrants I think. */
	double getAngleRadians()
	{ return atan(getSlope());
	}
	/* Returns -1 degrees if points are on top of each other. */
	double getAngleDegrees()
	{ return BasicMath::get2DLineAngleDegrees <T> (p1.x,p1.y,p2.x,p2.y); }
};

/* Quad2D. A 2D shape with 4 sides. Quad is short for quadrilateral. */
template <class T>
class Quad2D
{
	public:
	
	//T x1,y1,x2,y2,x3,y3,x4,y4;
	Line2D <T> l1,l2,l3,l4;
	
	/* Note, the points should go clockwise. */
	Point2D <T> p1,p2,p3,p4;
	
	
	/* Empty constructor. */
	Quad2D() {}
	/* Constructor taking raw coordinates. */
	/* TODO: Make sure the points are arranged starting from top-left and going clockwise. */
	Quad2D(const T _x1, const T _y1, const T _x2, const T _y2, const T _x3, const T _y3, const T _x4, const T _y4)
	{
		//x1=_x1; y1=_y1; x2=_x2; y2=_y2; x3=_x3; y3=_y3; x4=_x4; y4=_y4;
		l1.setCoordinates(_x1,_y1,_x2,_y2);
		l2.setCoordinates(_x2,_y2,_x3,_y3);
		l3.setCoordinates(_x3,_y3,_x4,_y4);
		l4.setCoordinates(_x4,_y4,_x1,_y1);
		
		p1.set(_x1,_y1);
		p2.set(_x2,_y2);
		p3.set(_x3,_y3);
		p4.set(_x4,_y4);
		
	}
	/* TODO: Constructor taking points, etc. */

	/* TODO: Make sure the points are arranged starting from top-left and going clockwise. */
	void setCoordinates(const T _x1, const T _y1, const T _x2, const T _y2, const T _x3, const T _y3, const T _x4, const T _y4)
	{
		l1.setCoordinates(_x1,_y1,_x2,_y2);
		l2.setCoordinates(_x2,_y2,_x3,_y3);
		l3.setCoordinates(_x3,_y3,_x4,_y4);
		l4.setCoordinates(_x4,_y4,_x1,_y1);
		
		p1.set(_x1,_y1);
		p2.set(_x2,_y2);
		p3.set(_x3,_y3);
		p4.set(_x4,_y4);
		
	}
	/* Check if this quad collides with another quad. This covers both line intersection and overlapping. */
	bool collidesWith(Quad2D <T> * quad)
	{
		if(l1.collidesWith(&quad->l1) || l1.collidesWith(&quad->l2) || l1.collidesWith(&quad->l3) || l1.collidesWith(&quad->l4)
		|| l2.collidesWith(&quad->l1) || l2.collidesWith(&quad->l2) || l2.collidesWith(&quad->l3) || l2.collidesWith(&quad->l4)
		|| l3.collidesWith(&quad->l1) || l3.collidesWith(&quad->l2) || l3.collidesWith(&quad->l3) || l3.collidesWith(&quad->l4)
		|| l4.collidesWith(&quad->l1) || l4.collidesWith(&quad->l2) || l4.collidesWith(&quad->l3) || l4.collidesWith(&quad->l4) )
		{ return true; }
		
		/* Also check if shapes overlap. I'll do this by picking a point from the smaller shape, and then creating a line with a point that is definitely outside of the bigger shape. If this line collides with the bigger shape, then they are overlapping. */
		
		/* Find smaller shape. */
		
		return false;
	}
	
	double getArea()
	{
		// std::cout<<"Points:\n";
		// std::cout<<"p1: ("<<p1.x<<","<<p1.y<<")\n";
		// std::cout<<"p2: ("<<p2.x<<","<<p2.y<<")\n";
		// std::cout<<"p3: ("<<p3.x<<","<<p3.y<<")\n";
		// std::cout<<"p4: ("<<p4.x<<","<<p4.y<<")\n";
	
		// std::cout<<"Stage 1 values:\n";
		// std::cout<<((p1.x*p2.y)-(p1.y*p2.x))<<".\n";
		// std::cout<<((p2.x*p3.y)-(p2.y*p3.x))<<".\n";
		// std::cout<<((p3.x*p4.y)-(p3.y*p4.x))<<".\n";
		// std::cout<<((p4.x*p1.y)-(p4.y*p1.x))<<".\n";
	
		return abs((((p1.x*p2.y)-(p1.y*p2.x))+((p2.x*p3.y)-(p2.y*p3.x))+((p3.x*p4.y)-(p3.y*p4.x))+((p4.x*p1.y)-(p4.y*p1.x)))/2);
		
		
		//abs(((x1*y2)-(y1*x2)+(x2*y3)-(y2*x3)...+(xn*y1)-(yn*x1))/2)
		//return 0;
	}
	
	/* NOTE: Untested... Probably doesn't work. */
	/* Get the point that lies closest to the origin (0,0). */
	/* If several points are equally close to the origin, it returns the first such one that it found. */
	Point2D <T> * getPointClosestToOrigin ()
	{
		/* Distance from origin = sqrt(x^2 + y^2.) */
		
		// double closestDistance;
		Point2D <T> * closestPoint=&p1;
		double closestDistance = p1.getDistanceFromOrigin();
		
		if(p2.getDistanceFromOrigin() < closestDistance)
		{
			closestDistance=p2.getDistanceFromOrigin();
			closestPoint=&p2;
		}
		if(p3.getDistanceFromOrigin() < closestDistance)
		{
			closestDistance=p3.getDistanceFromOrigin();
			closestPoint=&p3;
		}
		if(p4.getDistanceFromOrigin() < closestDistance)
		{
			closestDistance=p4.getDistanceFromOrigin();
			closestPoint=&p4;
		}
		
		
	//	l1
		
		
		return closestPoint;
		
	}
	
	/* Returns coordinates to a point guaranteed to be outside of this quad. Useful for collision detection algorithms. */
	Point2D <T> * getPointOutside()
	{
		T smallestX = p1.x;
		
		if(p2.x<smallestX) { smallestX=p2.x; }
		if(p3.x<smallestX) { smallestX=p3.x; }
		if(p4.x<smallestX) { smallestX=p4.x; }
	
		T smallestY = p1.y;
	
		if(p2.y<smallestY) { smallestY=p2.y; }
		if(p3.y<smallestY) { smallestY=p3.y; }
		if(p4.y<smallestY) { smallestY=p4.y; }
	
		Point2D <T> * p = new Point2D <T> (smallestX-1,smallestY-1);
	
		return p;
	}
	
	// bool isSmallerThan(Quad2D <T> * quad)
	// {
		// if(getArea
		// return false;
	// }
	
	/* Operators: > < >= <= are for area. Ie, if quad1 has smaller area than quad2, then quad1 > quad2 is false. */
	bool operator > (Quad2D <T> & quad)
	{ return (getArea() > quad.getArea()); }
	bool operator < (Quad2D <T> & quad)
	{ return (getArea() < quad.getArea()); }
	bool operator >= (Quad2D <T> & quad)
	{ return (getArea() >= quad.getArea()); }
	bool operator <= (Quad2D <T> & quad)
	{ return (getArea() <= quad.getArea()); }
	
	
	void printCoordinates()
	{
		std::cout<<"("<<p1.x<<","<<p1.y<<") ("<<p2.x<<","<<p2.y<<") ("<<p3.x<<","<<p3.y<<") ("<<p4.x<<","<<p4.y<<")";
	}
	
};


/* Container for all possible 2D shapes, stored as a vector of points. */
template <class T>
class Shape2D
{
	public:
	/* Unrotated points. */
	Vector <Point2D <T> * > vPoint;
	/* Modified points. */
	//Vector <Point2D <T> * > vModifiedPoint;
	
	
	/* Empty the vector of points. */
	void clear()
	{ vPoint.clear(); }
	
	void addPoint(const int _x, const int _y)
	{
		Point2D <T> * point = new Point2D <T> (_x,_y);
		vPoint.push(point);
	}
	Vector <Point2D <T> * > * rotateAboutCenterDegrees(const double degrees)
	{
		/* Find center point by averaging the coordinates. */
		T averageX=0;
		T averageY=0;
		for (int i=0;i<vPoint.size();++i)
		{
			averageX+=vPoint(i)->x;
			averageY+=vPoint(i)->y;
		}
		averageX/=vPoint.size();
		averageY/=vPoint.size();
		
		Point2D <T> centerPoint (averageX,averageY);
		
		/* Copy all points into a new vector, so they can be modified without changing the original points. Rotate the copied points. */
		Vector <Point2D <T> *> * vModifiedPoint = new Vector <Point2D <T> *>;
		
		for(int i=0;i<vPoint.size();++i)
		{
			Point2D <T> * point = new Point2D <T> (vPoint(i)->x,vPoint(i)->y);
			point->rotateThisAboutPointDegrees(&centerPoint,degrees);
			vModifiedPoint->push(point);
		}
		
		return vModifiedPoint;
	}
	void print()
	{
		std::cout<<"Shape2D::print():\n";
		std::cout<<"Printing points.\n";
		for(int i=0;i<vPoint.size();++i)
		{
			std::cout<<"("<<vPoint(i)->x<<","<<vPoint(i)->y<<")\n";
		}
	}
	
};


/* MathVector2D
It is called MathVector instead of Vector because 'Vector' more commonly refers to the STL container class.

Template means that vectors can be made using ints or other non-decimal datatypes. However, getting something like the magnitude of a vector will return a double, since 2 integer points will often have a decimal length. Anyway, making this a template class might be excessive, it would probably be best to just use doubles... Any rounding would probably be better to be done outside of the vector object.

*/
template <class T>
class MathVector2D
{
	public:
	Point2D <T> initialPoint;
	Point2D <T> terminalPoint;
	
	MathVector2D()
	{}
	
	MathVector2D(Point2D <T> p1, Point2D <T> p2)
	{
		initialPoint.set(p1);
		terminalPoint.set(p2);
	}
	
	void print()
	{
		initialPoint.print();
		std::cout<<" -> ";
		terminalPoint.print();
	}
	
	/* Basically flips the points so the vector is turned 180 degrees. */
	void invert()
	{
		const T tempX=initialPoint.x;
		const T tempY=initialPoint.y;
		
		initialPoint.x=terminalPoint.x;
		initialPoint.y=terminalPoint.y;
		terminalPoint.set(tempX,tempY);
	}

	
	void addVector (MathVector2D <T> target)
	{
		terminalPoint.x+=target.xChange();
		terminalPoint.y+=target.yChange();
	}
	void addVector (MathVector2D <T>* target)
	{
		terminalPoint.x+=target->xChange();
		terminalPoint.y+=target->yChange();
	}
	
	/* Turns the vector into a unit vector, ie a vector of length 1. */
	void normalise() { normalize(); }
	void normalize()
	{
	
		std::cout<<"Normalising vector.\n";
		/* Get length. */
		const double l = length();
		
		const double lengthX = BasicMath::absolute(terminalPoint.x-initialPoint.x)/l;
		const double lengthY = BasicMath::absolute(terminalPoint.y-initialPoint.y)/l;
		
		terminalPoint.x=initialPoint.x+lengthX;
		terminalPoint.y=initialPoint.y+lengthY;
	}
	
	void set(const T _x1, const T _y1, const T _x2, const T _y2)
	{
		initialPoint.set(_x1,_y1);
		terminalPoint.set(_x2,_y2);
	}
	
	void rotateDegrees(const double degrees)
	{
		//std::cout<<"Rotate: "<<degrees<<".\n";
		////std::cout<<"Old: "<<initialPoint.x<<","<<initialPoint.y<<","<<terminalPoint.x<<","<<terminalPoint.y<<".\n";
		//terminalPoint.rotateThisAboutPointDegrees(&initialPoint,degrees);
		//std::cout<<"New: "<<initialPoint.x<<","<<initialPoint.y<<","<<terminalPoint.x<<","<<terminalPoint.y<<".\n";
		
		//std::cout<<"Rotating: "<<degrees<<".\n";
		
		double mag = getMagnitude();
		double currentAngle = getAngleDegrees();
		
		set(0,0,0,0);
		//std::cout<<"Setting vector angle from "<<currentAngle<<" to "<<currentAngle+degrees<<".\n";
		addVectorDegrees(mag,currentAngle+degrees);
		//std::cout<<"New angle: "<<getAngleDegrees()<<".\n";
		
		
	}
	
	/* Move the point by the amount specified by the vector. */
	void movePoint(Point2D <T> * point)
	{
		//std::cout<<"Points: "<<initialPoint.x<<","<<initialPoint.y<<".\n";
		//std::cout<<"Points: "<<terminalPoint.x<<","<<terminalPoint.y<<".\n";
		
		const T xDiff = terminalPoint.x - initialPoint.x;
		const T yDiff = terminalPoint.y - initialPoint.y;
		
		//std::cout<<"Diffs: "<<xDiff<<","<<yDiff<<".\n";
		
		point->x += xDiff;
		point->y += yDiff;
	}
	void addVectorDegrees(const T magnitude, const double degrees)
	{
		/* The first point is set at the origin. The second point is (x,y). */
		/* I thought sin was for y and cos was for x... There's probably something wrong here, or downstream... */
		const T x = magnitude*BasicMath::sinDegrees(degrees);
		const T y = magnitude*BasicMath::cosDegrees(degrees);
		/* Adding the vector is now simple. */
		terminalPoint.x+=x;
		terminalPoint.y+=y;
	}
	
	/* Get the slope of the vector. */
	double getSlope()
	{ return initialPoint.getSlope(&terminalPoint); }

	/* Returns the length of a vector. */
	double length()
	{ return getMagnitude(); }
	/* Get the magnitude of the vector, by finding the distance between the 2 points. Value is always positive. */
	double getMagnitude()
	{
		const double xLength = terminalPoint.x - initialPoint.x;
		const double yLength = terminalPoint.y - initialPoint.y;
		const double magnitude=BasicMath::absolute(sqrt((xLength*xLength)+(yLength*yLength)));
		return magnitude;
	}
	/* Get the angle of the vector in degrees. 0 degrees is facing up. 90 degrees is facing directly right. */
	double getAngleDegrees()
	{ return BasicMath::get2DLineAngleDegrees <T> (initialPoint.x,initialPoint.y,terminalPoint.x,terminalPoint.y); }
	
	double xChange()
	{ return getXChange(); }
	double yChange()
	{ return getYChange(); }
	
	double getXChange()
	{ return terminalPoint.x-initialPoint.x; }
	double getYChange()
	{ return terminalPoint.y-initialPoint.y; }
	
	
	/* TODO: Merge reduceMagnitude and increaseMagnitude. */
	
	/* Reduce the magnitude of the vector by a specified amount. The magnitude will not go below 0. */
	void reduceMagnitude(const T amount)
	{
		/* Get line length. */
		// const T xLength = terminalPoint.x - initialPoint.x;
		// const T yLength = terminalPoint.y - initialPoint.y;
		
		// const int vectorMagnitude = sqrt((xLength*xLength)+(yLength*yLength));
		const double vectorMagnitude = getMagnitude();
		
		//std::cout<<"Current magnitude: "<<vectorMagnitude<<".\n";
		
		/* if the magnitude reduction is greater than the current magnitude, the make the vector 0 magnitude. */
		if(vectorMagnitude<=amount)
		{
			terminalPoint.x = initialPoint.x;
			terminalPoint.y = initialPoint.y;
		}
		else
		{
			/* Get angle of vector. */
			
			//const double slope = getSlope();
			//std::cout<<"Vector slope: "<<slope<<".\n";
			
			//const double angleRadians = BasicMath::aTan(getSlope());
			
			//const double angleDegrees = BasicMath::convertRadiansToDegrees(angleRadians);
			
			const double angleDegrees = BasicMath::get2DLineAngleDegrees <double> (initialPoint.x,initialPoint.y,terminalPoint.x,terminalPoint.y);
			
			//std::cout<<"Vector angle: "<<angleDegrees<<".\n";
			
			//std::cout<<"Current magnitude: "<<vectorMagnitude<<".\n";
			
			/* Make new vector with same angle but reduced magnitude. */
			
			const double newMagnitude = vectorMagnitude-amount;
			
			
			//std::cout<<"Reducing magnitude to: "<<newMagnitude<<".\n";
			//std::cout<<"New magnitude: "<<newMagnitude<<".\n";
			/* Make vector 0 magnitude. */
			terminalPoint.x = initialPoint.x;
			terminalPoint.y = initialPoint.y;
			/* Add new magnitude at same angle. */
			addVectorDegrees(newMagnitude,angleDegrees);
			
			/* Recalc magnitude. */
			
			
		}

		
	}
	
	void increaseMagnitude(const double amount)
	{
		const double vectorMagnitude = getMagnitude();
		/* We can't increase the magnitude of a 0 magnitude vector, since we don't know what direction to go in. */
		if(vectorMagnitude==0)
		{ return; }
		
		const double angleDegrees = BasicMath::get2DLineAngleDegrees <double> (initialPoint.x,initialPoint.y,terminalPoint.x,terminalPoint.y);
			
		//std::cout<<"Vector angle: "<<angleDegrees<<".\n";
		
		//std::cout<<"Current magnitude: "<<vectorMagnitude<<".\n";
		
		/* Make new vector with same angle but reduced magnitude. */
		
		const double newMagnitude = vectorMagnitude+amount;
		
		
		//std::cout<<"Reducing magnitude to: "<<newMagnitude<<".\n";
		//std::cout<<"New magnitude: "<<newMagnitude<<".\n";
		/* Make vector 0 magnitude. */
		terminalPoint.x = initialPoint.x;
		terminalPoint.y = initialPoint.y;
		/* Add new magnitude at same angle. */
		addVectorDegrees(newMagnitude,angleDegrees);
		
	}
	

	void deepCopy(MathVector2D <T> * target)
	{
		initialPoint.x=target->initialPoint.x;
		initialPoint.y=target->initialPoint.y;
		
		terminalPoint.x=target->terminalPoint.x;
		terminalPoint.y=target->terminalPoint.y;
	}
	
	
};
















/* Deprecated */

/* Replaced by template class, Point2D */
class Pointfloat
{
	public:
	float x, y;
	
	Pointfloat () {}
	Pointfloat ( const float x2, const float y2 )
	{ x=x2; y=y2; }
};
/* Replaced by template class, Point2D */
class PointInt
{
	public:
	int x,y;
	PointInt() {}
	PointInt(const int _x, const int _y)
	{ x=_x; y=_y; }
	
	//int rotateAboutOriginRadians(double radians)
	//{
	//}
	//int rotateAboutOriginDegrees(double degrees)
	//{
	///}
};
/* Replaced by template class, Point2D */
class PointDouble
{
	public:
	double x,y;
	PointDouble() {}
	PointDouble(const double _x, const double _y)
	{ x=_x; y=_y; }
	
	//int rotateAboutOriginRadians(double radians)
	//{
	//}
	//int rotateAboutOriginDegrees(double degrees)
	//{
	///}
	
	PointDouble* rotateAboutPointRadians ( PointDouble* point, double radians )
	{
		PointDouble* newPoint = new PointDouble;
		
		int originX = point->x-x;
		int originY = point->y-y;
		
		int newX, newY;
		
		newX = (originX*cos(radians))-(originY*sin(radians));
		newY = (originX*sin(radians))-(originY*cos(radians));
		
		newX += point->x;
		newY += point->y;
		
		newPoint->x = newX;
		newPoint->y = newY;
		
		return newPoint;
	}
	void rotateThisAboutPointDegrees ( PointDouble* point, const double degrees )
	{ rotateThisAboutPointRadians(point,degrees*0.0174532889); }
	
	void rotateThisAboutPointRadians ( PointDouble* point, const double radians )
	{
		//std::cout<<"Rotation at radians: "<<radians<<".\n";
	
	//	std::cout<<"This point: "<<x<<","<<y<<".\n";
		//std::cout<<"Origin: "<<point->x<<","<<point->y<<".\n";
		//int originX = point->x-x;
		//int originY = point->y-y;
		const int originX = x-point->x;
		const int originY = y-point->y;
		
		//std::cout<<"Point relative to origin: "<<originX<<", "<<originY<<".\n";
		//std::cout<<"Rotating: "<<radians<<" radians.\n";
		
		//int newX, newY;
		
		/* This is probably bad. I just messed around till it worked. */
		x = (originX*cos(radians))+(originY*sin(radians));
		y = (originY*cos(radians))-(originX*sin(radians));
		
		//std::cout<<"Point after rotation around origin: " <<x<<", "<<y<<".\n";
		
		x += point->x;
		y += point->y;
		
		
	//	std::cout<<"Distance is: "<<sqrt(pow(point->x-x,2)+pow(point->y-y,2))<<".\n";
		
		//newPoint->x = newX;
		//newPoint->y = newY;
		
		//return newPoint;
	}
	
	// double distanceToPoint(PointDouble* point)
	// {
		
		
	// }
	
	// void rotateThisAboutPointDegrees ( PointDouble* point, double radians )
	// {
		// int originX = point->x-x;
		// int originY = point->y-y;
		
		// std::cout<<"Point relative to origin: "<<originX<<", "<<originY<<".\n";
		// std::cout<<"Rotating: "<<radians<<" radians.\n";
		
		// //int newX, newY;
		
		// x = (originX*cos(radians))-(originY*sin(radians));
		// y = (originY*cos(radians))-(originX*sin(radians));
		
		// std::cout<<"Point after rotation around origin: " <<x<<", "<<y<<".\n";
		
		// x += point->x;
		// y += point->y;
		
		// //newPoint->x = newX;
		// //newPoint->y = newY;
		
		// //return newPoint;
	// }
	
};


/*
int main(int nArgs, char** arg)
{
   Line l1 (5,6,5,4);
   Line l2 (5,3,6,6);
   std::cout<<l1.intersectsDiscrete (l2)<<"\n";
}
*/

