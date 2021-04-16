#pragma once
#ifndef WILDCAT_GUI_GUI_BUTTON_HPP
#define WILDCAT_GUI_GUI_BUTTON_HPP

/* Wildcat: GUI_Button
	#include <GUI/GUI_Button.hpp>
	
	A button is a control that can display text or a texture, and may be clicked.
*/

#include <Graphics/GUI/GUI_Interface.hpp>

class GUI_Button: public GUI_Interface
{
	public:

	std::string text; /* The text that is displayed on the button, indicating it's function. (optional) */

	ColourRGB <unsigned char> cDisabled; /* The texture that is used for the button when the user is unable to click it. (optional) */
	ColourRGB <unsigned char> cNormal;
	ColourRGB <unsigned char> cHovered; /* The colour to be used if the player is hovering the mouse cursor over the button. (optional) */


	bool clicked;
	bool hovered;

	bool cycleClick; // DEFAULTE FALSE. TRUE ALLOWS A BUTTON TO BE UNCLICKED BY CLICKING IT.

	Wildcat::Region regionButtonArea;

	Texture* texture;
	Texture* clickedTexture; // DISPLAY THIS TEXTURE WHILE THE BUTTON IS CLICKED.

	GUI_Button()
	{
		hovered=false;
		texture=0;
		clickedTexture=0;
		cycleClick=false;
	}
	
	virtual void init() override
	{
		GUI_Interface::init();
	}
	
	
	void init (const std::string _text, Texture* const _texture, const ColourRGB <unsigned char>& _cNormal,
	const ColourRGB <unsigned char>& _cHovered, const ColourRGB <unsigned char>& _cDisabled,
	const bool _active)
	{
		text=_text;
		texture=_texture;
		setColours(_cNormal,_cHovered,_cDisabled);
		active=_active;
	}

	void unClick() { unclick(); }
	void unclick()
	{
		clicked=false;
		hovered=false;
	}

	bool grabClick()
	{
		const bool ret = clicked;
		clicked=false;
		return ret;
	}

	// GUI_INTERFACE::SETPANEL()
	// GUI_INTERFACE::MOVEPANEL()

	void setPanel(const int _x1, const int _y1, const int _x2, const int _y2)
	{
		GUI_Interface::setPanel(_x1,_y1,_x2,_y2);
		regionButtonArea.set(panelX1,panelY1,panelX2,panelY2);
	}

	void movePanel ( const int _moveX, const int _moveY ) override
	{
		GUI_Interface::movePanel(_moveX,_moveY);
		regionButtonArea.move(_moveX,_moveY);
	}

		// CALLS SUPER
	GUI_Button(const int _x1, const int _y1, const int _x2, const int _y2, const std::string _text="") : GUI_Interface()
	{
		text=_text;
		//texDisabled=_texDisabled; texNormal=_texNormal; texHovered=_texHovered;
		//x1=_x1; y1=_y1; x2=_x2; y2=_y2;
		panelX1=_x1; panelY1=_y1; panelX2=_x2; panelY2=_y2;
		clicked=false;

		regionButtonArea.set(panelX1,panelY1,panelX2,panelY2);
	}

	void setColours(const ColourRGB <unsigned char>& _cNormal, const ColourRGB <unsigned char>& _cHovered,
		const ColourRGB <unsigned char> _cDisabled)
	{
		cNormal.set(_cNormal);
		cHovered.set(_cHovered);
		cDisabled.set(_cDisabled);
	}

	/* OpenGL. Call whenever the button needs to be rendered. NOTE: Move to GUI parent class. */
	void render() override
	{
		if(active==true && panelNX!=0 && panelNY!=0)
		{

			/* If the mouse is hovering over the button, and we have a hovered texture for the button. */
			if (hovered==true)
			{ Renderer::placeColour4(cHovered,panelX1,panelY1,panelX2,panelY2);
			}
			else
			{ Renderer::placeColour4(cNormal,panelX1,panelY1,panelX2,panelY2);
			}

			if ( clicked==true && clickedTexture!=0)
			{
				Renderer::placeTexture4(panelX1,panelY1,panelX2,panelY2,clickedTexture);
			}
			else if (texture!=0)
			{
				Renderer::placeTexture4(panelX1,panelY1,panelX2,panelY2,texture);
			}




			//if(texture!=0)
			//{
				//Renderer::placeTexture4(panelX1,panelY1,panelX2,panelY2,texture);
			//}

			if(font!=0)
			{ font->drawText(text,panelX1,panelY2,panelX2,panelY1,true,true); }

		}
	}

	/* Handle mouse input. */
	bool mouseEvent ( Mouse* mouse ) override
	{
		if(active==true)
		{

			if ( mouse->isLeftClick )
			{
				if (regionButtonArea.liesOn(mouse->x,mouse->y))
				{

					if ( cycleClick == true )
					{
						clicked = !clicked;
					}
					else
					{
						clicked=true;
					}
					mouse->isLeftClick=false;
					return true;
				}
			}
			else
			{
				/* If the player is hovering the mouse over the button, hovered is true, otherwise, hovered is false. */
				hovered = regionButtonArea.liesOn(mouse->x,mouse->y);
			}
		}
		return false;
	}

	/* Handle mouse input. */
	// void mouseClick ( Mouse* mouse )
	// {
		// if (mouse->isLeftClick)
		// {
			// std::cout<<"Clicked button.\n";
		// }

	// }
	// void mouseClick ( const int mouseX, const int mouseY, const int clickType )
	// {
		// if ( clickType==LEFT_CLICK )
		// {
			// if (regionUnitNameButton.liesOn(mouseX,mouseY) )
			// { textEntryUnitName.active=true; }
			// else if (regionMakeNewComponent.liesOn(mouseX,mouseY))
			// {
				// //std::cout<<"Make new component.\n";
				// componentConstructorMenu.active=true;
			// }
			// else { textEntryUnitName.active=false; }
		// }
	// }

};

#endif
