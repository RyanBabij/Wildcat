#pragma once
#ifndef WILDCAT_CONTAINER_ARRAYS2_ARRAYS2_HPP
#define WILDCAT_CONTAINER_ARRAYS2_ARRAYS2_HPP

/*
	#include <Container/ArrayS2/ArrayS2.hpp>

	My implementation of a 2D array. This header contains just the basic functions.

	0230223265
	Moving all non-basic stuff out of core header. Did cpp/hpp thingy. Tidied things up.

	0252764426
	Note that HasXY may end up being replaced with tuples... In this case, std::pair.
	
	
	NOTES:
	
	How to initialise null value for an array of non-pointer objects.
	
		ArrayS2 <Obj> aObj;
		Obj nullValue;
		aObj.init(100,100,nullValue);
	
*/

#include <string>
#include <iostream>
#include <Container/Vector/Vector.hpp>
#include <Data/DataTools.hpp> // Swap()

//#define CONTAINER_ARRAYS2_DEBUG

/* Debug macro */
#ifdef CONTAINER_ARRAYS2_DEBUG
#include <iostream>
#undef debug
#define debug std::cout<<
#else
#undef debug
#define debug //
#endif

#include <iostream>
#include <math.h> // For getClosestDistanceTrue

// USED TO RECIEVE AND PASS ARRAY INDEXES.
#include <Interface/HasXY.hpp>

template <class ARRAYS2_T>
class ArrayS2
{
	private:
	public:

	ARRAYS2_T * data;
	ARRAYS2_T nullValue;
	ARRAYS2_T * nullAddress;
	ARRAYS2_T * currentElement;
	ARRAYS2_T outOfBounds;
	bool overFlow;
	int nX,nY;

	int neighborNumber;

	std::string name;

		// DESTRUCTOR
	~ArrayS2();

		// CONSTRUCTORS AND INITIALISATION
	ArrayS2();
	ArrayS2(const unsigned int _nX, const unsigned int _nY, const ARRAYS2_T _nullValue);
		// DEEP COPY CONSTRUCTOR
	ArrayS2 (const ArrayS2 <ARRAYS2_T> &array);
		// EXPLICIT INITIALISATION
	void init(const unsigned int x, const unsigned int y, const ARRAYS2_T _nullValue);
		// When initializing without pointers, we can't pass a default address.
	void initClass(const unsigned int x, const unsigned int y);

		// SET FUNCTIONS
		// 0223626497
		// Only sets value if it is safe, otherwise it does nothing.
	void safeSet(const int _x, const int _y, const ARRAYS2_T _value);

		// DEEP COPY ASSIGNMENT OPERATOR
	//void operator = (const ArrayS2 <T> &array);
	void operator = (const ArrayS2 <ARRAYS2_T> &array)
	{
		nX=array.nX;
		nY=array.nY;

		int memSize=array.nullAddress-array.data;
		delete [] data;
		data=new ARRAYS2_T[memSize];
		nullAddress=data+memSize;
		nullValue=array.nullValue;

		ARRAYS2_T * i = data;
		ARRAYS2_T * j = array.data;
		while(i!=nullAddress)
		{
			*i=*j;
			i++;
			j++;
		}
	}

	
		/* ITERATORS
			You can loop through a vector easily using:
			
			for(auto const& value: vChildren) { std::cout<<"Person: "<<value->firstName<<".\n"; }
			
			Todo: Test this for ArrayS2
		
		*/
	
	ARRAYS2_T* begin()
	{
		return &data[0];
	}
	ARRAYS2_T* end()
	{
		return &data[nX*nY];
	}


template<class Function>
  Function for_each(Function fn)
{
	const int ARRAY_SIZE = nX*nY;
	for ( int i=0;i<ARRAY_SIZE;++i)
	{ fn (i);
	}



  return fn;      // or, since C++11: return move(fn);
}

// FOREACH MACRO
#define FOR_EACH(a,x) const int _ARRAYSIZE=a.nX*a.nY; for (int _i=0;_i<_ARRAYSIZE;++_i){ x; }

	// HOW TO USE:
	// FOR_EACH(aTest,
	// if(Random::oneIn(5)==true)
	// {
		// aTest[_i]=69;
	// }
	// );

	// 0230223405 UPDATED TO PREVENT DATATYPE SIZE OVERRUNS.
	ARRAYS2_T averageValue(const int skipSize=1);

	ArrayS2<ARRAYS2_T>* resizeTo(const int _x, const int _y);

	// NOTE: This algorithm uses a square searching algorithm. This is inaccurate if you're not operating under
	// the assumption that diagonal movement is equivalent to sideways movement.
	int getClosestDistanceManhattan(const int X, const int Y, const char EXPRESSION, const ARRAYS2_T COMPARISON_VALUE, const int SEARCH_RADIUS);

	double getClosestDistanceTrue(const int X, const int Y, const char EXPRESSION, const ARRAYS2_T COMPARISON_VALUE, const int SEARCH_RADIUS);

	void replaceAll(const ARRAYS2_T find, const ARRAYS2_T replace);

// Search
	bool contains(ARRAYS2_T value);

	inline bool testExpression (const ARRAYS2_T V1, const char EXPRESSION, const ARRAYS2_T V2);

	ArrayS2 sub (unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2);

		// RETURNS REQUESTED ELEMENT BY ADDRESS, SO IT CAN BE MODIFIED.
	//inline T& operator() (const int x, const int y);
		// SAME AS ABOVE, BUT USES HASXY OBJECTS.
	//inline T& operator() (HasXY* _objectWithXY);
		// ACCEPTS A SINGLE VALUE INSTEAD OF THE STANDARD 2 VALUES. THIS MIGHT BE FASTER... I DUNNO.
	//inline T& operator() (const int _index);
	
	
//SET FUNCTIONS
		// SAME AS THE BRACKETS OPERATOR. USEFUL FOR INTERNAL SETTING.
	void set (const int _x, const int _y, const ARRAYS2_T _value);
	


