#pragma once
#ifndef WILDCAT_CONTAINER_ARRAYS2_ARRAYS2_CPP
#define WILDCAT_CONTAINER_ARRAYS2_ARRAYS2_CPP

#include <stdlib.h> /* abs */

/*

	<Container/ArrayS2/ArrayS2.cpp>
	Implementation of <Container/ArrayS2/ArrayS2.hpp>

*/

template <class ARRAYS2_T>
ArrayS2<ARRAYS2_T>::~ArrayS2() { delete[] data; }

template <class ARRAYS2_T>
ArrayS2<ARRAYS2_T>::ArrayS2()
{
	nX=0;
	nY=0;
	nullAddress=0;
	currentElement=0;
	overFlow=false;
	data=0;
	name="unnamed";
	neighborNumber=0;
}

template <class ARRAYS2_T>
ArrayS2<ARRAYS2_T>::ArrayS2(const unsigned int _nX, const unsigned int _nY, const ARRAYS2_T _nullValue)
{
	nX=0;
	nY=0;
	nullAddress=0;
	currentElement=0;
	overFlow=false;
	data=0;
	name="unnamed";
	neighborNumber=0;
	init(_nX,_nY,_nullValue);
}

template <class ARRAYS2_T>
ArrayS2<ARRAYS2_T>::ArrayS2 (const ArrayS2 <ARRAYS2_T> &array)
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

template <class ARRAYS2_T>
void ArrayS2<ARRAYS2_T>::init(const unsigned int x, const unsigned int y, const ARRAYS2_T _nullValue)
{
	delete[] data;	
	nX=x;
	nY=y;
	data = new ARRAYS2_T [x*y];
	nullAddress=&data[x*y];
	this->nullValue=_nullValue;
	currentElement=&data[0];
	fill(_nullValue);
}

template <class ARRAYS2_T>
void ArrayS2<ARRAYS2_T>::initClass(const unsigned int x, const unsigned int y)
{
	delete[] data;	
	nX=x;
	nY=y;
	data = new ARRAYS2_T [x*y];
	nullAddress=&data[x*y];
	currentElement=&data[0];
}



template <class ARRAYS2_T>
void ArrayS2<ARRAYS2_T>::safeSet(const int _x, const int _y, const ARRAYS2_T _value)
{
	if (isSafe(_x,_y))
	{
		(*this)(_x,_y)=_value;
	}
}




// 0230223405 UPDATED TO PREVENT DATATYPE SIZE OVERRUNS.
// UNTESTED
template <class ARRAYS2_T>
ARRAYS2_T ArrayS2<ARRAYS2_T>::averageValue(const int skipSize)
{
	// FIND OUT HOW MANY SAMPLES WE WILL BE TAKING.
	double nValues;
	for(long int y=0;y<nY;y+=skipSize)
	{
		for(long int x=0;x<nX;x+=skipSize)
		{
			++nValues;
		}
	}
	// CALCULATE THE AVERAGE
	double movingAverage=0;
	for(int y=0;y<nY;y+=skipSize)
	{
		for(int x=0;x<nX;x+=skipSize)
		{
			movingAverage+=((*this)(x,y)/nValues);
		}
	}
	return movingAverage;
}

template <class ARRAYS2_T>
ArrayS2<ARRAYS2_T>* ArrayS2<ARRAYS2_T>::resizeTo(const int _x, const int _y)
/* can upsize and downsize. can squash aspect ratio */
/* works by discarding pixels */
{
	// number of old pixels for each new pixel: oldX/newX
	ArrayS2 <ARRAYS2_T> * resizedArray = new ArrayS2 <ARRAYS2_T>;
	const double pixelRatioX = (double)nX/_x;
	const double pixelRatioY = (double)nY/_y;
	
	//std::cout<<"Resizing ratios: "<<pixelRatioX<<","<<pixelRatioY<<"\n";
	
	resizedArray->init(_x,_y,0);
	
	for(int y=0;y<resizedArray->nY;++y)
	{
		for(int x=0;x<resizedArray->nX;++x)
		{
			int targetX = x*pixelRatioX;
			int targetY = y*pixelRatioY;
			(*resizedArray)(x,y)=(*this)(targetX,targetY);
			//get all the required neighbor pixels.
			//Vector <T> neighbours;
			
			//for(int y2=y;y2<y+pixelRatioY;++y2)
			//{
			//	for(int x2=x;x2<x+pixelRatioX;++x2)
			//	{
					//neighbours.push(vIndex(x2,y2));
					
			//	}
			//}
			
		}
	}
	
	return resizedArray;
}

// NOTE: This algorithm uses a square searching algorithm. This is inaccurate if you're not operating under
// the assumption that diagonal movement is equivalent to sideways movement.
template <class ARRAYS2_T>
int ArrayS2<ARRAYS2_T>::getClosestDistanceManhattan(const int X, const int Y, const char EXPRESSION, const ARRAYS2_T COMPARISON_VALUE, const int SEARCH_RADIUS)
{
	int searchX = X - SEARCH_RADIUS;
	if(searchX<0) { searchX=0; }
	else if(searchX>nX-1) { searchX=nX-1; }
	int searchY = Y - SEARCH_RADIUS;
	if(searchY<0) { searchY=0; }
	else if(searchY>nY-1) { searchY=nY-1; }
	
	int endX = X+SEARCH_RADIUS;
	int endY = Y+SEARCH_RADIUS;
	if(endX>nX-1) { endX=nX-1; }
	else if(endX<0) { endX=0; }
	if(endY>nY-1) { endY=nY-1; }
	else if (endY<0) { endY=0; }
	
	if(searchX==endX && searchY==endY) { return -1; }
	
	int bestDist=-1;
	
	const int searchX2 = searchX;
	
	for(;searchY<=endY;++searchY)
	{
		for(searchX=searchX2;searchX<=endX;++searchX)
		{
			if(testExpression((*this)(searchX,searchY),EXPRESSION,COMPARISON_VALUE)==true)
			{
				const int newDist = abs((X-searchX)) + abs((Y-searchY));

				if(newDist<bestDist || bestDist==-1)
				{ bestDist=newDist;
				}
			}
		}
	}
	return bestDist;
}

