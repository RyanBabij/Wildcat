#pragma once
#ifndef WILDCAT_CONTAINER_ARRAYS2_ARRAYS2_FLOODFILL_HPP
#define WILDCAT_CONTAINER_ARRAYS2_ARRAYS2_FLOODFILL_HPP

/* ArrayS2_FloodFill.cpp
	Implementation of flood fill.
*/

	// THIS FUNCTION RETURNS A VECTOR OF ALL THE COORDINATES WITHIN THE FLOOD FILL AREA.
	// THIS PARTICULAR VERSION HAS NO TOLERANCE SETTING, IT ONLY CHECKS FOR EQUALITY.

template <class ARRAYS2_T>
void ArrayS2<ARRAYS2_T>::fill322(ARRAYS2_T value)
{
	currentElement=&data[0];
	while(currentElement!=nullAddress)
	{
		*currentElement=value;
		++currentElement;
	}
	currentElement=&data[0];
}

#ifdef NOPE
	
template <class ARRAYS2_T>
Vector <HasXY> * floodFillVector ( const int _startX, const int _startY, bool includeDiagonals )
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
			vToFill.push(new HasXY(posX,posY));

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
		if(posY>0 && aIsLand(posX,posY-1)==true && aLandmassID(posX,posY-1)==-1)
		{
			/* push coords onto vectors. */
			vX.push(posX); vY.push(posY-1);
		}

		// DIAGONALS: SAME BUT CHECK BELOW-LEFT
		if(posY<nY-1 && posX>0 && aIsLand(posX-1,posY+1)==true && aLandmassID(posX-1,posY+1)==-1 )
		{
			vX.push(posX-1); vY.push(posY+1);
		}
		
		// DIAGONALS: SAME BUT CHECK ABOVE-LEFT
		if(posY>0 && posX>0 && aIsLand(posX-1,posY-1)==true && aLandmassID(posX-1,posY-1)==-1 )
		{
			vX.push(posX-1); vY.push(posY-1);
		}

		posX=INITIAL_X+1;
		
		/* Go back to the initial x position, and now fill right */
		while ( posX < aIsLand.nX && aIsLand(posX,posY)==true && aLandmassID(posX,posY)==-1 )
		{
			aLandmassID(posX,posY)=currentID;
			++landMassSize;
			/* check below */
			/* If we're not on the bottom row, and the below tile is water. */
			if(posY<aIsLand.nY-1 && aIsLand(posX,posY+1)==true )
			{
			/* If we are at the leftmost or rightmost column of the map. */
			if(posX==0 || posX==aIsLand.nX-1 )
			{
			/* push tile below us if it's valid. */
			if(aLandmassID(posX,posY+1)==-1)
			{ vX.push(posX); vY.push(posY+1); }
			}
			/* if we aren't at the leftmost or rightmost column of the map. */
			else
			{
			/* if the bottom-left tile is land, and the below tile has not been processed. */
			if ( aIsLand(posX-1,posY+1)==false && aLandmassID(posX,posY+1)==-1 )
			{ vX.push(posX); vY.push(posY+1); }
			}
			}
			/* check above */
			/* If we're not on the top row, and the above tile is water. */
			if(posY>0 && aIsLand(posX,posY-1)==true )
			{
			/* If we are at the leftmost or rightmost column of the map. */
			if(posX==0 || posX==aIsLand.nX-1 )
			{
			/* push tile above us if it's valid. */
			if(aLandmassID(posX,posY-1)==-1)
			{ vX.push(posX); vY.push(posY-1);
			}
			}
			/* if we aren't at the leftmost or rightmost column of the map. */
			else
			{
			/* if the top-left tile is land, and the above tile has not been processed. */
			if ( aIsLand(posX-1,posY-1)==false && aLandmassID(posX,posY-1)==-1 )
			{ vX.push(posX); vY.push(posY-1); }
			}
			}
			/* move right 1 tile */
			++posX;
		}

		//KLUDGE. DO WHILE MAYBE IS SMARTER. MAYBE.
		--posX;
		
		// DIAGONALS: CHECK BELOW-RIGHT
		if(posY<aIsLand.nY-1 && posX<aIsLand.nX-1 && aIsLand(posX+1,posY+1)==true && aLandmassID(posX+1,posY+1)==-1 )
		{
			vX.push(posX+1); vY.push(posY+1);
		}

		// DIAGONALS: CHECK ABOVE-RIGHT
		if(posY>0 && posX<aIsLand.nX-1 && aIsLand(posX+1,posY-1)==true && aLandmassID(posX+1,posY-1)==-1 )
		{
			vX.push(posX+1); vY.push(posY-1);
		}

		/* entire row is complete. Go to next set of coords. */
		/* Look for some good coords on the vector. */
		++currentV;

		for (;currentV<vX.size();++currentV)
		{
			if ( aLandmassID(vX(currentV),vY(currentV)) == -1 )
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
}

#endif

#endif