	inline ARRAYS2_T& operator () (const int x, const int y)
	{ return data[nX*y+x]; }
	inline ARRAYS2_T& operator () (HasXY _objectWithXY)
	{ return data[nX*_objectWithXY.y+_objectWithXY.x]; }
	inline ARRAYS2_T& operator () (HasXY* _objectWithXY)
	{ return data[nX*_objectWithXY->y+_objectWithXY->x]; }
	inline ARRAYS2_T& operator () (const int _index)
	{ return data[_index]; }
	
	


		// BOUNDS CHECKING.
	//bool isSafe(const int _x, const int _y);
	bool isSafe(const int _x, const int _y);
		bool isSafe(HasXY* _objectWithXY);


		// CONVERT 1 DIMENSIONAL INDEX TO 2 DIMENSIONAL COORDS.
	inline void getIndexCoords(const int _index, int* _returnX, int* _returnY);



// inline T *index(const unsigned int x, const unsigned int y)
// { return &data[nX*y+x]; }
// inline T (*this)(const unsigned int x, const unsigned int y)
// { return data[nX*y+x]; }



// inline T *safeIndex(const int x, const int y)
// {
	// if(x<nX && y<nY && x>=0 && y>=0)
	// {
		// return &data[nX*y+x];
	// }
	// return 0;
// }

/* ArrayS2_Fill.hpp
	Algorithms for filling the array.
*/

	/* If data[i] is 'OP' comparisonValue, then fill. */
	void fillConditional(const char op, ARRAYS2_T comparisonValue, ARRAYS2_T fillValue)
	{
		currentElement=&data[0];
		while(currentElement!=nullAddress)
		{
			if(op=='=' && *currentElement == comparisonValue )
			{ *currentElement=fillValue; }
			else if(op=='<' && *currentElement < comparisonValue )
			{ *currentElement=fillValue; }
			else if(op=='>' && *currentElement > comparisonValue )
			{ *currentElement=fillValue; }
			else if(op=='!' && *currentElement != comparisonValue )
			{ *currentElement=fillValue; }


			++currentElement;
		}
		currentElement=&data[0];
	}

// 0230233259
// Old fillborder did not accept args, but filled the entire array border.
// I have updated it to fill the entire array border by default, but to
// also allow coordinates to be passed.
// THE COORDS ARE INCLUSIVE.

void fillBorder ( const ARRAYS2_T _value, const int _startX=0, const int _startY=0, const int _endX=-1, const int _endY=-1 );


/* ArrayS2_Neighbours.hpp
	Deals with functions that handle neighbors, as well as directions.
	These are basically a simplistic form of relative coordinates. */

	/* SECTION: Neighbours */


// bool includeSelf_;

// inline ArrayS2<T>& INCLUDE_SELF()
// { includeSelf_ = true; return *this; }

// void testicles() : includeSelf_ (false)
// {
// }

	// unsigned char INCLUDE_SELF; // 00000001

	// void setNeighbors2(const int _x, const int _y, const T _value, const int size, unsigned char params)
	// {
		// if ( params & INCLUDE_SELF  == INCLUDE_SELF )
		// {
			// std::cout<<"INCLUDE_SELF\n";
		// }

	// }

void setNeighbors(const int _x, const int _y, const ARRAYS2_T _value, const int size, bool _includeSelf)
{
	for ( int _y2=_y-size;_y2<=_y+size;++_y2)
	{
		for ( int _x2=_x-size;_x2<=_x+size;++_x2)
		{
			if ( isSafe(_x2,_y2) )
			{
				//if ( (_x2!=_x && _y2!=_y) || (_includeSelf==true && _x2==_x && _y2==_y) )
				if ( _includeSelf==true || _x2!=_x || _y2!=_y )
				{
					(*this)(_x2,_y2) = _value;
				}
			}
		}
	}
}

	// RETURN A VECTOR OF INDEXES SURROUNDING (AND POSSIBLY INCLUDING) THE PASSED COORDINATES.
Vector <HasXY*> * getNeighbors(const int _x, const int _y, const bool _includeSelf=false)
{
	Vector <HasXY*> * vectorIndex = new Vector <HasXY*>;

	if (_includeSelf==true && isSafe(_x,_y))
	{ vectorIndex->push(new HasXY(_x,_y)); }

	if(isSafe(_x-1,_y-1))
	{ vectorIndex->push(new HasXY(_x-1,_y-1)); }
	if(isSafe(_x-1,_y))
	{ vectorIndex->push(new HasXY(_x-1,_y)); }
	if(isSafe(_x-1,_y+1))
	{ vectorIndex->push(new HasXY(_x-1,_y+1)); }
	if(isSafe(_x,_y-1))
	{ vectorIndex->push(new HasXY(_x,_y-1)); }
	if(isSafe(_x,_y+1))
	{ vectorIndex->push(new HasXY(_x,_y+1)); }
	if(isSafe(_x+1,_y-1))
	{ vectorIndex->push(new HasXY(_x+1,_y-1)); }
	if(isSafe(_x+1,_y))
	{ vectorIndex->push(new HasXY(_x+1,_y)); }
	if(isSafe(_x+1,_y+1))
	{ vectorIndex->push(new HasXY(_x+1,_y+1)); }

	return vectorIndex;
}
Vector <HasXY*> * getNeighbors( HasXY* _index, const bool _includeSelf=false)
{ return getNeighbors(_index->x, _index->y, _includeSelf); }

	// 0241794590: APPARENTLY THIS NEVER EXISTED BEFORE NOW. MAYBE MAKE THIS THE NEW STANDARD FUNCTION.
	// ALSO THIS LOOKS TO BE A HUGE MESS.
Vector <HasXY*> * getNeighbors2(const int _x, const int _y, const int radius, const bool _includeSelf=false)
{
	Vector <HasXY*> * vectorIndex = new Vector <HasXY*>;

	for(int currentX=_x-radius;currentX<=_x+radius;++currentX)
	{
		//int currentY=0;
		for(int currentY=_y-radius;currentY<=_y+radius;++currentY)
		{
			if (isSafe(currentX,currentY))
			{
				vectorIndex->push(new HasXY(currentX,currentY));
			}
		}
		//currentY=_y-radius;
	}
	return vectorIndex;
}

ARRAYS2_T getNeighboursTotal(const int _x, const int _y, const bool _includeSelf=false)
{
	Vector <ARRAYS2_T> * vNeighbours = getNeighborValues(_x,_y,_includeSelf);


	ARRAYS2_T total=0;
	for ( int i=0;i<vNeighbours->size();++i)
	{
		total+=(*vNeighbours)(i);
	}

	return total;
}