template <class ARRAYS2_T>
double ArrayS2<ARRAYS2_T>::getClosestDistanceTrue(const int X, const int Y, const char EXPRESSION, const ARRAYS2_T COMPARISON_VALUE, const int SEARCH_RADIUS)
{
	//Get rough search area.
	int searchX = X - SEARCH_RADIUS;
	if(searchX<0) { searchX=0; }
	else if(searchX>nX-1) { searchX=nX-1; }
	int searchY = Y - SEARCH_RADIUS;
	if(searchY<0) { searchY=0; }
	else if(searchY>nY-1) { searchY=nY-1; }
	
	int endX = X+SEARCH_RADIUS;
	int endY = Y+SEARCH_RADIUS;
	if(endX>nX-1) { endX=nX-1; }
	else if(endX<0) { endX=0; }
	if(endY>nY-1) { endY=nY-1; }
	else if (endY<0) { endY=0; }
	
	
	if(searchX==endX && searchY==endY) { return -1; }
	
	double bestDist=-1;
	
	const int searchX2 = searchX;
	
	for(;searchY<=endY;++searchY)
	{
		for(searchX=searchX2;searchX<=endX;++searchX)
		{
			//Does this point lie within the search circle?
			// ie, is the distance from the center of the
			// circle greater than the radius?
			// Before that, might as well make sure it's
			// shorter than the shortest distance so far.
			
			const double distFromCenter = sqrt(((X-searchX)*(X-searchX))+((Y-searchY)*(Y-searchY)));
			if((distFromCenter<bestDist || bestDist==-1) && distFromCenter<=SEARCH_RADIUS)
			{
				if(testExpression((*this)(searchX,searchY),EXPRESSION,COMPARISON_VALUE)==true)
				{
					bestDist=distFromCenter;
				}
			}
		}
	}
	return bestDist;
}

template <class ARRAYS2_T>
void ArrayS2<ARRAYS2_T>::replaceAll(const ARRAYS2_T find, const ARRAYS2_T replace)
{
	currentElement=&data[0];
	while(currentElement!=nullAddress)
	{
		if(*currentElement == find)
		{
			*currentElement=replace;
		}
		++currentElement;
	}
	currentElement=&data[0];
}

template <class ARRAYS2_T>
bool ArrayS2<ARRAYS2_T>::contains(ARRAYS2_T value)
{
	currentElement=&data[0];
	while(currentElement!=nullAddress)
	{
		if(*currentElement==value)
		{
			return true;
		}
		++currentElement;
	}
	currentElement=&data[0];
	return false;
}

template <class ARRAYS2_T>
inline bool ArrayS2<ARRAYS2_T>::testExpression (const ARRAYS2_T V1, const char EXPRESSION, const ARRAYS2_T V2)
{
	if(EXPRESSION=='=')
	{   if(V1==V2) { return true; }
	}
	else if(EXPRESSION=='<')
	{   if(V1<V2) { return true; }
	}
	else if(EXPRESSION=='>')
	{   if(V1>V2) { return true; }
	}
	else if(EXPRESSION=='!')
	{   if(V1!=V2) { return true; }
	}
	return false;
}

template <class ARRAYS2_T>
ArrayS2<ARRAYS2_T> ArrayS2<ARRAYS2_T>::sub (unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2)
{
	ArrayS2 <ARRAYS2_T> subArray;
	subArray.init(x2-x1+1,y2-y1+1,nullValue);
	for(unsigned int y=0;y<subArray.nY;++y)
	{
		for(unsigned int x=0;x<subArray.nX;++x)
		{
			subArray(x,y) = (*this)(x1+x,y1+y);
		}
	}
	return subArray;
}


// bool ArrayS2<ARRAYS2_T>::isSafeIndex(const int _x, const int _y) //021-082-0000
// {
	// if(_x<nX && _y<nY && _x>=0 && _y>=0)
	// { return true; }
	// return false;
// }

template <class ARRAYS2_T>
bool ArrayS2<ARRAYS2_T>::isSafe(const int _x, const int _y)
{
	if(_x<nX && _y<nY && _x>=0 && _y>=0)
	{ return true; }
	return false;
}
	template <class ARRAYS2_T>
	inline bool ArrayS2<ARRAYS2_T>::isSafe(HasXY* _objectWithXY)
	{ return isSafe(_objectWithXY->x,_objectWithXY->y); }
	// template <class ARRAYS2_T>
	// inline bool ArrayS2<ARRAYS2_T>::isSafe(HasXY _objectWithXY)
	// { return isSafe(_objectWithXY.x,_objectWithXY.y); }

template <class ARRAYS2_T>
inline void ArrayS2<ARRAYS2_T>::getIndexCoords(const int _index, int* _returnX, int* _returnY)
{
	*_returnX = (_index%nX);
	*_returnY = (_index-*_returnX)/nX;
}


template <class ARRAYS2_T>
void ArrayS2<ARRAYS2_T>::set (const int _x, const int _y, const ARRAYS2_T _value)
{
	(*this)(_x,_y)=_value;
}

#include <Container/ArrayS2/ArrayS2_Fill.cpp>

#endif
