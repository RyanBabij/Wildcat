#pragma once
#ifndef WILDCAT_BOARD_BOARD_HPP
#define WILDCAT_BOARD_BOARD_HPP

#include <Interface/HasXY.hpp>
#include <Container/ArrayS2/ArrayS2.hpp>
#include <Container/Vector/Vector.hpp>
//#include <Interface/HasTexture.hpp>

/*
	#include <Board/Board.hpp>
	
	A board is a special kind of array. Objects occupy a position on the board, and functions are provided to move them. There is also support for movement restrictions using another array.
	
	One complication is that there are 2D and 3D boards.
	
	
	The board uses an array of Vectors. Vectors allow for stacking.
	
	
	Travel rules:
	0000 0000 - Unrestricted movement.
	1000 0000 - Cannot move N.
	0100 0000 - Cannot move NE.
	0010 0000 - Cannot move E.
	0001 0000 - Cannot move SE.
	0000 1000 - Cannot move S.
	0000 0100 - Cannot move SW.
	0000 0010 - Cannot move W.
	0000 0001 - Cannot move NW.
	
	These bits can be combined. For example:
	
	0100 0110 - Cannot move NE, SW or W.
	
*/

class BoardObject: public HasXY
{
	public:
	// INT X, Y FROM HASXY.
	// setCoordinates (x,y) from HASXY
	
	BoardObject()
	{
	}
};


class Board
{
	public:
	ArrayS2 < Vector <BoardObject*> * > aBoardObject;
	
	ArrayS2 <char> aTravelRule;
	
	Board()
	{
	}
	
	void init(const int _size)
	{
		aBoardObject.init(_size,_size,0);
		aTravelRule.init(_size,_size,0);
		
		//for ( int 
	}
	void teleportObject(BoardObject* _boardObject, const int _teleportX, const int _teleportY)
	{
		
		_boardObject->setCoordinates(_teleportX,_teleportY);
		//aBoardObject->push(_boardObject);
	}
	void removeObject( BoardObject* _boardObject )
	{

		if ( aBoardObject.isSafe ( _boardObject ) )
		{
		}
	
	}
	// void moveObject (BoardObject* )
	// {
	// }
	
};


#endif