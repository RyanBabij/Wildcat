/* ArrayS2_Neighbours.hpp
	Deals with functions that handle neighbors, as well as directions.
	These are basically a simplistic form of relative coordinates. */
	
	/* SECTION: Neighbours */
	

// bool includeSelf_;

// inline ArrayS2<T>& INCLUDE_SELF()
// { includeSelf_ = true; return *this; }


#ifdef NOPE


	// WHAT THE HELL IS 'INDEX'? I'LL PUT A HASXY IMPLEMENTATION HERE.
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





T getNeighborsTotal(const int _x, const int _y, const bool _includeSelf=false)
{
	Vector <HasXY*> * vectorIndex = getNeighbors(_x,_y,_includeSelf);
	
	
	return 0;
	
}



unsigned char INCLUDE_SELF; // 00000001
	
void setNeighbors2(const int _x, const int _y, const ARRAYS2_T _value, const int size, unsigned char params)
{
	if ( params & INCLUDE_SELF  == INCLUDE_SELF )
	{
		std::cout<<"INCLUDE_SELF\n";
	}

}

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
Vector <Index*> * getNeighbors(const int _x, const int _y, const bool _includeSelf=false)
{
	Vector <Index*> * vectorIndex = new Vector <Index*>;

	if (_includeSelf==true && isSafe(_x,_y))
	{ vectorIndex->push(new Index(_x,_y)); }
	
	if(isSafe(_x-1,_y-1))
	{ vectorIndex->push(new Index(_x-1,_y-1)); }
	if(isSafe(_x-1,_y))
	{ vectorIndex->push(new Index(_x-1,_y)); }
	if(isSafe(_x-1,_y+1))
	{ vectorIndex->push(new Index(_x-1,_y+1)); }
	if(isSafe(_x,_y-1))
	{ vectorIndex->push(new Index(_x,_y-1)); }
	if(isSafe(_x,_y+1))
	{ vectorIndex->push(new Index(_x,_y+1)); }
	if(isSafe(_x+1,_y-1))
	{ vectorIndex->push(new Index(_x+1,_y-1)); }
	if(isSafe(_x+1,_y))
	{ vectorIndex->push(new Index(_x+1,_y)); }
	if(isSafe(_x+1,_y+1))
	{ vectorIndex->push(new Index(_x+1,_y+1)); }

	return vectorIndex;
}




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
unsigned char nNeighborsEqual(Index* _index, const ARRAYS2_T _VALUE)
{ return nNeighborsEqual(_index->x, _index->y, _VALUE); }
	
unsigned char nNeighborsEqual(const int _X, const int _Y, const ARRAYS2_T _VALUE)
{
	int nNeighbors = 0;

	if(isSafe(_X-1,_Y-1) && vIndex(_X-1,_Y-1) == _VALUE)
	{ ++nNeighbors; }
	if(isSafe(_X-1,_Y) && vIndex(_X-1,_Y) == _VALUE)
	{ ++nNeighbors; }
	if(isSafe(_X-1,_Y+1) && vIndex(_X-1,_Y+1) == _VALUE)
	{ ++nNeighbors; }
	if(isSafe(_X,_Y-1) && vIndex(_X,_Y-1) == _VALUE)
	{ ++nNeighbors; }
	if(isSafe(_X,_Y+1) && vIndex(_X,_Y+1) == _VALUE)
	{ ++nNeighbors; }
	if(isSafe(_X+1,_Y-1) && vIndex(_X+1,_Y-1) == _VALUE)
	{ ++nNeighbors; }
	if(isSafe(_X+1,_Y) && vIndex(_X+1,_Y) == _VALUE)
	{ ++nNeighbors; }
	if(isSafe(_X+1,_Y+1) && vIndex(_X+1,_Y+1) == _VALUE)
	{ ++nNeighbors; }
	return nNeighbors;
}
	
