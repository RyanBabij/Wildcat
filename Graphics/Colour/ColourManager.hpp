#pragma once
#ifndef WILDCAT_GRAPHICS_COLOUR_COLOUR_MANAGER_HPP
#define WILDCAT_GRAPHICS_COLOUR_COLOUR_MANAGER_HPP

/* Wildcat: Graphics_ColourManager.hpp
	#include <Graphics/Colour/ColourManager.hpp>
	
	Class which contains and manages colours. The most important functionality is to determine the
	closest named colour to an RGB value.
	After this it should also maintain a heirarchy for example RED is above DARK RED and LIGHT RED.
	However for now I think maintaining a big list should be fine
*/

template <class T>
class ColourManager
{
	Vector <ColourRGB <T> * > vColour;
	Vector <std::string> vName;
	
	public:
	
	ColourManager()
	{
	}
	
	// construct this colour and push it to the vector
	void makeColour(const T _red,const  T _green, const T _blue, const std::string _name)
	{
		vColour.push( new ColourRGB <T>(_red,_green,_blue) );
		vName.push(_name);
	}
	
	ColourRGB <T> * getClosestTo(ColourRGB <T>* colour)
	{
		return getClosestTo(colour->red, colour->green, colour->blue);
	}
	ColourRGB <T> * getClosestTo(ColourRGB <T>& colour)
	{
		return getClosestTo(colour.red, colour.green, colour.blue);
	}
	
	ColourRGB <T> * getClosestTo(T _red, T _green, T _blue)
	{	
		ColourRGB <T> colour(_red, _green, _blue);
		
		
		unsigned int closestDiff = UINT_MAX;
		ColourRGB <T> * closestColour = 0;
		for (int i=0;i<vColour.size(); ++i)
		{
			if ( closestColour == 0 || colour.distanceTo(vColour(i)) < closestDiff )
			{
				closestDiff = colour.distanceTo(vColour(i));
				closestColour = vColour(i);
			}
		}
		return closestColour;
	}
	
	
	std::string getNameOfColour(unsigned char _red, unsigned char _green, unsigned char _blue)
	{
		//return "Black";
		
		if ( vName.size() == 0 )
		{ return ""; }
	
		// get closest index and then print name
		ColourRGB <T> colour(_red, _green, _blue);
		
		unsigned int closestDiff = UINT_MAX;
		int closestIndex = 0;
		for (int i=0;i<vColour.size(); ++i)
		{
			if ( closestDiff == UINT_MAX || colour.distanceTo(vColour(i)) < closestDiff )
			{
				closestDiff = colour.distanceTo(vColour(i));
				closestIndex = i;
			}
		}
		return vName(closestIndex);
	}
	std::string getNameOfColour(ColourRGB <T>& colour)
	{
		return getNameOfColour(colour.red,colour.green,colour.blue);
	}
	
	std::string toString()
	{
		std::string strRet = "";
		for (int i=0;i<vColour.size();++i)
		{
			strRet+=vName(i)+": "+vColour(i)->toString()+"\n";
		}	
		return strRet;
	}

	void getRandom()
	{
		// return a random named colour
	}
	
	// return random colour in the given ranges. Might be useful for generating types of red for example.
	// or skin colours or flower colours as they tend to all fall within certain ranges.
	void getRandomInRange(const unsigned char minRed, const unsigned char maxRed, const unsigned char minGreen,
		const unsigned char maxGreen, const unsigned char minBlue, const unsigned char maxBlue)
	{
	}
	
};

#endif