	// RETURNS A VECTOR OF VALUES RATHER THAN INDEXES. USEFUL FOR SIMPLER ARRAYS, LIKE AN ARRAY OF NUMBERS.
Vector <ARRAYS2_T>* getNeighborValues(const int _x, const int _y, const bool _includeSelf=false)
{
	Vector <ARRAYS2_T> * vValues = new Vector <ARRAYS2_T>;

	if (_includeSelf==true && isSafe(_x,_y))
	{ vValues->push((*this)(_x,_y)); }

	if(isSafe(_x-1,_y-1))
	{ vValues->push((*this)(_x-1,_y-1)); }
	if(isSafe(_x-1,_y))
	{ vValues->push((*this)(_x-1,_y)); }
	if(isSafe(_x-1,_y+1))
	{ vValues->push((*this)(_x-1,_y+1)); }
	if(isSafe(_x,_y-1))
	{ vValues->push((*this)(_x,_y-1)); }
	if(isSafe(_x,_y+1))
	{ vValues->push((*this)(_x,_y+1)); }
	if(isSafe(_x+1,_y-1))
	{ vValues->push((*this)(_x+1,_y-1)); }
	if(isSafe(_x+1,_y))
	{ vValues->push((*this)(_x+1,_y)); }
	if(isSafe(_x+1,_y+1))
	{ vValues->push((*this)(_x+1,_y+1)); }

	return vValues;
}
Vector <ARRAYS2_T> * getNeighborValues( HasXY* _index, const bool _includeSelf=false)
{ return getNeighborValues(_index->x, _index->y, _includeSelf); }

	// 0223626539
	// SET NEIGHBORS ONLY IF THE CONDITION ON THAT TILE IS TRUE.
void setNeighborsConditional(const int _x, const int _y, const ARRAYS2_T _value, const int size, const std::string condition, const ARRAYS2_T _comparisonValue, bool _includeSelf=true)
{
	for ( int _y2=_y-size;_y2<=_y+size;++_y2)
	{
		for ( int _x2=_x-size;_x2<=_x+size;++_x2)
		{
			if ( isSafe(_x2,_y2) )
			{
				if ( _includeSelf==true || _x2!=_x || _y2!=_y )
				{
					if (condition=="==")
					{
						if ( (*this)(_x2,_y2) == _comparisonValue )
						{
							(*this)(_x2,_y2) = _value;
						}
					}
					else if (condition=="!=")
					{
						if ( (*this)(_x2,_y2) != _comparisonValue )
						{
							(*this)(_x2,_y2) = _value;
						}
					}
				}
			}
		}
	}
}


	// RETURN ALL VALID INDEXES AROUND THE BORDER.
Vector <HasXY*>* getBorder ( const int _centerX, const int _centerY, int _radius )
{
	Vector <HasXY*> * retVect = new Vector <HasXY*>;

	// // X SWEEP
	{
	const int topY=_centerY-_radius;
	const int bottomY=_centerY+_radius;
	for (int _x=_centerX-_radius;_x<=_centerX+_radius;++_x)
	{
		if ( isSafe(_x,topY)==true )
		{
			retVect->push( new HasXY (_x,topY) );
		}
		if ( isSafe(_x,bottomY)==true )
		{
			retVect->push( new HasXY (_x,bottomY) );
		}
	}
	}

	{
	const int leftX=_centerX-_radius;
	const int rightX=_centerX+_radius;
	for (int _y=_centerY-_radius+1;_y<_centerY+_radius;++_y)
	{
		if ( isSafe(leftX,_y)==true )
		{
			retVect->push( new HasXY (leftX,_y) );
		}
		if ( isSafe(rightX,_y)==true )
		{
			retVect->push( new HasXY (rightX,_y) );
		}
	}
	}

	return retVect;
}


void setNeighbors(const int _x, const int _y, const ARRAYS2_T _value, bool _includeSelf=true)
{
	// NEW SETUP.
	if ( _includeSelf==true )
	{ safeSet(_x,_y,_value); }

	if(isSafe(_x-1,_y-1))
	{ (*this)(_x-1,_y-1) = _value; }
	if(isSafe(_x-1,_y))
	{ (*this)(_x-1,_y) = _value; }
	if(isSafe(_x-1,_y+1))
	{ (*this)(_x-1,_y+1) = _value; }
	if(isSafe(_x,_y-1))
	{ (*this)(_x,_y-1) = _value; }
	if(isSafe(_x,_y+1))
	{ (*this)(_x,_y+1) = _value; }
	if(isSafe(_x+1,_y-1))
	{ (*this)(_x+1,_y-1) = _value; }
	if(isSafe(_x+1,_y))
	{ (*this)(_x+1,_y) = _value; }
	if(isSafe(_x+1,_y+1))
	{ (*this)(_x+1,_y+1) = _value; }
}

