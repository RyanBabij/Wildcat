#pragma once
#ifndef WILDCAT_GUI_GUI_LINK_HPP
#define WILDCAT_GUI_GUI_LINK_HPP

/* Wildcat: GUI_Link
	#include <GUI/GUI_Link.hpp>
	
	A link is clickable text which can send you somewhere when you click it.
*/

#include <GUI/GUI_Interface.hpp>

#include <Graphics/Region/Region.hpp>

/* Link. Clickable text. */
class GUI_Link: public GUI_Interface
{
	public:
	std::string text;
	//Font* font;
	Colour boxColour;

	bool centeredX, centeredY;
	
	bool clicked;
	Wildcat::Region regionButtonArea;

	GUI_Link() : GUI_Interface() /* Super */
	{
		font=0;
		boxColour.set(255,255,255); /* White as default. */
		text="";
		centeredX=false;
		centeredY=false;
		
		clicked=false;

		//drawBox=true;
	}
	
	virtual ~GUI_Link()
	{
	}
	
	void setPanel(const int _x1, const int _y1, const int _x2, const int _y2)
	{
		GUI_Interface::setPanel(_x1,_y1,_x2,_y2);
		regionButtonArea.set(panelX1,panelY1,panelX2,panelY2);
	}

	void movePanel ( const int _moveX, const int _moveY )
	{
		GUI_Interface::movePanel(_moveX,_moveY);
		regionButtonArea.move(_moveX,_moveY);
	}

	void setColours(const Colour* bColour)
	{
		//boxColour.set(bColour);
	}

	void render()
	{
		if(active==true)
		{
			//if ( drawBox==true)
			//{ Renderer::placeColour4(&boxColour, panelX1, panelY1, panelX2, panelY2); }

			font8x8.drawText(text,panelX1,panelY2,panelX2,panelY1,centeredX,centeredY,20,20,200);
		}
	}
	
	/* Handle mouse input. */
	bool mouseEvent ( Mouse* mouse )
	{
		if(active==true)
		{
			if ( mouse->isLeftClick )
			{
				if (regionButtonArea.liesOn(mouse->x,mouse->y))
				{
					clicked = !clicked;
				}
			}
		}
		return false;
	}
	
	void unclick()
	{
		clicked=false;
	}
		inline void unClick() { unclick(); }

};

#endif
