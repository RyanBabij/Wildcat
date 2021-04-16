#pragma once
#ifndef WILDCAT_GRAPHICS_COLOUR_COLOUR_HPP
#define WILDCAT_GRAPHICS_COLOUR_COLOUR_HPP

/* Wildcat: Graphics_Colour.hpp
	#include <Graphics/Colour/Colour.hpp>
	
	Class to store colour data. Collections of colours should use a ColourManager container.
	
	This class has been replaced. I'm just keeping it around in case I broke something.
*/

#include <Math/Random/RandomInterface.hpp>
#include <Data/DataTools.hpp>

class Colour
{
	public:
	unsigned char red, green, blue, alpha;
	
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
	
	// set a random colour within given ranges
	void setRandomRange(unsigned char minRed, unsigned char maxRed, unsigned char minGreen, unsigned char maxGreen,
		unsigned char minBlue, unsigned char maxBlue, RandomInterface &rng)
	{
		set(rng.range32(minRed,maxRed), rng.range32(minGreen,maxGreen), rng.range32(minBlue,maxBlue));
	}
	// set to a completely random colour
	void setRandom(RandomInterface& rng)
	{
		set(rng.rand8(), rng.rand8(), rng.rand8());
	}
	
	void setANSI(unsigned char _value, bool _boldModifier=false)
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
		
		unsigned char _intensity;
		_boldModifier? _intensity=255 : _intensity=128;
		
		switch(_value)
		{
			case 30: //FOREGROUND_BLACK
				set(0,0,0,255);
				break;
			case 31: //FOREGROUND_RED
				set(_intensity,0,0,255);
				break;
			case 32: //FOREGROUND_GREEN
				set(0,_intensity,0,255);
				break;
			case 33: //FOREGROUND_YELLOW
				set(_intensity,_intensity,0,255);
				break;
			case 34: //FOREGROUND_BLUE
				set(0,0,_intensity,255);
				break;
			case 35: //FOREGROUND_MAGENTA
				set(_intensity,0,_intensity,255);
				break;
			case 36: //FOREGROUND_CYAN
				set(0,_intensity,_intensity,255);
				break;
			case 37: //FOREGROUND_WHITE
				set(_intensity,_intensity,_intensity,255);
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
	
	int distanceTo(Colour* colour)
	{
		// sum of absolute diffs between all colours
		int rDiff = abs( red - colour->red );
		int gDiff = abs( green - colour->green );
		int bDiff = abs( blue - colour->blue );
		
		return rDiff+gDiff+bDiff;
	}
	
	std::string toString()
	{
		return DataTools::toString((int)red) + ", " + DataTools::toString((int)green) + ", " +
		DataTools::toString((int)blue);
	}
};



#endif