	/* 0223520734 */
// unsigned char nNeighborsEqual(Index* _index, const T _VALUE)
// { return nNeighborsEqual(_index->x, _index->y, _VALUE); }

unsigned char nNeighborsEqual(const int _X, const int _Y, const ARRAYS2_T _VALUE)
{
	int nNeighbors = 0;

	if(isSafe(_X-1,_Y-1) && (*this)(_X-1,_Y-1) == _VALUE)
	{ ++nNeighbors; }
	if(isSafe(_X-1,_Y) && (*this)(_X-1,_Y) == _VALUE)
	{ ++nNeighbors; }
	if(isSafe(_X-1,_Y+1) && (*this)(_X-1,_Y+1) == _VALUE)
	{ ++nNeighbors; }
	if(isSafe(_X,_Y-1) && (*this)(_X,_Y-1) == _VALUE)
	{ ++nNeighbors; }
	if(isSafe(_X,_Y+1) && (*this)(_X,_Y+1) == _VALUE)
	{ ++nNeighbors; }
	if(isSafe(_X+1,_Y-1) && (*this)(_X+1,_Y-1) == _VALUE)
	{ ++nNeighbors; }
	if(isSafe(_X+1,_Y) && (*this)(_X+1,_Y) == _VALUE)
	{ ++nNeighbors; }
	if(isSafe(_X+1,_Y+1) && (*this)(_X+1,_Y+1) == _VALUE)
	{ ++nNeighbors; }
	return nNeighbors;
}

bool allNeighborsLessThan(const int X, const int Y, const ARRAYS2_T VALUE)
{
	if(isSafe(X-1,Y-1) && (*this)(X-1,Y-1) >= VALUE)
	{ return false; }
	if(isSafe(X-1,Y) && (*this)(X-1,Y) >= VALUE)
	{ return false; }
	if(isSafe(X-1,Y+1) && (*this)(X-1,Y+1) >= VALUE)
	{ return false; }
	if(isSafe(X,Y-1) && (*this)(X,Y-1) >= VALUE)
	{ return false; }
	if(isSafe(X,Y+1) && (*this)(X,Y+1) >= VALUE)
	{ return false; }
	if(isSafe(X+1,Y-1) && (*this)(X+1,Y-1) >= VALUE)
	{ return false; }
	if(isSafe(X+1,Y) && (*this)(X+1,Y) >= VALUE)
	{ return false; }
	if(isSafe(X+1,Y+1) && (*this)(X+1,Y+1) >= VALUE)
	{ return false; }
	return true;
}
bool anyNeighborsLessThan(const int X, const int Y, const ARRAYS2_T VALUE)
{
	if(isSafe(X-1,Y-1) && (*this)(X-1,Y-1) < VALUE)
	{ return true; }
	if(isSafe(X-1,Y) && (*this)(X-1,Y) < VALUE)
	{ return true; }
	if(isSafe(X-1,Y+1) && (*this)(X-1,Y+1) < VALUE)
	{ return true; }
	if(isSafe(X,Y-1) && (*this)(X,Y-1) < VALUE)
	{ return true; }
	if(isSafe(X,Y+1) && (*this)(X,Y+1) < VALUE)
	{ return true; }
	if(isSafe(X+1,Y-1) && (*this)(X+1,Y-1) < VALUE)
	{ return true; }
	if(isSafe(X+1,Y) && (*this)(X+1,Y) < VALUE)
	{ return true; }
	if(isSafe(X+1,Y+1) && (*this)(X+1,Y+1) < VALUE)
	{ return true; }
	return false;
}
bool anyNeighborsGreaterThan(const int X, const int Y, const ARRAYS2_T VALUE)
{
	if(isSafe(X-1,Y-1) && (*this)(X-1,Y-1) > VALUE)
	{ return true; }
	if(isSafe(X-1,Y) && (*this)(X-1,Y) > VALUE)
	{ return true; }
	if(isSafe(X-1,Y+1) && (*this)(X-1,Y+1) > VALUE)
	{ return true; }
	if(isSafe(X,Y-1) && (*this)(X,Y-1) > VALUE)
	{ return true; }
	if(isSafe(X,Y+1) && (*this)(X,Y+1) > VALUE)
	{ return true; }
	if(isSafe(X+1,Y-1) && (*this)(X+1,Y-1) > VALUE)
	{ return true; }
	if(isSafe(X+1,Y) && (*this)(X+1,Y) > VALUE)
	{ return true; }
	if(isSafe(X+1,Y+1) && (*this)(X+1,Y+1) > VALUE)
	{ return true; }
	return false;
}
// unsigned char nNeighboursLessThan(const int _x, const int _y, const T _value)
// {
	// unsigned char n=0;
	// if(isSafe(X-1,Y-1) && (*this)(X-1,Y-1) < VALUE)
	// { ++n; }
	// if(isSafe(X-1,Y) && (*this)(X-1,Y) < VALUE)
	// { ++n; }
	// if(isSafe(X-1,Y+1) && (*this)(X-1,Y+1) < VALUE)
	// { ++n; }
	// if(isSafe(X,Y-1) && (*this)(X,Y-1) < VALUE)
	// { ++n; }
	// if(isSafe(X,Y+1) && (*this)(X,Y+1) < VALUE)
	// { ++n; }
	// if(isSafe(X+1,Y-1) && (*this)(X+1,Y-1) < VALUE)
	// { ++n; }
	// if(isSafe(X+1,Y) && (*this)(X+1,Y) < VALUE)
	// { ++n; }
	// if(isSafe(X+1,Y+1) && (*this)(X+1,Y+1) < VALUE)
	// { ++n; }
	// return n;
// }

unsigned char nNeighboursLessThan(const int _x, const int _y, const ARRAYS2_T _value)
{
	unsigned char n=0;

	if(isSafe(_x-1,_y-1) && (*this)(_x-1,_y-1) < _value)
	{ ++n; }
	if(isSafe(_x-1,_y) && (*this)(_x-1,_y) < _value)
	{ ++n; }
	if(isSafe(_x-1,_y+1) && (*this)(_x-1,_y+1) < _value)
	{ ++n; }
	if(isSafe(_x,_y-1) && (*this)(_x,_y-1) < _value)
	{ ++n; }
	if(isSafe(_x,_y+1) && (*this)(_x,_y+1) < _value)
	{ ++n; }
	if(isSafe(_x+1,_y-1) && (*this)(_x+1,_y-1) < _value)
	{ ++n; }
	if(isSafe(_x+1,_y) && (*this)(_x+1,_y) < _value)
	{ ++n; }
	if(isSafe(_x+1,_y+1) && (*this)(_x+1,_y+1) < _value)
	{ ++n; }
	return n;
}

unsigned char nNeighboursGreaterThan(const int _x, const int _y, const ARRAYS2_T _value)
{
	unsigned char n=0;

	if(isSafe(_x-1,_y-1) && (*this)(_x-1,_y-1) > _value)
	{ ++n; }
	if(isSafe(_x-1,_y) && (*this)(_x-1,_y) > _value)
	{ ++n; }
	if(isSafe(_x-1,_y+1) && (*this)(_x-1,_y+1) > _value)
	{ ++n; }
	if(isSafe(_x,_y-1) && (*this)(_x,_y-1) > _value)
	{ ++n; }
	if(isSafe(_x,_y+1) && (*this)(_x,_y+1) > _value)
	{ ++n; }
	if(isSafe(_x+1,_y-1) && (*this)(_x+1,_y-1) > _value)
	{ ++n; }
	if(isSafe(_x+1,_y) && (*this)(_x+1,_y) > _value)
	{ ++n; }
	if(isSafe(_x+1,_y+1) && (*this)(_x+1,_y+1) > _value)
	{ ++n; }
	return n;
}

unsigned char nNeighboursLessThan(const int _index, const ARRAYS2_T _value)
{
	unsigned char n=0;
	//const int maxIndex = nX*nY;

	int _x,_y;
	getIndexCoords(_index,&_x,&_y);

	if(isSafe(_x-1,_y-1) && (*this)(_x-1,_y-1) < _value)
	{ ++n; }
	if(isSafe(_x-1,_y) && (*this)(_x-1,_y) < _value)
	{ ++n; }
	if(isSafe(_x-1,_y+1) && (*this)(_x-1,_y+1) < _value)
	{ ++n; }
	if(isSafe(_x,_y-1) && (*this)(_x,_y-1) < _value)
	{ ++n; }
	if(isSafe(_x,_y+1) && (*this)(_x,_y+1) < _value)
	{ ++n; }
	if(isSafe(_x+1,_y-1) && (*this)(_x+1,_y-1) < _value)
	{ ++n; }
	if(isSafe(_x+1,_y) && (*this)(_x+1,_y) < _value)
	{ ++n; }
	if(isSafe(_x+1,_y+1) && (*this)(_x+1,_y+1) < _value)
	{ ++n; }
	return n;
}

