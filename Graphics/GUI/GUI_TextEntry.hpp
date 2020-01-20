#pragma once
#ifndef WILDCAT_GUI_GUI_TEXTENTRY_HPP
#define WILDCAT_GUI_GUI_TEXTENTRY_HPP

/* Wildcat: GUI_TextEntry
	#include <GUI/GUI_TextEntry.hpp>
	
	A TextEntry is a box within which the player can enter text. The text entry can be just a single line, or can be multiple lines, allowing for a large amount of text to be entered.

  Features:
  * Character limit.
  * Alpha-numeric limit.
  
*/

#include <Graphics/GUI/GUI_Interface.hpp>

class GUI_TextEntry: public GUI_Interface
{
	public:
	std::string input;
	std::string fieldName;

	//int panelX1,panelY1,panelX2,panelY2;

	//Font* font;
	//GLuint backgroundTexture;

	Colour colourNormal;
	Colour colourSelected;

	bool active; /* Whether the control is visible and recieves input. */

	bool selected; /* Whether the control should be recieving data input. */

	bool acceptNewline;

	unsigned int characterLimit; /* Mamimum amount of characters. 0 is unlimited. */

	Wildcat::Region regionTextEntry;

	GUI_TextEntry() : GUI_Interface() /* super */
	{
		input="";
		fieldName="Blah:";
		active=true;
		selected=false;
		characterLimit=0;
		acceptNewline=false;

	}

	void movePanel ( const int moveX, const int moveY )
	{
		panelX1+=moveX; panelX2+=moveX; panelY1+=moveY; panelY2+=moveY;
		regionTextEntry.move(moveX,moveY);
	}

	void setPanel(const int _panelX1, const int _panelY1, const int _panelX2, const int _panelY2)
	{
		panelX1=_panelX1; panelY1=_panelY1; panelX2=_panelX2; panelY2=_panelY2;
		/* Automatically sort the panel coordinates here, so we don't need to input the coordinates in any particular order. */
		if (panelY1<panelY2) { DataTools::swap(&panelY1,&panelY2); }
		if (panelX1>panelX2) { DataTools::swap(&panelX1,&panelX2); }

		regionTextEntry.set(panelX1,panelY1,panelX2,panelY2);
	}

	void setColours(const Colour* cNormal, const Colour* cSelected)
	{
		colourNormal.set(cNormal);
		colourSelected.set(cSelected);
	}

	inline void addChar (const char c)
	{
		if ( characterLimit==0 || input.size() < characterLimit )
		{
			input+=c;
		}
	}
	void backspace()
	{
		if(input.size()>0)
		{ input.erase(input.size()-1); }
	}

	void render()
	{
		if(active==true)
		{
			//{
			//	Renderer::placeTexture4(panelX1,panelY1,panelX2,panelY2, backgroundTexture);

			//}

			if (selected==false)
			{
				Renderer::placeColour4(&colourNormal,panelX1,panelY1,panelX2,panelY2);
			}
			else
			{
				Renderer::placeColour4(&colourSelected,panelX1,panelY1,panelX2,panelY2);
			}
			font8x8.drawText(fieldName+" "+input, panelX1+2, panelY2, panelX2-2, panelY1, false, true);
		}

	}

	bool mouseEvent ( Mouse* mouse )
	{
		/* If the text box isn't disabled. */
		if ( active == true )
		{
			if ( mouse->isLeftClick )
			{
				/* If the player is hovering the mouse over the text box. */
				if ( regionTextEntry.liesOn(mouse->x,mouse->y) )
				{
					/* Activate the text field, so that the user can enter data using the keyboard. */
					selected=true;
					mouse->isLeftClick=false;
					return true;
				}
				/* If the player has clicked away from the text field, deactivate it. */
				else
				{
					selected=false;
					//mouse->isLeftClick=false;
					
					
				}
			}
		}
		return false;
	}
	
		/* If this control is selected, it should take all keyboard events (except critical commands). */
	bool stealKeyboard()
	{
		return (selected);
	}

	bool keyboardEvent(Keyboard* keyboard)
	{
		
		if ( active==true && selected==true && keyboard->keyWasPressed==true )
		{
			/* If the player pressed an alpha-numeric key, then add it to the input. */
			if ( keyboard->isAlphaNumeric(keyboard->lastKey))
			{ addChar(keyboard->lastKey); }
			//{ input+=keyboard->lastKey; }
			/* If the player pressed backspace, remove the last input. */
			else if (keyboard->lastKey==Keyboard::BACKSPACE)
			{ backspace(); }
			/* If the player pressed spacebar, add a space to the input. */
			else if (keyboard->lastKey==Keyboard::SPACEBAR)
			{ input+=" "; }
			// ASTERISK SUPPORT
			else if (keyboard->lastKey==Keyboard::ASTERISK)
			{ input+="*"; }
			else if (keyboard->lastKey==Keyboard::PERIOD)
			{ input+="."; }
			/* If the player pressed enter, deselect the control. */
			else if (keyboard->lastKey==Keyboard::ENTER)
			{
				if ( acceptNewline == true )
				{
					input+="\n";
				}
				else
				{
					selected=false;
				}
			}

			//{ input+="\n"; }
			//else if (keyboard->lastKey==Keyboard::ENTER)
			//{ selected=false; }

			/* Clear the keyboard. */
			keyboard->keyUp(keyboard->lastKey);
			keyboard->keyWasPressed=false;

			return true;
		}
		return false;
	}


};


#endif
