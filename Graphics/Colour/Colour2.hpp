#pragma once
#ifndef WILDCAT_GRAPHICS_COLOUR_COLOUR_2_HPP
#define WILDCAT_GRAPHICS_COLOUR_COLOUR_2_HPP

/* Wildcat: Graphics_Colour2.hpp
	#include <Graphics/Colour/Colour2.hpp>
	
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
	
	ColourRGB(T _red=255, T _green=255, T _blue=255)
	{
		red = _red;
		green = _green;
		blue = _blue;
	}
	
	void set(T _red=255, T _green=255, T _blue=255)
	{
		red = _red;
		green = _green;
		blue = _blue;
	}
	
	// set to a completely random colour
	void setRandom(RandomInterface& rng)
	{
		set(rng.rand8(), rng.rand8(), rng.rand8());
	}
	
	// set a random colour within given ranges
	void setRandomRange(unsigned char minRed, unsigned char maxRed, unsigned char minGreen, unsigned char maxGreen,
		unsigned char minBlue, unsigned char maxBlue, RandomInterface &rng)
	{
		set(rng.range32(minRed,maxRed), rng.range32(minGreen,maxGreen), rng.range32(minBlue,maxBlue));
	}
	
	unsigned int distanceTo(ColourRGB colour)
	{
		return 0;
	}
	
	std::string toString()
	{
		return "Test";
	}
};

// Add alpha
// Note alpha values here.
template <class T>
class ColourRGBA: public ColourRGB <T>
{
	public:
	T alpha;
	
	ColourRGBA(T _red=255, T _green=255, T _blue=255, T _alpha=255): ColourRGB <T>(0,0,0)
	{
		alpha = _alpha;
	}
};


template <class T>
class ColourManager
{
	Vector <ColourRGB <T> * > vColour;
	Vector <std::string> vName;
	
	public:
	
	// construct this colour and push it to the vector
	void makeColour(T _red, T _green, T _blue, std::string _name)
	{
		vColour.push( new ColourRGB <T>(_red,_green,_blue) );
		//vColour.push( new Colour(1,1,1) );
		vName.push(_name);
	}
	
	std::string getNameOfColour(ColourRGB <T> & colour)
	{
		return "Black";
	}
	
	std::string toString()
	{
		return "TEST";
	}
	
	ColourRGB <T> * getClosestTo(ColourRGB <T>& colour)
	{
		return getClosestTo(colour.red, colour.green, colour.blue);
	}
	ColourRGB <T> * getClosestTo(T _red, T _green, T _blue)
	{
		return 0;
		
		// ColourRGB <T> colour(_red, _green, _blue);
		
		
		// int closestDiff = -1;
		// Colour * closestColour = 0;
		// for (int i=0;i<vColour.size(); ++i)
		// {
			// if ( closestColour == 0 || colour.distanceTo(vColour(i)) < closestDiff )
			// {
				// closestDiff = colour.distanceTo(vColour(i));
				// closestColour = vColour(i);
			// }
		// }
		// return closestColour;
	}
};

#endif