	/* SECTION: Directions */

bool directionIsSafe(const int _x, const int _y, const int direction)
{
	switch (direction)
	{
		case(0): /* top-left */
			if(isSafe(_x-1,_y+1))
			{ return true; }
			return false;
		case(1): /* top */
			if(isSafe(_x,_y+1))
			{ return true; }
			return false;
		case(2): /* top-right */
			if(isSafe(_x+1,_y+1))
			{ return true; }
			return false;
		case(3): /* right */
			if(isSafe(_x+1,_y))
			{ return true; }
			return false;
		case(4): /* bottom-right */
			if(isSafe(_x+1,_y-1))
			{ return true; }
			return false;
		case(5): /* bottom */
			if(isSafe(_x,_y-1))
			{ return true; }
			return false;
		case(6): /* bottom-left */
			if(isSafe(_x-1,_y-1))
			{ return true; }
			return false;
		case(7): /* left */
			if(isSafe(_x-1,_y))
			{ return true; }
			return false;
		default:
		std::cout<<"Bad direction supplied: "<<direction<<"\n";
		return false;
	}

	return false;
}


void getDirectionCoords(const int _x, const int _y, const int _direction, int* _returnX, int* _returnY)
{
	switch (_direction)
	{
		case(0): /* top-left */
			*_returnX=_x-1;
			*_returnY=_y+1;
		return;
		case(1): /* top */
			*_returnX=_x;
			*_returnY=_y+1;
		return;
		case(2): /* top-right */
			*_returnX=_x+1;
			*_returnY=_y+1;
		return;
		case(3): /* right */
			*_returnX=_x+1;
			*_returnY=_y;
		return;
		case(4): /* bottom-right */
			*_returnX=_x+1;
			*_returnY=_y-1;
		return;
		case(5): /* bottom */
			*_returnX=_x;
			*_returnY=_y-1;
		return;
		case(6): /* bottom-left */
			*_returnX=_x-1;
			*_returnY=_y-1;
		return;
		case(7): /* left */
			*_returnX=_x-1;
			*_returnY=_y;
		return;
		default:
		std::cout<<"Bad direction supplied: "<<_direction<<"\n";
	}
	return;
}

ARRAYS2_T vDirection(const int _x, const int _y, const int direction)
{
	/* Sometimes I want to use directions for neighbors, instead of absolute coordinates.
		The directions are a value from 0-7, starting from top-left and rotating clockwise.
		NOTE: I must make sure the direction is safe before calling this function */

	switch (direction)
	{
		case(0): /* top-left */
			return (*this)(_x-1,_y+1);
		break;
		case(1): /* top */
			return (*this)(_x,_y+1);
		break;
		case(2): /* top-right */
			return (*this)(_x+1,_y+1);
		break;
		case(3): /* right */
			return (*this)(_x+1,_y);
		break;
		case(4): /* bottom-right */
			return (*this)(_x+1,_y-1);
		break;
		case(5): /* bottom */
			return (*this)(_x,_y-1);
		break;
		case(6): /* bottom-left */
			return (*this)(_x-1,_y-1);
		break;
		case(7): /* left */
			return (*this)(_x-1,_y);
		break;
		default:
		std::cout<<"Bad direction supplied: "<<direction<<"\n";
	}
	return nullValue;
}

bool testAnyNeighbors(const char expression, const int X, const int Y, const ARRAYS2_T COMPARISON_VALUE)
{
	if(X>1&&Y>1&&X<nX-2&&Y<nY-2)
	{
		{const ARRAYS2_T INDEX_VALUE = (*this)(X-1,Y-1);
		if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
		{ return true; }
		if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
		{ return true; }
		if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
		{ return true; }}
		{const ARRAYS2_T INDEX_VALUE = (*this)(X-1,Y);
		if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
		{ return true; }
		if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
		{ return true; }
		if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
		{ return true; }}
		{const ARRAYS2_T INDEX_VALUE = (*this)(X-1,Y+1);
		if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
		{ return true; }
		if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
		{ return true; }
		if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
		{ return true; }}
		{const ARRAYS2_T INDEX_VALUE = (*this)(X,Y-1);
		if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
		{ return true; }
		if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
		{ return true; }
		if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
		{ return true; }}
		{const ARRAYS2_T INDEX_VALUE = (*this)(X,Y+1);
		if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
		{ return true; }
		if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
		{ return true; }
		if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
		{ return true; }}
		{const ARRAYS2_T INDEX_VALUE = (*this)(X+1,Y-1);
		if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
		{ return true; }
		if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
		{ return true; }
		if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
		{ return true; }}
		{const ARRAYS2_T INDEX_VALUE = (*this)(X+1,Y);
		if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
		{ return true; }
		if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
		{ return true; }
		if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
		{ return true; }}
		{const ARRAYS2_T INDEX_VALUE = (*this)(X+1,Y+1);
		if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
		{ return true; }
		if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
		{ return true; }
		if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
		{ return true; }}
	}
	else
	{
		if(isSafe(X-1,Y-1))
		{   const ARRAYS2_T INDEX_VALUE = (*this)(X-1,Y-1);
			if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
			{ return true; }
			if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
			{ return true; }
			if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
			{ return true; }
		}
		if(isSafe(X-1,Y))
		{   const ARRAYS2_T INDEX_VALUE = (*this)(X-1,Y);
			if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
			{ return true; }
			if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
			{ return true; }
			if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
			{ return true; }
		}
		if(isSafe(X-1,Y+1))
		{   const ARRAYS2_T INDEX_VALUE = (*this)(X-1,Y+1);
			if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
			{ return true; }
			if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
			{ return true; }
			if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
			{ return true; }
		}
		if(isSafe(X,Y-1))
		{   const ARRAYS2_T INDEX_VALUE = (*this)(X,Y-1);
			if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
			{ return true; }
			if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
			{ return true; }
			if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
			{ return true; }
		}
		if(isSafe(X,Y+1))
		{   const ARRAYS2_T INDEX_VALUE = (*this)(X,Y+1);
			if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
			{ return true; }
			if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
			{ return true; }
			if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
			{ return true; }
		}
		if(isSafe(X+1,Y-1))
		{   const ARRAYS2_T INDEX_VALUE = (*this)(X+1,Y-1);
			if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
			{ return true; }
			if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
			{ return true; }
			if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
			{ return true; }
		}
		if(isSafe(X+1,Y))
		{   const ARRAYS2_T INDEX_VALUE = (*this)(X+1,Y);
			if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
			{ return true; }
			if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
			{ return true; }
			if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
			{ return true; }
		}
		if(isSafe(X+1,Y+1))
		{   const ARRAYS2_T INDEX_VALUE = (*this)(X+1,Y+1);
			if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
			{ return true; }
			if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
			{ return true; }
			if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
			{ return true; }
		}
	}
	return false;
}

bool safeForNeighbors (const int X, const int Y)
{
	if(X>1&&Y>1&&X<nX-2&&Y<nY-2)
	{ return true; }
	return false;
}

void getNeighborVector(const int _x, const int _y, Vector <ARRAYS2_T>* v, const bool includeSelf=false)
{
	v->clear();
	if(isSafe(_x-1,_y-1))
	{ v->add((*this)(_x-1,_y-1)); }
	if(isSafe(_x-1,_y))
	{ v->add((*this)(_x-1,_y)); }
	if(isSafe(_x-1,_y+1))
	{ v->add((*this)(_x-1,_y+1)); }
	if(isSafe(_x,_y-1))
	{ v->add((*this)(_x,_y-1)); }
	if(isSafe(_x,_y+1))
	{ v->add((*this)(_x,_y+1)); }
	if(isSafe(_x+1,_y-1))
	{ v->add((*this)(_x+1,_y-1)); }
	if(isSafe(_x+1,_y))
	{ v->add((*this)(_x+1,_y)); }
	if(isSafe(_x+1,_y+1))
	{ v->add((*this)(_x+1,_y+1)); }

	if (includeSelf==true)
	{
		if(isSafe(_x,_y))
		{ v->add((*this)(_x,_y)); }
	}
}

// void getNeighborNodes(const int _x, const int _y, Vector <ArrayS2Node <T>* >* v)
// {
	// //std::cout<<"Get neighbor nodes.\n";

