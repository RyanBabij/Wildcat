#pragma once
#ifndef WILDCAT_GRAPHICS_COLOUR_COLOUR_MANAGER_2_HPP
#define WILDCAT_GRAPHICS_COLOUR_COLOUR_MANAGER_2_HPP

/* Wildcat: Graphics_ColourManager.hpp
	#include <Graphics/Colour/ColourManager.hpp>
	
	Class which contains and manages colours. The most important functionality is to determine the
	closest named colour to an RGB value.
	After this it should also maintain a heirarchy for example RED is above DARK RED and LIGHT RED.
	However for now I think maintaining a big list should be fine
*/

template <class T>
class ColourManager2
{
	Vector <ColourRGB <T> * > vColour;
	Vector <std::string> vName;
	
	public:
	
	ColourManager2()
	{
	}
	
	// construct this colour and push it to the vector
	void makeColour(const T _red,const  T _green, const T _blue, const std::string _name)
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