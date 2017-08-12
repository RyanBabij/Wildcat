/* Graphics_Colour.hpp
#include <Graphics/Colour/Colour.hpp>
	Easier management of colours.
	
	// NOTE: CONSIDER STD::TUPLE.
*/
	
#pragma once
	
class Colour
{
	public:
	unsigned char red, green, blue;
	
	Colour (const unsigned char r, const unsigned char g, const unsigned char b)
	{
		red=r;
		green=g;
		blue=b;
	}
	Colour()
	{
		/* Default to white. */
		red=255;
		green=255;
		blue=255;
	}
	
	void set (const unsigned char r, const unsigned char g, const unsigned char b)
	{
		red=r;
		green=g;
		blue=b;
	}
	void set(const Colour* colour)
	{
		if(colour!=0)
		{
			red=colour->red;
			green=colour->green;
			blue=colour->blue;
		}
	}
	void set(const Colour colour)
	{
		red=colour.red;
		green=colour.green;
		blue=colour.blue;
	}
	
	/* Copy the rgb values from another colour object. */
	void copy ( Colour* colour )
	{
		if (colour!=0)
		{
			red=colour->red;
			green=colour->green;
			blue=colour->blue;
		}
	}
	void copy ( Colour colour )
	{
		red=colour.red;
		green=colour.green;
		blue=colour.blue;
	}
	
};