	// v->clear();
	// addNeighborNodes(_x,_y,v);

	// //std::cout<<"Done.\n";
// }
// void addNeighborNodes (const int _x, const int _y, Vector <ArrayS2Node <T>* >* v)
// {
	// if(isSafe(_x-1,_y-1))
	// { v->add(new ArrayS2Node <T> (_x-1,_y-1,(*this)(_x-1,_y-1))); }
	// if(isSafe(_x-1,_y))
	// { v->add(new ArrayS2Node <T> (_x-1,_y,(*this)(_x-1,_y))); }
	// if(isSafe(_x-1,_y+1))
	// { v->add(new ArrayS2Node <T> (_x-1,_y+1,(*this)(_x-1,_y+1))); }
	// if(isSafe(_x,_y-1))
	// { v->add(new ArrayS2Node <T> (_x,_y-1,(*this)(_x,_y-1))); }
	// if(isSafe(_x,_y+1))
	// { v->add(new ArrayS2Node <T> (_x,_y+1,(*this)(_x,_y+1))); }
	// if(isSafe(_x+1,_y-1))
	// { v->add(new ArrayS2Node <T> (_x+1,_y-1,(*this)(_x+1,_y-1))); }
	// if(isSafe(_x+1,_y))
	// { v->add(new ArrayS2Node <T> (_x+1,_y,(*this)(_x+1,_y))); }
	// if(isSafe(_x+1,_y+1))
	// { v->add(new ArrayS2Node <T> (_x+1,_y+1,(*this)(_x+1,_y+1))); }
// }

void resetNeighbor()
{ neighborNumber=0; }

// ArrayS2Node <T>* getNextNeighbor(const int x, const int y)
// {
	// ++neighborNumber;
	// if(neighborNumber==1) //bottom left
	// {
		// if(isSafe(x-1,y-1))
		// { return new ArrayS2Node <T> (x-1,y-1,(*this)(x-1,y-1)); }
		// else
		// { return getNextNeighbor(x,y); }
	// }
	// else if(neighborNumber==2) // left
	// {
		// if(isSafe(x-1,y))
		// { return new ArrayS2Node <T> (x-1,y,(*this)(x-1,y)); }
		// else
		// { return getNextNeighbor(x,y); }
	// }
	// else if(neighborNumber==3) // top left
	// {
		// if(isSafe(x-1,y+1))
		// { return new ArrayS2Node <T> (x-1,y+1,(*this)(x-1,y+1)); }
		// else
		// { return getNextNeighbor(x,y); }
	// }
	// else if(neighborNumber==4) // top
	// {
		// if(isSafe(x,y+1))
		// { return new ArrayS2Node <T> (x,y+1,(*this)(x,y+1)); }
		// else
		// { return getNextNeighbor(x,y); }
	// }
	// else if(neighborNumber==5) // top right
	// {
		// if(isSafe(x+1,y+1))
		// { return new ArrayS2Node <T> (x+1,y+1,(*this)(x+1,y+1)); }
		// else
		// { return getNextNeighbor(x,y); }
	// }
	// else if(neighborNumber==6) // right
	// {
		// if(isSafe(x+1,y))
		// { return new ArrayS2Node <T> (x+1,y,(*this)(x+1,y)); }
		// else
		// { return getNextNeighbor(x,y); }
	// }
	// else if(neighborNumber==7) // bottom right
	// {
		// if(isSafe(x+1,y-1))
		// { return new ArrayS2Node <T> (x+1,y-1,(*this)(x+1,y-1)); }
		// else
		// { return getNextNeighbor(x,y); }
	// }
	// else if(neighborNumber==8) // bottom.
	// {
		// if(isSafe(x,y-1))
		// { return new ArrayS2Node <T> (x,y-1,(*this)(x,y-1)); }
		// else
		// { return getNextNeighbor(x,y); }
	// }

