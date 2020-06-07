#pragma once
#ifndef WILDCAT_CONTAINER_ARRAYS2_ARRAYS2_FLOODFILL_HPP
#define WILDCAT_CONTAINER_ARRAYS2_ARRAYS2_FLOODFILL_HPP

/* ArrayS2_FloodFill.cpp
#include <Container/ArrayS2/ArrayS2_FloodFill.hpp>
Implementation of flood fill.
*/

// THIS FUNCTION RETURNS A VECTOR OF ALL THE COORDINATES WITHIN THE FLOOD FILL AREA.
// THIS PARTICULAR VERSION HAS NO TOLERANCE SETTING, IT ONLY CHECKS FOR EQUALITY.
// Later versions will allow to specify what value to fill and how to compare.

// I cleaned up the code and removed some redundant pushes to vector. This seems to have almost doubled
// performance. I should also try using a deque instead of vector.


Vector <HasXY*> * floodFillVector ( const int _startX, const int _startY, const bool includeDiagonals )
{
	const ARRAYS2_T initialValue = (*this)(_startX,_startY);

	// Vector of coordinates to return.
	Vector <HasXY*> * const vToFill = new Vector <HasXY*>;
	vToFill->reserve(nX+nY);

	/* vectors of coordinates to be checked (this can be changed to std::pair */
	Vector <HasXY*> vToCheck;
	vToCheck.reserve(nX+nY);

	int currentV=-1;
	int posX=_startX;
	int posY=_startY;

	ArrayS2 <bool> aChecked ( nX,nY,false );

	/* scanline fill loop */
	while (currentV!=vToCheck.size())
	{
		const int INITIAL_X=posX;
		/* go left to find the leftmost tile for this row which meets the criteria for water, filling along the way
		we also check the tile directly above or below to see if there are any tiles that can be filled on those rows.
		If we find such tiles, then we store the leftmost ones, for both the upper and lower row.
		NOTE: y coords seems to encode upside-down, so we need to work upside-down here. */

		/* Fill left */
		while ( posX>=0 && (*this)(posX,posY)==initialValue )
		{
			// Fill the coordinate we are currently on.
			vToFill->push(new HasXY(posX,posY));

			/* check below */
			/* If we're not on the bottom row, and the below tile is land. */
			if(posY<nY-1 && (*this)(posX,posY+1)==initialValue && aChecked(posX,posY+1)==false )
			{
				/* If we are at the leftmost or rightmost column, or we have hit the end of the fill line */
				if ( posX==0 || posX==nX-1 || (*this)(posX-1,posY+1)!=initialValue )
				{
					/* push tile below us if it's valid. */
					vToCheck.push(new HasXY(posX,posY+1));
					aChecked(posX,posY+1)=true;
				}
			}
			/* check above */
			/* If we're not on the top row, and the above tile is water. */
			if (posY>0 && (*this)(posX,posY-1)==initialValue && aChecked(posX,posY-1)==false )
			{
				/* If we are at the leftmost or rightmost column of the map. */
				if( posX==0 || posX==nX-1 || (*this)(posX-1,posY-1)!=initialValue )
				{
					/* push tile above us if it's valid. */
					vToCheck.push(new HasXY(posX,posY-1));
					aChecked(posX,posY-1)=true;
				}
			}
			--posX; /* move left 1 tile */
		}


		/* check the last X for this scan. */
		// NOTE: I THINK SWITCHING TO A DO WHILE MIGHT REMOVE THE NEED FOR THIS BIT.

		++posX;
		/* check below */
		/* If we're not on the bottom row, and the below tile is water, and the below tile hasn't been processed yet. */
		if(posY<nY-1 && (*this)(posX,posY+1)==initialValue && aChecked(posX,posY+1)==false )
		{
			/* push coords onto vectors. */
			vToCheck.push(new HasXY(posX,posY+1));
			aChecked(posX,posY+1)=true;
		}

		/* check above */
		/* If we're not on the top row, and the above tile is water, and the above tile hasn't been processed yet. */
		if(posY>0 && (*this)(posX,posY-1)==initialValue && aChecked(posX,posY-1)==false)
		{
			/* push coords onto vectors. */
			vToCheck.push(new HasXY(posX,posY-1));
			aChecked(posX,posY-1)=true;
		}

		if ( includeDiagonals )
		{
			// DIAGONALS: SAME BUT CHECK BELOW-LEFT
			if(posY<nY-1 && posX>0 && (*this)(posX-1,posY+1)==initialValue && aChecked(posX-1,posY+1)==false )
			{
				vToCheck.push(new HasXY(posX-1,posY+1));
				aChecked(posX-1,posY+1)=true;
			}
			// DIAGONALS: SAME BUT CHECK ABOVE-LEFT
			if(posY>0 && posX>0 && (*this)(posX-1,posY-1)==initialValue && aChecked(posX-1,posY-1)==false )
			{
				vToCheck.push(new HasXY(posX-1,posY-1));
				aChecked(posX-1,posY-1)=true;
			}
		}
		posX=INITIAL_X+1;


		/* Go back to the initial x position, and now fill right */
		while ( posX < nX && (*this)(posX,posY)==initialValue )
		{
			// Fill the coordinate we are currently on.
			vToFill->push(new HasXY(posX,posY));

			/* check below */
			/* If we're not on the bottom row, and the below tile is water. */
			if(posY<nY-1 && (*this)(posX,posY+1)==initialValue)
			{
				/* If we are at the leftmost or rightmost column of the map. */
				if ( aChecked(posX,posY+1)==false && (posX==0 || posX==nX-1 || (*this)(posX-1,posY+1)!=initialValue ) )
				{
					/* push tile below us */
					vToCheck.push(new HasXY(posX,posY+1));
					aChecked(posX,posY+1)=true;
				}
			}
			/* check above */
			/* If we're not on the top row, and the above tile is water. */
			if(posY>0 && (*this)(posX,posY-1)==initialValue )
			{
				/* If we are at the leftmost or rightmost column of the map. */
				if ( aChecked(posX,posY-1)==false && ( posX==0 || posX==nX-1 || (*this)(posX-1,posY-1)!=initialValue ) )
				{
					/* push tile above us. */
					vToCheck.push(new HasXY(posX,posY-1));
					aChecked(posX,posY-1)=true;
				}
			}
			/* move right 1 tile */
			++posX;
		}

		//KLUDGE. DO WHILE MAYBE IS SMARTER. MAYBE.
		--posX;

		if ( includeDiagonals )
		{
			// DIAGONALS: CHECK BELOW-RIGHT
			if(posY<nY-1 && posX<nX-1 && (*this)(posX+1,posY+1)==initialValue && aChecked(posX+1,posY+1) == false )
			{
				vToCheck.push(new HasXY(posX+1,posY+1));
				aChecked(posX+1,posY+1) = true;
			}

			// DIAGONALS: CHECK ABOVE-RIGHT
			if(posY>0 && posX<nX-1 && (*this)(posX+1,posY-1)==initialValue && aChecked(posX+1,posY-1) == 0 )
			{
				vToCheck.push(new HasXY(posX+1,posY-1));
				aChecked(posX+1,posY-1) = true;
			}
		}


		/* entire row is complete. Go to next set of coords. */
		/* Look for some good coords on the vector. */
		++currentV;

		if ( currentV < vToCheck.size() )
		{
			posX=vToCheck(currentV)->x;
			posY=vToCheck(currentV)->y;
		}
	}

	for ( int i=0; i<vToCheck.size(); ++i)
	{
		delete vToCheck(i);
	}

	return vToFill;
}

