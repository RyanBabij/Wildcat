/*
	ArrayS3_Geometry.hpp



*/



// 2 POINTS ARE PASSED. A LIST OF TILES WHICH THE LINE PASSES THROUGH ARE RETURNED.

Vector <HasXY*> * raytrace ( int _x1, int _y1, const int _x2, const int _y2 )
{
	Vector <HasXY*> * vXY = new Vector <HasXY*>;

	int xDiff = 0;
	if ( _x1 > _x2 )
	{ xDiff = _x2 - _x1; }
	else if ( _x2 > _x1 )
	{ xDiff = _x1 - _x2; }

	int yDiff = 0;
	if ( _y1 > _y2 )
	{ yDiff = _y2 - _y1; }
	else if ( _y2 > _y1 )
	{ yDiff = _y1 - _y2; }

	double slope = BasicMath::getSlope(_x1,_y1,_x2,_y2);

		// IF SLOPE IS INFINITY, CHANGE VALUE TO 0.
	if ( slope == std::numeric_limits<double>::infinity() )
	{ slope=0; }

		// SPECIAL CASE: 1 TILE.
	if ( (_x1==_x2) && (_y1==_y2) )
	{
		// REVEAL TILE STANDING ON.
		if ( isSafe(_x1,_y2) == true )
		{
			vXY->push( new HasXY (_x1,_y2) );
		}
	}
		// SPECIAL CASE: UP/DOWN
	else if (_x1 == _x2)
	{
		std::cout<<"Special case: up/down.\n";
		while ( _y1 != _y2 )
		{
			if ( isSafe(_x1,_y1) == true )
			{
				vXY->push( new HasXY (_x1,_y1) );
			}
			if ( _y1 < _y2 )
			{ ++_y1; }
			else
			{ --_y1; }
		}
	}
		//SHALLOW SLOPE.
	else if ( xDiff <= yDiff )
	{
		if (_x1>_x2)
		{ slope*=-1; }

		std::cout<<"Slope: "<<slope<<".\n";

		double currentY = _y1;

		while (_x1 != _x2 )
		{
			//const int roundedY = BasicMath::roundOff(currentY);

			int roundedY=0;

			if ( _y1 > _y2 )
			{
				roundedY = floor(currentY);
			}
			else if ( _y1 < _y2 )
			{
				roundedY = ceil(currentY);
			}
			else
			{
				roundedY = round(currentY);
			}

			std::cout<<"Roundingy: "<<currentY<<"->"<<roundedY<<".\n";
			std::cout<<"coord: "<<_x1<<","<<roundedY<<"\n";

			if ( isSafe(_x1,roundedY) == true )
			{
				//aFogOfWar (_x1,roundedY) = fogMode;
				(*this)(_x1,roundedY) = 2;


				//if ( fogMode==false )
				//{
					//Vector <BattlescapeObject*> * vObj = aObjects(_x1,roundedY,1);
					//for ( int i=0;i<vObj->size();++i)
					//{
					//	if ( (*vObj)(i)->blockLOS==true)
					//	{
							// DONT OVERWRITE REVEALED TILES WITH FOG
					//		return;
					//		fogMode=true;
					//	}
					//}
				//}

				currentY+=slope;

			}
			if (_x1<_x2)
			{ ++_x1; }
			else
			{ --_x1; }
		}
	}
		//STEEP SLOPE.
	else
	{
		slope = BasicMath::getSlope(_y1,_x1,_y2,_x2);

		if (_y1>_y2)
		{ slope*=-1; }

		std::cout<<"Slope: "<<slope<<".\n";

		double currentX = _x1;

		while (_y1 != _y2 )
		{
			std::cout<<".";

			int roundedX=0;

			if ( _x1 > _x2 )
			{ roundedX = floor(currentX); }
			else if ( _x1 < _x2 )
			{ roundedX = ceil(currentX); }
			else
			{ roundedX = round(currentX); }

			//std::cout<<"Roundingy: "<<currentY<<"->"<<roundedY<<".\n";
			//std::cout<<"coord: "<<_x1<<","<<roundedY<<"\n";

			if ( isSafe(roundedX,_y1) == true )
			{
				//aFogOfWar (roundedX,_y1) = fogMode;
				(*this) (roundedX,_y1) = 2;


				//if ( fogMode==false )
				//{
				//	Vector <BattlescapeObject*> * vObj = aObjects(roundedX,_y1,1);
				//	for ( int i=0;i<vObj->size();++i)
				//	{
				//		if ( (*vObj)(i)->blockLOS==true)
				//		{
				//			// DONT OVERWRITE REVEALED TILES WITH FOG
				//			return;
				//			fogMode=true;
				//		}
				//	}
				//}

				currentX+=slope;

			}
			if (_y1<_y2)
			{ ++_y1; }
			else
			{ --_y1; }
		}
	}
	return vXY;
}

void test()
{
	std::cout<<"Test.\n";
}