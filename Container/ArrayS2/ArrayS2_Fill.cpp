/* ArrayS2_Fill.cpp
	Implementation of fill functions.
*/



template <class ARRAYS2_T>
void ArrayS2<ARRAYS2_T>::fill(ARRAYS2_T value)
{
	currentElement=&data[0];
	while(currentElement!=nullAddress)
	{
		*currentElement=value;
		++currentElement;
	}
	currentElement=&data[0];
}

template <class ARRAYS2_T>
void ArrayS2<ARRAYS2_T>::fill(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, ARRAYS2_T value)
{
	for(unsigned int _x1=x1;_x1<x2;++_x1)
	{
		for(unsigned int _y1=y1;_y1<y2;++_y1)
		{
			(*this)(_x1,_y1)=value;
		}
	}
}
	// 0231775585: I ADDED THIS 'PROPER' FILL FUNCTION. IS INCLUSIVE AND HOPEFULLY MORE FRIENDLY TOO.
template <class ARRAYS2_T>
void ArrayS2<ARRAYS2_T>::fill2Safe(int x1, int y1, int x2, int y2, ARRAYS2_T value)
{
	if ( x1>x2 )
	{ DataTools::swap(&x1,&x2); }
	if ( y1>y2 )
	{ DataTools::swap(&y1,&y2); }



	for(int _x1=x1;_x1<=x2;++_x1)
	{
		for(int _y1=y1;_y1<=y2;++_y1)
		{
			if (isSafe(_x1,_y1))
			{
				(*this)(_x1,_y1)=value;
			}
		}
	}
}

template <class ARRAYS2_T>
void ArrayS2<ARRAYS2_T>::fillFromCenterPoint(const int _centerX, const int _centerY, const int _radius, ARRAYS2_T _value)
{
	fill2Safe(_centerX-_radius,_centerY-_radius,_centerX+_radius,_centerY+_radius,_value);
}



template <class ARRAYS2_T>
void ArrayS2<ARRAYS2_T>::fillBorder ( const ARRAYS2_T _value, const int _startX, const int _startY, int _endX, int _endY )
{
	// FOR SOME REASON I CAN REFERENCE THE CLASS VARS IN THE DEFAULT ASSIGNMENT...
	if (_endX==-1) { _endX = nX-1; }
	if (_endY==-1) { _endY = nY-1; }

	// X SWEEP
	for (int _x=_startX;_x<=_endX;++_x)
	{
		set(_x,_startY,_value);
		set(_x,_endY,_value);
	}
	for (int _y=_startY;_y<=_endY;++_y)
	{
		set(_startX,_y,_value);
		set(_endX,_y,_value);
	}
}