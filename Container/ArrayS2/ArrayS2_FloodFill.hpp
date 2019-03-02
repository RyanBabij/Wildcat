#pragma once
#ifndef WILDCAT_CONTAINER_ARRAYS2_ARRAYS2_FLOODFILL_HPP
#define WILDCAT_CONTAINER_ARRAYS2_ARRAYS2_FLOODFILL_HPP

/* ArrayS2_FloodFill.cpp
	#include <Container/ArrayS2/ArrayS2_FloodFill.hpp>
	Implementation of flood fill.
*/

	// THIS FUNCTION RETURNS A VECTOR OF ALL THE COORDINATES WITHIN THE FLOOD FILL AREA.
	// THIS PARTICULAR VERSION HAS NO TOLERANCE SETTING, IT ONLY CHECKS FOR EQUALITY.

// template <class ARRAYS2_T>
// void ArrayS2<ARRAYS2_T>::fill322(ARRAYS2_T value)
// {
	// currentElement=&data[0];
	// while(currentElement!=nullAddress)
	// {
		// *currentElement=value;
		// ++currentElement;
	// }
	// currentElement=&data[0];
// }

	// THIS FUNCTION RETURNS A VECTOR OF ALL THE COORDINATES WITHIN THE FLOOD FILL AREA.
	// THIS PARTICULAR VERSION HAS NO TOLERANCE SETTING, IT ONLY CHECKS FOR EQUALITY.
	// Later versions will allow to specify what value to fill and how to compare.

//#ifdef NOPE
	
