#pragma once
#ifndef WILDCAT_GRAPHICS_COLOUR_COLOUR_2_HPP
#define WILDCAT_GRAPHICS_COLOUR_COLOUR_2_HPP

/* Wildcat: Graphics_Colour2.hpp
	#include <Graphics/Colour/Colour.hpp>
	
	Colour but an attempt to do it properly using polymorphism and templates, about as complex as things get.
	
	The reason it's complicated is because we have 2 types of polymorphism going on:
		* Colours might be RGB, RGBA or other formats with different amount of variables.
		* Colour values may be different datatypes.
		
	The solution to the first issue is to use polymorphism. For example create a ColourRGB base class with just the
	red, green and blue variables. Then create a ColourRGBA class which derives from ColourRGB and adds an alpha channel.
	
	The solution to the second issue is of course templates, allowing the user to specify what datatype they will be
	using, for example ColourRGB <unsigned char> colour;
	
	Therefore to effectively implement this, we must use both polymorphism and templates, which can get slightly
	confusing.
	
	Todo: Look into using std::tuple
*/

#include <Math/Random/RandomInterface.hpp>
#include <Data/DataTools.hpp>
#include <Container/Vector/Vector.hpp>

// Highest base class. Basically RGB colour.
template <class T>
class ColourRGB
{
	public:
	T red, green, blue;
	
	ColourRGB(const T _red=255, const T _green=255, const T _blue=255)
	{
		set(_red,_green,_blue);
	}
	
	void set(const T _red=255, const T _green=255, const T _blue=255)
	{
		red = _red;
		green = _green;
		blue = _blue;
	}
	void set(const ColourRGB& colour)
	{ set (colour.red,colour.green,colour.blue); }
	void copy(const ColourRGB& colour)
	{ set(colour); }
	
	// set to a completely random colour
	void setRandom(RandomInterface& rng)
	{
		set(rng.rand8(), rng.rand8(), rng.rand8());
	}
	
	// set a random colour within given ranges
	void setRandomRange(const unsigned char minRed, const unsigned char maxRed, const unsigned char minGreen,
		const unsigned char maxGreen, const unsigned char minBlue, const unsigned char maxBlue,
		RandomInterface &rng)
	{
		set(rng.range32(minRed,maxRed), rng.range32(minGreen,maxGreen), rng.range32(minBlue,maxBlue));
	}
	
	unsigned int distanceTo(const ColourRGB& colour)
	{
		// sum of absolute diffs between all colours
		return abs(red-colour.red)+abs(green-colour.green)+abs(blue-colour.blue);
	}
	
	unsigned int distanceTo(const ColourRGB* colour)
	{
		if (colour==0) // handle null ptr
		{ return UINT_MAX; }
		// sum of absolute diffs between all colours
		return abs(red-colour->red)+abs(green-colour->green)+abs(blue-colour->blue);
	}
	
	std::string toString()
	{
		return DataTools::toString((int)red) + ", " + DataTools::toString((int)green) + ", " +
		DataTools::toString((int)blue);
	}
};

// Add alpha
// Note alpha values here.
template <class T>
class ColourRGBA: public ColourRGB <T>
{
	public:
	T alpha;
	
	ColourRGBA(const T _red=255, const T _green=255, const T _blue=255, const T _alpha=255):
		ColourRGB <T>(_red,_green,_blue)
	{
		alpha = _alpha;
	}
	
	void set(const T _red=255, const T _green=255, const T _blue=255, const T _alpha=255)
	{
		ColourRGB <T>::set(_red,_green,_blue);
		alpha = _alpha;	
	}
	void set (ColourRGBA <unsigned char>& colour)
	{
		set(colour.red, colour.green, colour.blue, colour.alpha);
	}
	
	void setANSI(const unsigned char _value, const bool _boldModifier=false)
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
};

#endif