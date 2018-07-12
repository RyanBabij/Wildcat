#pragma once

/* Region.hpp
	Makes UI code easier. Allows quick checking to see if the mouse lies within a region. Regionmanager makes it easier to manage menus and submenus.
*/


#include <Container/Vector/Vector.hpp>

namespace Wildcat
{
	// REGION conflicts with an X11 class...


/* Regions are defined areas of the screen. They make it easier to tell if a button is being clicked. */
class Region
{
	public:
	int x1, y1, x2, y2;

		// DEACTIVATED REGION WILL NEVER RETURN TRUE.
	bool active;
	
	Region (const int _x1, const int _y1, const int _x2, const int _y2)
	{
		set(_x1,_y1,_x2,_y2);
		active = true;
	}
	void set(const int _x1, const int _y1, const int _x2, const int _y2)
	{
		/* Automatically sort the coordinates so that you can't make a bad region by putting the coords in backwards. */
		if(_x1<=_x2)
		{ x1=_x1; x2=_x2; }
		else
		{ x1=_x2; x2=_x1; }
		if(_y1<=_y2)
		{ y1=_y1; y2=_y2; }
		else
		{ y1=_y2; y2=_y1; }
	}
	Region()
	{
		set(-1,-1,-1,-1);
		active=true;
	}
	
	bool liesOn(const int x, const int y)
	{
		if ( active == false ) { return false; }
		return(x>=x1&&x<=x2&&y>=y1&&y<=y2);
	}
	
	void move (const int moveX, const int moveY)
	{ x1+=moveX; x2+=moveX; y1+=moveY; y2+=moveY; }
	
};

/* Keeps a vector of regions. This makes it easier to handle a group of similar buttons. The vector index of the region is used as the ID for the region. -1 means that no region lies within the coordinates. */
class RegionManager
{
	public:
	Vector <Region*> vRegion;
	
	int addRegion(Region* region)
	{ vRegion.push(region); return vRegion.size()-1;
	}
	
	int addRegion (const int _x1, const int _y1, const int _x2, const int _y2)
	{ vRegion.push(new Region (_x1,_y1,_x2,_y2)); return vRegion.size()-1;
	}
	
	int checkRegions(const int _x, const int _y)
	{
		for (int i=0;i<vRegion.size();++i)
		{
			if (vRegion(i)->liesOn(_x,_y))
			{ return i; }
		}
		return -1;
	}
	
	void clearMemory()
	{
		for (int i=0;i<vRegion.size();++i)
		{
			delete vRegion(i);
			vRegion.clear();
		}
	}
	
};

}