	// neighborNumber=0;
	// return 0; //if no more neighbors, return 0 and reset.
// }

	ARRAYS2_T neighborAverage(const unsigned int _x, const unsigned int _y) //021-082-0000
	{ //includes diagonals.
		int totalSides = 0;
		ARRAYS2_T number;
		if(isSafe(_x-1,_y-1))
		{
			++totalSides;
			number+=(*this)(_x-1,_y-1);
		}
		if(isSafe(_x-1,_y))
		{
			++totalSides;
			number+=(*this)(_x-1,_y);
		}
		if(isSafe(_x-1,_y+1))
		{
			++totalSides;
			number+=(*this)(_x-1,_y+1);
		}

		if(isSafe(_x,_y-1))
		{
			++totalSides;
			number+=(*this)(_x-1,_y-1);
		}
		if(isSafe(_x,_y+1))
		{
			++totalSides;
			number+=(*this)(_x-1,_y+1);
		}

		if(isSafe(_x+1,_y-1))
		{
			++totalSides;
			number+=(*this)(_x+1,_y-1);
		}
		if(isSafe(_x+1,_y))
		{
			++totalSides;
			number+=(*this)(_x+1,_y);
		}
		if(isSafe(_x+1,_y+1))
		{
			++totalSides;
			number+=(*this)(_x+1,_y+1);
		}

		number/=totalSides;
		return number;

	}

	#include <Container/ArrayS2/ArrayS2_Fill.hpp>
	
	// CURRENTLY BEING IMPLEMENTED
	// Return a vector of coordinates which are connected to the initial point with the same value.
	// In future we can add fuzzy select and whatnot.
	// Currently this is expecting binary values. It need to be changed to allow all values.
	