//template <class ARRAYS2_T>
Vector <HasXY*> * floodFillVector ( const int _startX, const int _startY, const bool includeDiagonals )
{
	//std::cout<<"Fill called for coordinates: ("<<_startX<<", "<<_startY<<")\n";
	const ARRAYS2_T initialValue = (*this)(_startX,_startY);
	
	// Vector of coordinates to return.
	Vector <HasXY*> * const vToFill = new Vector <HasXY*>;

	/* vectors of coordinates to be checked (this can be changed to std::pair */
	Vector <HasXY*> vToCheck;
	//vToCheck.reserve(nX);

	int currentV=-1;
	int posX=_startX;
	int posY=_startY;
	
	ArrayS2 <bool> aChecked ( nX,nY,false );

	/* scanline fill loop */
	//while (true)
	while (currentV!=vToCheck.size())
	{
        const int INITIAL_X=posX;
        /* go left to find the leftmost tile for this row which meets the criteria for water, filling along the way */
        /* we also check the tile directly above or below to see if there are any tiles that can be filled on those rows. */
        /* If we find such tiles, then we store the leftmost ones, for both the upper and lower row. */
        /* NOTE: y coords seems to encode upside-down, so we need to work upside-down here. */
        /* Fill left */
       // while ( posX>=0 && aChecked(posX,posY)==false && (*this)(posX,posY)==initialValue )
        while ( posX>=0 && (*this)(posX,posY)==initialValue && aChecked(posX,posY)==false)
        {
			aChecked(posX,posY)=true;
			vToFill->push(new HasXY(posX,posY));

			/* check below */
			/* If we're not on the bottom row, and the below tile is land. */
			if(posY<nY-1 && (*this)(posX,posY+1)==initialValue && aChecked(posX,posY+1)==false )
			{

				/* If we are at the leftmost or rightmost column of the map. */
				if(posX==0 || posX==nX-1 )
				{
					/* push tile below us if it's valid. */
					//if(aChecked(posX,posY+1)==false)
					{
						vToCheck.push(new HasXY(posX,posY+1));
					}
				}
				/* if we aren't at the leftmost or rightmost column of the map. */
				else
				{
					/* if the bottom-left tile doesn't match, and the below tile has not been processed. */
					//if ( (*this)(posX-1,posY+1)!=initialValue && aChecked(posX,posY+1)==false )
					if ( (*this)(posX-1,posY+1)!=initialValue )
					{ 
						vToCheck.push(new HasXY(posX,posY+1));
					}
				}
			}
			/* check above */
			/* If we're not on the top row, and the above tile is water. */
			if(posY>0 && (*this)(posX,posY-1)==initialValue && aChecked(posX,posY-1)==false )
			{
				/* If we are at the leftmost or rightmost column of the map. */
				if(posX==0 || posX==nX-1 )
				{
					/* push tile above us if it's valid. */
					//if(aChecked(posX,posY-1)==false)
					{
						vToCheck.push(new HasXY(posX,posY-1));
					}
				}
				/* if we aren't at the leftmost or rightmost column of the map. */
				else
				{
					/* if the top-left tile is land, and the above tile has not been processed. */
					//if ((*this)(posX-1,posY-1)!=initialValue && aChecked(posX,posY-1)==false)
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
		if(posY<nY-1 && (*this)(posX,posY+1)==initialValue && aChecked(posX,posY+1)==false )
		{
			/* push coords onto vectors. */
			vToCheck.push(new HasXY(posX,posY+1));
		}

		/* check above */
		/* If we're not on the top row, and the above tile is water, and the above tile hasn't been processed yet. */
		if(posY>0 && (*this)(posX,posY-1)==initialValue && aChecked(posX,posY-1)==false)
		{
			/* push coords onto vectors. */
			vToCheck.push(new HasXY(posX,posY-1));
		}
		
		if ( includeDiagonals )
		{

			// DIAGONALS: SAME BUT CHECK BELOW-LEFT
			if(posY<nY-1 && posX>0 && (*this)(posX-1,posY+1)==initialValue && aChecked(posX-1,posY+1)==false )
			{
				vToCheck.push(new HasXY(posX-1,posY+1));
			}
			
			// DIAGONALS: SAME BUT CHECK ABOVE-LEFT
			if(posY>0 && posX>0 && (*this)(posX-1,posY-1)==initialValue && aChecked(posX-1,posY-1)==false )
			{
				vToCheck.push(new HasXY(posX-1,posY-1));
			}
		
		}

		posX=INITIAL_X+1;
		
		
		/* Go back to the initial x position, and now fill right */
		//while ( posX < nX && (*this)(posX,posY)==initialValue && aChecked(posX,posY)==false )
		while ( posX < nX && (*this)(posX,posY)==initialValue )
		{
			aChecked(posX,posY)=true;
			vToFill->push(new HasXY(posX,posY));
			//++landMassSize;
			/* check below */
			/* If we're not on the bottom row, and the below tile is water. */
			if(posY<nY-1 && (*this)(posX,posY+1)==initialValue)
			{
				/* If we are at the leftmost or rightmost column of the map. */
				if(posX==0 || posX==nX-1 )
				{
					/* push tile below us if it's valid. */
					if(aChecked(posX,posY+1)==false)
					{ vToCheck.push(new HasXY(posX,posY+1)); /* vX.push(posX); vY.push(posY+1); */ }
				}
				/* if we aren't at the leftmost or rightmost column of the map. */
				else
				{
					/* if the bottom-left tile is land, and the below tile has not been processed. */
					//if ( aIsLand(posX-1,posY+1)==false && aChecked(posX,posY+1)==false)
					if ( (*this)(posX-1,posY+1)!=initialValue && aChecked(posX,posY+1)==false)
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
					if(aChecked(posX,posY-1)==false)
					{ vToCheck.push(new HasXY(posX,posY-1)); /* vX.push(posX); vY.push(posY-1); */ }
				}
				/* if we aren't at the leftmost or rightmost column of the map. */
				else
				{
					/* if the top-left tile is land, and the above tile has not been processed. */
					//if ( aIsLand(posX-1,posY-1)==false && aChecked(posX,posY-1)==false )
					if ( (*this)(posX-1,posY-1)!=initialValue && aChecked(posX,posY-1)==false )
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
			//if(posY<nY-1 && posX<nX-1 && aIsLand(posX+1,posY+1)==true && aChecked(posX+1,posY+1)==false )
			if(posY<nY-1 && posX<nX-1 && (*this)(posX+1,posY+1)==initialValue && aChecked(posX+1,posY+1)==false )
			{
				//vX.push(posX+1); vY.push(posY+1);
				vToCheck.push(new HasXY(posX+1,posY+1));
			}

			// DIAGONALS: CHECK ABOVE-RIGHT
			//if(posY>0 && posX<nX-1 && aIsLand(posX+1,posY-1)==true && aChecked(posX+1,posY-1)==false )
			if(posY>0 && posX<nX-1 && (*this)(posX+1,posY-1)==initialValue && aChecked(posX+1,posY-1)==false )
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
			if ( aChecked(vToCheck(currentV)) == false )
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

		// if(currentV==vToCheck.size())
		// {
			// break;
		// }

	}
	
	for ( int i=0; i<vToCheck.size(); ++i)
	{
		delete vToCheck(i);
	}

	return vToFill;
	
}

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
	
	//std::cout<<"Fill called for coordinates: ("<<_startX<<", "<<_startY<<")\n";
	ARRAYS2_T initialValue = (*this)(_x,_y);
	
	// Vector of coordinates to return.
	//Vector <HasXY*> * const vToFill = new Vector <HasXY*>;

	/* vectors of coordinates to be checked (this can be changed to std::pair */
	Vector <HasXY*> vToCheck;
	//vToCheck.reserve(nX);

	int currentV=-1;
	int posX=_x;
	int posY=_y;
	

	

	
	//ArrayS2 <bool> aID ( nX,nY,false );

	/* scanline fill loop */
	//while (true)
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

		// if(currentV==vToCheck.size())
		// {
			// break;
		// }

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

// This flood fill will fill all connected areas with unique values. It is faster because it only needs 1 pass.


//New idea: Traditional flood fill. HOWEVER, on each flood, save the first non-connected tile you find, and after fill is finished, increment id and go again.

// class IDNode
// {
	// public:
	// int value;
	
	// static int linksPushed;
	
	// IDNode* parentLink;
	
	// Vector <IDNode*> subLinks;
	
	// IDNode(int v)
	// {
		// value=v;
		// parentLink=0;
	// }
	
	// void link(IDNode* idLink)
	// {
		// //std::cout<<",";
		// if ( idLink->value < value && subLinks.contains(idLink) == false)
		// {
			// //std::cout<<".";
			// subLinks.push(idLink);
			
			// for (int i=0;i<subLinks.size();++i)
			// {
				// subLinks(i)->link(idLink);
			// }
		// }
		
		// if ( parentLink==0 && idLink->value > value)
		// {
			// parentLink=idLink;
		// }
		// else if ( parentLink!=0 && idLink->value > parentLink->value )
		// {
			// parentLink->link(idLink);
			// parentLink=idLink;
		// }
		
		// // if (lowestLink==0)
		// // {
			// // if ( idLink->value < value )
			// // {
				// // lowestLink = idLink;
			// // }
		// // }
		// // else if (idLink->value < lowestLink->value)
		// // {
			// // lowestLink->link(idLink);
			// // lowestLink = idLink;
		// // }
	// }
	
	// int getSuperLowestValue()
	// {
		// return getSuperParent()->getLowestValue();
	// }
	
	// int getLowestValue()
	// {
		// //return getSuperParent->
		// //if ( parentLink==0)
		// //{
			// int lowestV = value;
			
			// for (int i=0;i<subLinks.size();++i)
			// {
				// int subLowest = subLinks(i)->getLowestValue();
				// if (subLowest < lowestV ) {lowestV = subLowest;}
			// }
			// return lowestV;
		// //}

			// //return parentLink->getLowestValue();
			// //return 0;

	// }
	
	// IDNode* getSuperParent()
	// {
		// if (parentLink==0)
		// {
			// return this;
		// }
		// return parentLink->getSuperParent();
	// }
	
	// // IDNode* getLowestLink()
	// // {
		// // //if (lowestLink==0) { return this; }
		// // //return lowestLink;
	// // }
	

// };

// This ended up being much less elegant than I was hoping.

//template <class ARRAYS2_T>
// ArrayS2 <int> * floodFillUnique ( bool includeDiagonals=false, int * maxID=0 )
// {
	// if ( nX < 1 || nY < 1 )
	// { return 0; }
	
	// int currentID = 0;
	// int highestID = 0;

	
	// // Keep track of linked ids, to later condense into single values. Maybe use vector of tuples or something.
	
	// ArrayS2 <int> * idFill = new ArrayS2 <int> (nX,nY,-1);
	
	// ArrayS2 <IDNode*> idNodeFill (nX,nY,0);

	
	// IDNode idNode (0);
	// Vector <IDNode*> vNodesToDelete;
	
	// (*idFill)(0,0) = 0;
	// idNodeFill(0,0) = &idNode;
	
	// //base case: top row. No comparisons to other rows can be made.
	// for (int _x=1;_x<nX;++_x)
	// {
		// if ( (*this)(_x-1,0) == (*this)(_x,0) )
		// {
			// //(*idFill)(_x,0) = (*idFill)(_x-1,0);
			// idNodeFill(_x,0) = idNodeFill (_x-1,0);
		// }
		// else
		// {
			// //++currentID;
			
			// //(*idFill)(_x,0) = currentID;
			
			// IDNode* newNode = new IDNode(++currentID);
			// idNodeFill(_x,0) = newNode;
			// vNodesToDelete.push(newNode);
			
		// }
	// }
	
	// // Base case: leftmost column. No comparisons to other columns can be made. Only can compare with above.
	// for (int _y=1;_y<nY;++_y)
	// {
		// if ( (*this)(0,_y-1) == (*this)(0,_y) )
		// {
			// //(*idFill)(0,_y) = (*idFill)(0,_y-1);
			// idNodeFill(0,_y) = idNodeFill (0,_y-1);
		// }
		// else
		// {
			// //++currentID;
			
			// //(*idFill)(0,_y) = currentID;
			
			// IDNode* newNode = new IDNode(++currentID);
			// idNodeFill(0,_y) = newNode;
			// vNodesToDelete.push(newNode);
		// }
	// }
	
	// // Now all comparisons can be made.
	// int i2=0;
	
	// for (int _y=1;_y<nY;++_y)
	// {
		// for (int _x=1;_x<nX;++_x)
		// {
			// if ( (*this)(_x-1,_y) == (*this)(_x,_y) )
			// {
				// //(*idFill)(_x,_y) = (*idFill)(_x-1,_y);
				// idNodeFill(_x,_y) = idNodeFill (_x-1,_y);
				
				// if ( (*this)(_x,_y-1) == (*this)(_x,_y) && idNodeFill(_x,_y) != idNodeFill(_x,_y-1))
				// {
					// //if ( (*this)(_x,_y-1)->value < (*this)(_x,_y) )
					// //{
						
					// //}
					
					// //idNodeFill(_x,_y-1)->value = idNodeFill (_x,_y)->value;
					// //idNodeFill(_x,_y)->value = idNodeFill (_x,_y-1)->value;
					// idNodeFill(_x,_y)->link(idNodeFill(_x,_y-1));
					// idNodeFill(_x,_y-1)->link(idNodeFill(_x,_y));
					
					// //if(i2++<10)
					// {
						// //std::cout<<"linked: "<<_x<<","<<_y<<" ("<<idNodeFill(_x,_y)->value<<") with "<<_x<<","<<_y-1<<" ("<<idNodeFill(_x,_y-1)->value<<").\n";
					// }
				// }
				
			// }
			// else if ( (*this)(_x,_y-1) == (*this)(_x,_y) )
			// {
			// //	//(*idFill)(_x,_y) = (*idFill)(_x,_y-1);
				// idNodeFill(_x,_y) = idNodeFill (_x,_y-1);
			// }
			// else
			// {
				// //(*idFill)(_x,_y) = ++currentID;
				
				// IDNode* newNode = new IDNode(++currentID);
				// idNodeFill(_x,_y) = newNode;
				// vNodesToDelete.push(newNode);
			// }
			
			// //idNodeFill(_x,_y)->value = idNodeFill(_x-1,_y)->value;
			// //idNodeFill(_x,_y)->value = idNodeFill(_x,_y-1)->value;
			
		// }
	// }
	
	
	// // Now we sweep backwards through the array to complete the final id matches
	// for (int _y=nY-1;_y>=0;--_y)
	// {
		// //for (int _x=nX-1;_x>=0;--_x)
		// //for (int _x=1;_x<nX;++_x)
		// {
			// //if ( (*this)(_x+1,_y) == (*this)(_x,_y) )
			// {
				// //idNodeFill(_x+1,_y)->value = idNodeFill (_x,_y)->value;
				
				// //if ( (*this)(_x,_y+1) == (*this)(_x,_y) )
				// {
					// //idNodeFill(_x,_y+1)->value = idNodeFill (_x,_y)->value;
				// }
			// }

		// }
	// }
	
	
		// // Get final values into array.
	// for (int _y=0;_y<nY;++_y)
	// {
		// for (int _x=0;_x<nX;++_x)
		// {
		// //	(*idFill)(_x,_y)=idNodeFill(_x,_y)->value;
		// //std::cout<<"[]";
				// (*idFill)(_x,_y)=idNodeFill(_x,_y)->getLowestValue();
				// (*idFill)(_x,_y)=idNodeFill(_x,_y)->getSuperLowestValue();
				// //(*idFill)(_x,_y)=0;
				// //(*idFill)(_x,_y)=0;
			
		// }
	// }
	
	// if(maxID !=0)
	// {
		// *maxID=currentID;
	// }
	
	// for ( int i=0;i<vNodesToDelete.size();++i)
	// {
		// delete vNodesToDelete(i);
	// }
	
	// return idFill;
	
	
// }


#endif