bool allNeighborsLessThan(const int X, const int Y, const ARRAYS2_T VALUE)
{
	if(isSafe(X-1,Y-1) && vIndex(X-1,Y-1) >= VALUE)
	{ return false; }
	if(isSafe(X-1,Y) && vIndex(X-1,Y) >= VALUE)
	{ return false; }
	if(isSafe(X-1,Y+1) && vIndex(X-1,Y+1) >= VALUE)
	{ return false; }
	if(isSafe(X,Y-1) && vIndex(X,Y-1) >= VALUE)
	{ return false; }
	if(isSafe(X,Y+1) && vIndex(X,Y+1) >= VALUE)
	{ return false; }
	if(isSafe(X+1,Y-1) && vIndex(X+1,Y-1) >= VALUE)
	{ return false; }
	if(isSafe(X+1,Y) && vIndex(X+1,Y) >= VALUE)
	{ return false; }
	if(isSafe(X+1,Y+1) && vIndex(X+1,Y+1) >= VALUE)
	{ return false; }
	return true;
}
bool anyNeighborsLessThan(const int X, const int Y, const ARRAYS2_T VALUE)
{
	if(isSafe(X-1,Y-1) && vIndex(X-1,Y-1) < VALUE)
	{ return true; }
	if(isSafe(X-1,Y) && vIndex(X-1,Y) < VALUE)
	{ return true; }
	if(isSafe(X-1,Y+1) && vIndex(X-1,Y+1) < VALUE)
	{ return true; }
	if(isSafe(X,Y-1) && vIndex(X,Y-1) < VALUE)
	{ return true; }
	if(isSafe(X,Y+1) && vIndex(X,Y+1) < VALUE)
	{ return true; }
	if(isSafe(X+1,Y-1) && vIndex(X+1,Y-1) < VALUE)
	{ return true; }
	if(isSafe(X+1,Y) && vIndex(X+1,Y) < VALUE)
	{ return true; }
	if(isSafe(X+1,Y+1) && vIndex(X+1,Y+1) < VALUE)
	{ return true; }
	return false;
}
bool anyNeighborsGreaterThan(const int X, const int Y, const ARRAYS2_T VALUE)
{
	if(isSafe(X-1,Y-1) && vIndex(X-1,Y-1) > VALUE)
	{ return true; }
	if(isSafe(X-1,Y) && vIndex(X-1,Y) > VALUE)
	{ return true; }
	if(isSafe(X-1,Y+1) && vIndex(X-1,Y+1) > VALUE)
	{ return true; }
	if(isSafe(X,Y-1) && vIndex(X,Y-1) > VALUE)
	{ return true; }
	if(isSafe(X,Y+1) && vIndex(X,Y+1) > VALUE)
	{ return true; }
	if(isSafe(X+1,Y-1) && vIndex(X+1,Y-1) > VALUE)
	{ return true; }
	if(isSafe(X+1,Y) && vIndex(X+1,Y) > VALUE)
	{ return true; }
	if(isSafe(X+1,Y+1) && vIndex(X+1,Y+1) > VALUE)
	{ return true; }
	return false;
}
// unsigned char nNeighboursLessThan(const int _x, const int _y, const ARRAYS2_T _value)
// {
	// unsigned char n=0;
	// if(isSafe(X-1,Y-1) && vIndex(X-1,Y-1) < VALUE)
	// { ++n; }
	// if(isSafe(X-1,Y) && vIndex(X-1,Y) < VALUE)
	// { ++n; }
	// if(isSafe(X-1,Y+1) && vIndex(X-1,Y+1) < VALUE)
	// { ++n; }
	// if(isSafe(X,Y-1) && vIndex(X,Y-1) < VALUE)
	// { ++n; }
	// if(isSafe(X,Y+1) && vIndex(X,Y+1) < VALUE)
	// { ++n; }
	// if(isSafe(X+1,Y-1) && vIndex(X+1,Y-1) < VALUE)
	// { ++n; }
	// if(isSafe(X+1,Y) && vIndex(X+1,Y) < VALUE)
	// { ++n; }
	// if(isSafe(X+1,Y+1) && vIndex(X+1,Y+1) < VALUE)
	// { ++n; }
	// return n;
// }