	// This function gets all unique areas. We should keep this functionality.
	
//template <class ARRAYS2_T>
Vector <HasXY> * floodFillVectorNOPE ( const int _startX=0, const int _startY=0, bool includeDiagonals=true )
{
	std::cout<<"Fill called for coordinates: ("<<_startX<<", "<<_startY<<")\n";
	const ARRAYS2_T initialValue = (*this)(_startX,_startY);
	
	// Vector of coordinates to return.
	Vector <HasXY> * vToFill = new Vector <HasXY>;

	/* vectors of coordinates to be checked (this can be changed to std::pair */
	Vector <int> vX;
	Vector <int> vY;
	int currentV=-1;
	int posX=_startX;
	int posY=_startY;
	
	int landMassSize = 0;
	Vector <int> vLandmassSize;
	int currentID = 1;
	
	ArrayS2 <bool> aChecked ( nX,nY,false );
	
	/* scanline fill loop */
	while (true)
	{
        const int INITIAL_X=posX;
        /* go left to find the leftmost tile for this row which meets the criteria for water, filling along the way */
        /* we also check the tile directly above or below to see if there are any tiles that can be filled on those rows. */
        /* If we find such tiles, then we store the leftmost ones, for both the upper and lower row. */
        /* NOTE: y coords seems to encode upside-down, so we need to work upside-down here. */
        /* Fill left */
        while ( posX>=0 && aChecked(posX,posY)==false && (*this)(posX,posY)==initialValue )
        {
			//aLandmassID(posX,posY)=currentID;
			aChecked(posX,posY)=true;
			vToFill->push(HasXY(posX,posY));

			/* check below */
			/* If we're not on the bottom row, and the below tile is land. */
			if(posY<nY-1 && (*this)(posX,posY+1)==true )
			{

				/* If we are at the leftmost or rightmost column of the map. */
				if(posX==0 || posX==nX-1 )
				{
					/* push tile below us if it's valid. */
					if(aChecked(posX,posY+1)==false)
					{
						vX.push(posX); vY.push(posY+1);
					}
				}
				/* if we aren't at the leftmost or rightmost column of the map. */
				else
				{
					/* if the bottom-left tile doesn't match, and the below tile has not been processed. */
					if ( (*this)(posX-1,posY+1)!=initialValue && aChecked(posX,posY+1)==false )
					{ 
						vX.push(posX); vY.push(posY+1);
					}
				}
			}
			/* check above */
			/* If we're not on the top row, and the above tile is water. */
			if(posY>0 && (*this)(posX,posY-1)==initialValue )
			{
				/* If we are at the leftmost or rightmost column of the map. */
				if(posX==0 || posX==nX-1 )
				{
					/* push tile above us if it's valid. */
					if(aChecked(posX,posY-1)==false)
					{
						vX.push(posX); vY.push(posY-1);
					}
				}
				/* if we aren't at the leftmost or rightmost column of the map. */
				else
				{
					/* if the top-left tile is land, and the above tile has not been processed. */
					if ((*this)(posX-1,posY-1)!=initialValue && aChecked(posX,posY-1)==false)
					{
						vX.push(posX); vY.push(posY-1);
					}
				}
			}
			--posX; /* move left 1 tile */
		}
		
		/* check the last X for this scan. */
		// NOTE: I THINK SWITCHING TO A DO WHILE MIGHT REMOVE THE NEED FOR THIS BIT.

		++posX;
		/* check below */
		/* If we're not on the bottom row, and the below tile is water, and the below tile hasn't been processed yet. */
		if(posY<nY-1 && (*this)(posX,posY+1)==true && aChecked(posX,posY+1)==false )
		{
			/* push coords onto vectors. */
			vX.push(posX); vY.push(posY+1);
		}

		/* check above */
		/* If we're not on the top row, and the above tile is water, and the above tile hasn't been processed yet. */
		if(posY>0 && (*this)(posX,posY-1)==true && aChecked(posX,posY-1)==false)
		{
			/* push coords onto vectors. */
			vX.push(posX); vY.push(posY-1);
		}

		// DIAGONALS: SAME BUT CHECK BELOW-LEFT
		if(posY<nY-1 && posX>0 && (*this)(posX-1,posY+1)==true && aChecked(posX-1,posY+1)==false )
		{
			vX.push(posX-1); vY.push(posY+1);
		}
		
		// DIAGONALS: SAME BUT CHECK ABOVE-LEFT
		if(posY>0 && posX>0 && (*this)(posX-1,posY-1)==true && aChecked(posX-1,posY-1)== false )
		{
			vX.push(posX-1); vY.push(posY-1);
		}

		posX=INITIAL_X+1;
		
		
		//aLandmassID = -1 is to become aChecked=false;
		//island is to become (*this).
		
		/* Go back to the initial x position, and now fill right */
		while ( posX < nX && (*this)(posX,posY)==true && aChecked(posX,posY)==false )
		{
			aChecked(posX,posY)=true;
			++landMassSize;
			/* check below */
			/* If we're not on the bottom row, and the below tile is water. */
			if(posY<nY-1 && (*this)(posX,posY+1)==true )
			{
			/* If we are at the leftmost or rightmost column of the map. */
			if(posX==0 || posX==nX-1 )
			{
			/* push tile below us if it's valid. */
			if(aChecked(posX,posY+1)==false)
			{ vX.push(posX); vY.push(posY+1); }
			}
			/* if we aren't at the leftmost or rightmost column of the map. */
			else
			{
			/* if the bottom-left tile is land, and the below tile has not been processed. */
			if ( (*this)(posX-1,posY+1)==false && aChecked(posX,posY+1)==false )
			{ vX.push(posX); vY.push(posY+1); }
			}
			}
			/* check above */
			/* If we're not on the top row, and the above tile is water. */
			if(posY>0 && (*this)(posX,posY-1)==true )
			{
			/* If we are at the leftmost or rightmost column of the map. */
			if(posX==0 || posX==nX-1 )
			{
			/* push tile above us if it's valid. */
			if(aChecked(posX,posY-1)==false)
			{ vX.push(posX); vY.push(posY-1);
			}
			}
			/* if we aren't at the leftmost or rightmost column of the map. */
			else
			{
			/* if the top-left tile is land, and the above tile has not been processed. */
			if ( (*this)(posX-1,posY-1)==false && aChecked(posX,posY-1)==false )
			{ vX.push(posX); vY.push(posY-1); }
			}
			}
			/* move right 1 tile */
			++posX;
		}

		//KLUDGE. DO WHILE MAYBE IS SMARTER. MAYBE.
		--posX;
		
		// DIAGONALS: CHECK BELOW-RIGHT
		if(posY<nY-1 && posX<nX-1 && (*this)(posX+1,posY+1)==true && aChecked(posX+1,posY+1)==false )
		{
			vX.push(posX+1); vY.push(posY+1);
		}

		// DIAGONALS: CHECK ABOVE-RIGHT
		if(posY>0 && posX<nX-1 && (*this)(posX+1,posY-1)==true && aChecked(posX+1,posY-1)==false )
		{
			vX.push(posX+1); vY.push(posY-1);
		}

		/* entire row is complete. Go to next set of coords. */
		/* Look for some good coords on the vector. */
		++currentV;

		for (;currentV<vX.size();++currentV)
		{
			if ( aChecked(vX(currentV),vY(currentV)) == false )
			{
				posX=vX(currentV);
				posY=vY(currentV);
				break;
			}
		}

		if(currentV==vX.size())
		{
			break;
		}
	}
	
	vX.clear();
	vY.clear();
	vLandmassSize.push(landMassSize);
	landMassSize=0;
	++currentID;	
	return 0;
}

//	Vector <HasXY> * floodFillVector ( const int _startX, const int _startY, bool includeDiagonals );
	
	//#include <Container/ArrayS2/ArrayS2_FloodFill.hpp>
// Vector <HasXY> * floodFillVector ( const int _startX, const int _startY, bool includeDiagonals )
// {
	// return 0;
// }
	
	#include <Container/ArrayS2/ArrayS2_Neighbours.hpp>

	#include <Container/ArrayS2/ArrayS2_FloodFill.hpp>
};

#include <Container/ArrayS2/ArrayS2.cpp>

#endif