// This one hasn't been updated.
ArrayS2 <short int> * floodFillUniqueID ( const bool includeDiagonals, int* lastID )
{
	ArrayS2 <short int>* aID = new ArrayS2 <short int> (nX,nY,-1);
	short int currentID=0;

	for (int _y=0;_y<nY;++_y)
	{
		for (int _x=0;_x<nX;++_x) 
		{
			if ( (*aID)(_x,_y) == -1 )
			{
				ARRAYS2_T initialValue = (*this)(_x,_y);

				// Vector of coordinates to return.
				//Vector <HasXY*> * const vToFill = new Vector <HasXY*>;

				/* vectors of coordinates to be checked (this can be changed to std::pair */
				Vector <HasXY*> vToCheck;
				//vToCheck.reserve(nX);

				int currentV=-1;
				int posX=_x;
				int posY=_y;

				/* scanline fill loop */
				while (currentV!=vToCheck.size())
				{
					const int INITIAL_X=posX;
					/* go left to find the leftmost tile for this row which meets the criteria for water, filling along the way */
					/* we also check the tile directly above or below to see if there are any tiles that can be filled on those rows. */
					/* If we find such tiles, then we store the leftmost ones, for both the upper and lower row. */
					/* NOTE: y coords seems to encode upside-down, so we need to work upside-down here. */
					/* Fill left */
					// while ( posX>=0 && aID(posX,posY)==false && (*this)(posX,posY)==initialValue )
					while ( posX>=0 && (*this)(posX,posY)==initialValue && (*aID)(posX,posY)==-1)
					{
						(*aID)(posX,posY)=currentID;
						//vToFill->push(new HasXY(posX,posY));
						//(*aID)(posX,posY)=currentID;

						/* check below */
						/* If we're not on the bottom row, and the below tile is land. */
						if(posY<nY-1 && (*this)(posX,posY+1)==initialValue && (*aID)(posX,posY+1)==-1 )
						{
							/* If we are at the leftmost or rightmost column of the map. */
							if(posX==0 || posX==nX-1 )
							{
								/* push tile below us if it's valid. */
								//if(aID(posX,posY+1)==false)
								{
									vToCheck.push(new HasXY(posX,posY+1));
								}
							}
							/* if we aren't at the leftmost or rightmost column of the map. */
							else
							{
								/* if the bottom-left tile doesn't match, and the below tile has not been processed. */
								//if ( (*this)(posX-1,posY+1)!=initialValue && aID(posX,posY+1)==false )
								if ( (*this)(posX-1,posY+1)!=initialValue )
								{ 
									vToCheck.push(new HasXY(posX,posY+1));
								}
							}
						}
						/* check above */
						/* If we're not on the top row, and the above tile is water. */
						if(posY>0 && (*this)(posX,posY-1)==initialValue && (*aID)(posX,posY-1)==-1 )
						{
							/* If we are at the leftmost or rightmost column of the map. */
							if(posX==0 || posX==nX-1 )
							{
								/* push tile above us if it's valid. */
								//if(aID(posX,posY-1)==false)
								{
									vToCheck.push(new HasXY(posX,posY-1));
								}
							}
							/* if we aren't at the leftmost or rightmost column of the map. */
							else
							{
								/* if the top-left tile is land, and the above tile has not been processed. */
								//if ((*this)(posX-1,posY-1)!=initialValue && aID(posX,posY-1)==false)
								if ((*this)(posX-1,posY-1)!=initialValue )
								{
									vToCheck.push(new HasXY(posX,posY-1));
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
					if(posY<nY-1 && (*this)(posX,posY+1)==initialValue && (*aID)(posX,posY+1)==-1 )
					{
						/* push coords onto vectors. */
						vToCheck.push(new HasXY(posX,posY+1));
					}

					/* check above */
					/* If we're not on the top row, and the above tile is water, and the above tile hasn't been processed yet. */
					if(posY>0 && (*this)(posX,posY-1)==initialValue && (*aID)(posX,posY-1)==-1)
					{
						/* push coords onto vectors. */
						vToCheck.push(new HasXY(posX,posY-1));
					}

					if ( includeDiagonals )
					{
						// DIAGONALS: SAME BUT CHECK BELOW-LEFT
						if(posY<nY-1 && posX>0 && (*this)(posX-1,posY+1)==initialValue && (*aID)(posX-1,posY+1)==-1 )
						{
							vToCheck.push(new HasXY(posX-1,posY+1));
						}

						// DIAGONALS: SAME BUT CHECK ABOVE-LEFT
						if(posY>0 && posX>0 && (*this)(posX-1,posY-1)==initialValue && (*aID)(posX-1,posY-1)==-1 )
						{
							vToCheck.push(new HasXY(posX-1,posY-1));
						}
					}

					posX=INITIAL_X+1;


					/* Go back to the initial x position, and now fill right */
					//while ( posX < nX && (*this)(posX,posY)==initialValue && aID(posX,posY)==false )
					while ( posX < nX && (*this)(posX,posY)==initialValue )
					{
						(*aID)(posX,posY)=currentID;
						//vToFill->push(new HasXY(posX,posY));
						//++landMassSize;
						/* check below */
						/* If we're not on the bottom row, and the below tile is water. */
						if(posY<nY-1 && (*this)(posX,posY+1)==initialValue)
						{
							/* If we are at the leftmost or rightmost column of the map. */
							if(posX==0 || posX==nX-1 )
							{
								/* push tile below us if it's valid. */
								if((*aID)(posX,posY+1)==-1)
								{ vToCheck.push(new HasXY(posX,posY+1)); /* vX.push(posX); vY.push(posY+1); */ }
							}
							/* if we aren't at the leftmost or rightmost column of the map. */
							else
							{
								/* if the bottom-left tile is land, and the below tile has not been processed. */
								//if ( aIsLand(posX-1,posY+1)==false && aID(posX,posY+1)==false)
								if ( (*this)(posX-1,posY+1)!=initialValue && (*aID)(posX,posY+1)==-1)
								{ vToCheck.push(new HasXY(posX,posY+1)); /* vX.push(posX); vY.push(posY+1); */ }
							}
						}
						/* check above */
						/* If we're not on the top row, and the above tile is water. */
						//if(posY>0 && aIsLand(posX,posY-1)==true )
						if(posY>0 && (*this)(posX,posY-1)==initialValue )
						{
							/* If we are at the leftmost or rightmost column of the map. */
							if(posX==0 || posX==nX-1 )
							{
								/* push tile above us if it's valid. */
								if((*aID)(posX,posY-1)==-1)
								{ vToCheck.push(new HasXY(posX,posY-1)); /* vX.push(posX); vY.push(posY-1); */ }
							}
							/* if we aren't at the leftmost or rightmost column of the map. */
							else
							{
								/* if the top-left tile is land, and the above tile has not been processed. */
								//if ( aIsLand(posX-1,posY-1)==false && aID(posX,posY-1)==false )
								if ( (*this)(posX-1,posY-1)!=initialValue && (*aID)(posX,posY-1)==-1 )
								{ vToCheck.push(new HasXY(posX,posY-1)); /* vX.push(posX); vY.push(posY-1); */ }
							}
						}
						/* move right 1 tile */
						++posX;
					}

					//KLUDGE. DO WHILE MAYBE IS SMARTER. MAYBE.
					--posX;

					if ( includeDiagonals )
					{

						// DIAGONALS: CHECK BELOW-RIGHT
						//if(posY<nY-1 && posX<nX-1 && aIsLand(posX+1,posY+1)==true && aID(posX+1,posY+1)==false )
						if(posY<nY-1 && posX<nX-1 && (*this)(posX+1,posY+1)==initialValue && (*aID)(posX+1,posY+1)==-1 )
						{
							//vX.push(posX+1); vY.push(posY+1);
							vToCheck.push(new HasXY(posX+1,posY+1));
						}

						// DIAGONALS: CHECK ABOVE-RIGHT
						//if(posY>0 && posX<nX-1 && aIsLand(posX+1,posY-1)==true && aID(posX+1,posY-1)==false )
						if(posY>0 && posX<nX-1 && (*this)(posX+1,posY-1)==initialValue && (*aID)(posX+1,posY-1)==-1 )
						{
							//vX.push(posX+1); vY.push(posY-1);
							vToCheck.push(new HasXY(posX+1,posY-1));
						}

					}


					/* entire row is complete. Go to next set of coords. */
					/* Look for some good coords on the vector. */
					++currentV;


					for (;currentV<vToCheck.size();++currentV)
					{
						if ( (*aID)(vToCheck(currentV)) == -1 )
						{
							posX=vToCheck(currentV)->x;
							posY=vToCheck(currentV)->y;
							break;
						}
						else
						{
							//std::cout<<"skipped a check\n";
						}
					}
				}

				for ( int i=0; i<vToCheck.size(); ++i)
				{
					delete vToCheck(i);
				}
				++currentID;

			}
		}
	}

	*lastID = currentID-1;
	return aID;

}


#endif