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

//#include <Math/Random/RandomInterface.hpp>


class ColourManager
{
	public:
	
	Vector <Colour*> vColour;
	Vector <std::string> vName;
	
	ColourManager()
	{
	}
	
	void makeColour(unsigned char _red, unsigned char _green, unsigned char _blue, std::string _name)
	{
		vColour.push( new Colour(_red,_green,_blue) );
		//vColour.push( new Colour(1,1,1) );
		vName.push(_name);
	}
	
	// void add(Colour * colour)
	// {
		// vColour.push(colour);
	// }
	
	Colour* getClosestTo(Colour* colour)
	{
		return getClosestTo(colour->red, colour->green, colour->blue);
	}
	Colour* getClosestTo(unsigned char _red, unsigned char _green, unsigned char _blue)
	{
		Colour colour(_red, _green, _blue);
		
		
		int closestDiff = -1;
		Colour * closestColour = 0;
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
		if ( vName.size() == 0 )
		{ return ""; }
	
		// get closest index and then print name
		Colour colour(_red, _green, _blue);
		
		int closestDiff = -1;
		int closestIndex = 0;
		for (int i=0;i<vColour.size(); ++i)
		{
			if ( closestDiff == -1 || colour.distanceTo(vColour(i)) < closestDiff )
			{
				closestDiff = colour.distanceTo(vColour(i));
				closestIndex = i;
			}
		}
		return vName(closestIndex);
	}
	std::string getNameOfColour(Colour& colour)
	{
		return getNameOfColour(colour.red,colour.green,colour.blue);
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
		
	//void get
	
	std::string toString()
	{
		std::string strRet = "";
		for (int i=0;i<vColour.size();++i)
		{
			strRet+=vName(i)+": "+vColour(i)->toString()+"\n";
		}
		
		return strRet;
	}
};

#endif
