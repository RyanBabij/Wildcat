/* Graphics_Colour.hpp
#include <Graphics/Colour/Colour.hpp>
	Easier management of colours.
	
	// NOTE: CONSIDER STD::TUPLE.
*/
	
#pragma once
	
class Colour
{
	public:
	unsigned char red, green, blue,alpha;
	
	Colour (const unsigned char r, const unsigned char g, const unsigned char b, unsigned char a=255)
	{
		red=r;
		green=g;
		blue=b;
		alpha=a;
	}
	Colour()
	{
		/* Default to white. */
		red=255;
		green=255;
		blue=255;
		alpha=255;
	}
	
	void set (const unsigned char r, const unsigned char g, const unsigned char b,const unsigned char a=255)
	{
		red=r;
		green=g;
		blue=b;
		alpha=a;
	}
	void set(const Colour* colour)
	{
		if(colour!=0)
		{
			red=colour->red;
			green=colour->green;
			blue=colour->blue;
			alpha=colour->alpha;
		}
	}
	void set(const Colour colour)
	{
		red=colour.red;
		green=colour.green;
		blue=colour.blue;
		alpha=colour.alpha;
	}
	
	void setANSI(unsigned char _value)
	{
		/*
			FOREGROUND_BLACK 30
			FOREGROUND_RED 31
			FOREGROUND_GREEN 32
			FOREGROUND_YELLOW 33
			FOREGROUND_BLUE 34
			FOREGROUND_MAGENTA 35
			FOREGROUND_CYAN 36
			FOREGROUND_WHITE 37
		*/
		switch(_value)
		{
			case 30: //FOREGROUND_BLACK
				set(0,0,0,255);
				break;
			case 31: //FOREGROUND_RED
				set(255,0,0,255);
				break;
			case 32: //FOREGROUND_GREEN
				set(0,255,0,255);
				break;
			case 33: //FOREGROUND_YELLOW
				set(255,255,0,255);
				break;
			case 34: //FOREGROUND_BLUE
				set(0,0,255,255);
				break;
			case 35: //FOREGROUND_MAGENTA
				set(255,0,255,255);
				break;
			case 36: //FOREGROUND_CYAN
				set(0,255,255,255);
				break;
			case 37: //FOREGROUND_WHITE
				set(255,255,255,255);
				break;
				default: set(255,255,255,255);
		}
		
	}
	
	/* Copy the rgb values from another colour object. */
	void copy ( Colour* colour )
	{
		if (colour!=0)
		{
			red=colour->red;
			green=colour->green;
			blue=colour->blue;
			alpha=colour->alpha;
		}
	}
	void copy ( Colour colour )
	{
		red=colour.red;
		green=colour.green;
		blue=colour.blue;
		alpha=colour.alpha;
	}
	
};