unsigned char nNeighboursLessThan(const int _x, const int _y, const ARRAYS2_T _value)
{
	unsigned char n=0;
	
	if(isSafe(_x-1,_y-1) && vIndex(_x-1,_y-1) < _value)
	{ ++n; }
	if(isSafe(_x-1,_y) && vIndex(_x-1,_y) < _value)
	{ ++n; }
	if(isSafe(_x-1,_y+1) && vIndex(_x-1,_y+1) < _value)
	{ ++n; }
	if(isSafe(_x,_y-1) && vIndex(_x,_y-1) < _value)
	{ ++n; }
	if(isSafe(_x,_y+1) && vIndex(_x,_y+1) < _value)
	{ ++n; }
	if(isSafe(_x+1,_y-1) && vIndex(_x+1,_y-1) < _value)
	{ ++n; }
	if(isSafe(_x+1,_y) && vIndex(_x+1,_y) < _value)
	{ ++n; }
	if(isSafe(_x+1,_y+1) && vIndex(_x+1,_y+1) < _value)
	{ ++n; }
	return n;
}

unsigned char nNeighboursLessThan(const int _index, const ARRAYS2_T _value)
{
	unsigned char n=0;
	//const int maxIndex = nX*nY;
	
	int _x,_y;
	getIndexCoords(_index,&_x,&_y);

	if(isSafe(_x-1,_y-1) && vIndex(_x-1,_y-1) < _value)
	{ ++n; }
	if(isSafe(_x-1,_y) && vIndex(_x-1,_y) < _value)
	{ ++n; }
	if(isSafe(_x-1,_y+1) && vIndex(_x-1,_y+1) < _value)
	{ ++n; }
	if(isSafe(_x,_y-1) && vIndex(_x,_y-1) < _value)
	{ ++n; }
	if(isSafe(_x,_y+1) && vIndex(_x,_y+1) < _value)
	{ ++n; }
	if(isSafe(_x+1,_y-1) && vIndex(_x+1,_y-1) < _value)
	{ ++n; }
	if(isSafe(_x+1,_y) && vIndex(_x+1,_y) < _value)
	{ ++n; }
	if(isSafe(_x+1,_y+1) && vIndex(_x+1,_y+1) < _value)
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
	
T vDirection(const int _x, const int _y, const int direction)
{
	/* Sometimes I want to use directions for neighbors, instead of absolute coordinates.
		The directions are a value from 0-7, starting from top-left and rotating clockwise.
		NOTE: I must make sure the direction is safe before calling this function */

	switch (direction)
	{
		case(0): /* top-left */
			return vIndex(_x-1,_y+1);
		break;
		case(1): /* top */
			return vIndex(_x,_y+1);
		break;
		case(2): /* top-right */
			return vIndex(_x+1,_y+1);
		break;
		case(3): /* right */
			return vIndex(_x+1,_y);
		break;
		case(4): /* bottom-right */
			return vIndex(_x+1,_y-1);
		break;
		case(5): /* bottom */
			return vIndex(_x,_y-1);
		break;
		case(6): /* bottom-left */
			return vIndex(_x-1,_y-1);
		break;
		case(7): /* left */
			return vIndex(_x-1,_y);
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
		{const ARRAYS2_T INDEX_VALUE = vIndex(X-1,Y-1);
		if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
		{ return true; }
		if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
		{ return true; }
		if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
		{ return true; }}
		{const ARRAYS2_T INDEX_VALUE = vIndex(X-1,Y);
		if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
		{ return true; }
		if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
		{ return true; }
		if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
		{ return true; }}
		{const ARRAYS2_T INDEX_VALUE = vIndex(X-1,Y+1);
		if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
		{ return true; }
		if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
		{ return true; }
		if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
		{ return true; }}
		{const ARRAYS2_T INDEX_VALUE = vIndex(X,Y-1);
		if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
		{ return true; }
		if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
		{ return true; }
		if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
		{ return true; }}
		{const ARRAYS2_T INDEX_VALUE = vIndex(X,Y+1);
		if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
		{ return true; }
		if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
		{ return true; }
		if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
		{ return true; }}
		{const ARRAYS2_T INDEX_VALUE = vIndex(X+1,Y-1);
		if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
		{ return true; }
		if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
		{ return true; }
		if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
		{ return true; }}
		{const ARRAYS2_T INDEX_VALUE = vIndex(X+1,Y);
		if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
		{ return true; }
		if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
		{ return true; }
		if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
		{ return true; }}
		{const ARRAYS2_T INDEX_VALUE = vIndex(X+1,Y+1);
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
		{   const ARRAYS2_T INDEX_VALUE = vIndex(X-1,Y-1);
			if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
			{ return true; }
			if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
			{ return true; }
			if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
			{ return true; }
		}
		if(isSafe(X-1,Y))
		{   const ARRAYS2_T INDEX_VALUE = vIndex(X-1,Y);
			if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
			{ return true; }
			if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
			{ return true; }
			if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
			{ return true; }
		}
		if(isSafe(X-1,Y+1))
		{   const ARRAYS2_T INDEX_VALUE = vIndex(X-1,Y+1);
			if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
			{ return true; }
			if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
			{ return true; }
			if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
			{ return true; }
		}
		if(isSafe(X,Y-1))
		{   const ARRAYS2_T INDEX_VALUE = vIndex(X,Y-1);
			if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
			{ return true; }
			if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
			{ return true; }
			if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
			{ return true; }
		}
		if(isSafe(X,Y+1))
		{   const ARRAYS2_T INDEX_VALUE = vIndex(X,Y+1);
			if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
			{ return true; }
			if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
			{ return true; }
			if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
			{ return true; }
		}
		if(isSafe(X+1,Y-1))
		{   const ARRAYS2_T INDEX_VALUE = vIndex(X+1,Y-1);
			if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
			{ return true; }
			if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
			{ return true; }
			if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
			{ return true; }
		}
		if(isSafe(X+1,Y))
		{   const ARRAYS2_T INDEX_VALUE = vIndex(X+1,Y);
			if(expression=='>' && INDEX_VALUE > COMPARISON_VALUE)
			{ return true; }
			if(expression=='<' && INDEX_VALUE < COMPARISON_VALUE)
			{ return true; }
			if(expression=='=' && INDEX_VALUE == COMPARISON_VALUE)
			{ return true; }
		}
		if(isSafe(X+1,Y+1))
		{   const ARRAYS2_T INDEX_VALUE = vIndex(X+1,Y+1);
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

void getNeighborVector(const int _x, const int _y, Vector <T>* v, const bool includeSelf=false)
{
	v->clear();
	if(isSafe(_x-1,_y-1))
	{ v->add(vIndex(_x-1,_y-1)); }
	if(isSafe(_x-1,_y))
	{ v->add(vIndex(_x-1,_y)); }
	if(isSafe(_x-1,_y+1))
	{ v->add(vIndex(_x-1,_y+1)); }
	if(isSafe(_x,_y-1))
	{ v->add(vIndex(_x,_y-1)); }
	if(isSafe(_x,_y+1))
	{ v->add(vIndex(_x,_y+1)); }
	if(isSafe(_x+1,_y-1))
	{ v->add(vIndex(_x+1,_y-1)); }
	if(isSafe(_x+1,_y))
	{ v->add(vIndex(_x+1,_y)); }
	if(isSafe(_x+1,_y+1))
	{ v->add(vIndex(_x+1,_y+1)); }
	
	if (includeSelf==true)
	{
		if(isSafe(_x,_y))
		{ v->add(vIndex(_x,_y)); }
	}
}

void getNeighborNodes(const int _x, const int _y, Vector <ArrayS2Node <T>* >* v)
{
	//std::cout<<"Get neighbor nodes.\n";
	
	v->clear();
	addNeighborNodes(_x,_y,v);
	
	//std::cout<<"Done.\n";
}
void addNeighborNodes (const int _x, const int _y, Vector <ArrayS2Node <T>* >* v)
{
	if(isSafe(_x-1,_y-1))
	{ v->add(new ArrayS2Node <T> (_x-1,_y-1,vIndex(_x-1,_y-1))); }
	if(isSafe(_x-1,_y))
	{ v->add(new ArrayS2Node <T> (_x-1,_y,vIndex(_x-1,_y))); }
	if(isSafe(_x-1,_y+1))
	{ v->add(new ArrayS2Node <T> (_x-1,_y+1,vIndex(_x-1,_y+1))); }
	if(isSafe(_x,_y-1))
	{ v->add(new ArrayS2Node <T> (_x,_y-1,vIndex(_x,_y-1))); }
	if(isSafe(_x,_y+1))
	{ v->add(new ArrayS2Node <T> (_x,_y+1,vIndex(_x,_y+1))); }
	if(isSafe(_x+1,_y-1))
	{ v->add(new ArrayS2Node <T> (_x+1,_y-1,vIndex(_x+1,_y-1))); }
	if(isSafe(_x+1,_y))
	{ v->add(new ArrayS2Node <T> (_x+1,_y,vIndex(_x+1,_y))); }
	if(isSafe(_x+1,_y+1))
	{ v->add(new ArrayS2Node <T> (_x+1,_y+1,vIndex(_x+1,_y+1))); }
}

void resetNeighbor()
{ neighborNumber=0; }
ArrayS2Node <T>* getNextNeighbor(const int x, const int y)
{
	++neighborNumber;
	if(neighborNumber==1) //bottom left
	{
		if(isSafe(x-1,y-1))
		{ return new ArrayS2Node <T> (x-1,y-1,vIndex(x-1,y-1)); }
		else
		{ return getNextNeighbor(x,y); }
	}
	else if(neighborNumber==2) // left
	{
		if(isSafe(x-1,y))
		{ return new ArrayS2Node <T> (x-1,y,vIndex(x-1,y)); }
		else
		{ return getNextNeighbor(x,y); }
	}
	else if(neighborNumber==3) // top left
	{
		if(isSafe(x-1,y+1))
		{ return new ArrayS2Node <T> (x-1,y+1,vIndex(x-1,y+1)); }
		else
		{ return getNextNeighbor(x,y); }
	}
	else if(neighborNumber==4) // top
	{
		if(isSafe(x,y+1))
		{ return new ArrayS2Node <T> (x,y+1,vIndex(x,y+1)); }
		else
		{ return getNextNeighbor(x,y); }
	}
	else if(neighborNumber==5) // top right
	{
		if(isSafe(x+1,y+1))
		{ return new ArrayS2Node <T> (x+1,y+1,vIndex(x+1,y+1)); }
		else
		{ return getNextNeighbor(x,y); }
	}
	else if(neighborNumber==6) // right
	{
		if(isSafe(x+1,y))
		{ return new ArrayS2Node <T> (x+1,y,vIndex(x+1,y)); }
		else
		{ return getNextNeighbor(x,y); }
	}
	else if(neighborNumber==7) // bottom right
	{
		if(isSafe(x+1,y-1))
		{ return new ArrayS2Node <T> (x+1,y-1,vIndex(x+1,y-1)); }
		else
		{ return getNextNeighbor(x,y); }
	}
	else if(neighborNumber==8) // bottom.
	{
		if(isSafe(x,y-1))
		{ return new ArrayS2Node <T> (x,y-1,vIndex(x,y-1)); }
		else
		{ return getNextNeighbor(x,y); }
	}
	
	neighborNumber=0;
	return 0; //if no more neighbors, return 0 and reset.
}

T neighborAverage(const unsigned int _x, const unsigned int _y) //021-082-0000
{ //includes diagonals.
	int totalSides = 0;
	T number;
	if(isSafeIndex(_x-1,_y-1))
	{
		++totalSides;
		number+=vIndex(_x-1,_y-1);
	}
	if(isSafeIndex(_x-1,_y))
	{
		++totalSides;
		number+=vIndex(_x-1,_y);
	}
	if(isSafeIndex(_x-1,_y+1))
	{
		++totalSides;
		number+=vIndex(_x-1,_y+1);
	}
	
	if(isSafeIndex(_x,_y-1))
	{
		++totalSides;
		number+=vIndex(_x-1,_y-1);
	}
	if(isSafeIndex(_x,_y+1))
	{
		++totalSides;
		number+=vIndex(_x-1,_y+1);
	}
	
	if(isSafeIndex(_x+1,_y-1))
	{
		++totalSides;
		number+=vIndex(_x+1,_y-1);
	}
	if(isSafeIndex(_x+1,_y))
	{
		++totalSides;
		number+=vIndex(_x+1,_y);
	}
	if(isSafeIndex(_x+1,_y+1))
	{
		++totalSides;
		number+=vIndex(_x+1,_y+1);
	}

	number/=totalSides;
	return number;
	
}

#endif