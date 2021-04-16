#pragma once
#ifndef WILDCAT_GUI_GUI_TEXTBOX_HPP
#define WILDCAT_GUI_GUI_TEXTBOX_HPP

/* Wildcat: GUI_TextBox
	#include <GUI/GUI_TextBox.hpp>
	
	A textbox is an area which will display text. It can be small or large.
  There are options to center the text. It will also automatically wrap the text.
*/

#include <Graphics/GUI/GUI_Interface.hpp>

/* Text Box. Displays text. */
class GUI_TextBox: public GUI_Interface
{
	public:
	std::string text;
	//Font* font;
	ColourRGB <unsigned char> boxColour;

	bool centeredX, centeredY;

	bool drawBox;

	GUI_TextBox() : GUI_Interface() /* Super */
	{
		font=0;
		boxColour.set(255,255,255); /* White as default. */
		text="";
		centeredX=false;
		centeredY=false;

		drawBox=true;
	}

	void setColours(const ColourRGB <unsigned char> bColour)
	{
		boxColour.set(bColour);
	}

	void render()
	{
		if(active==true)
		{
			if ( drawBox==true)
			{ Renderer::placeColour4(boxColour, panelX1, panelY1, panelX2, panelY2); }

			font8x8.drawText(text,panelX1,panelY2,panelX2,panelY1,centeredX,centeredY);
		}
	}

};

#endif
