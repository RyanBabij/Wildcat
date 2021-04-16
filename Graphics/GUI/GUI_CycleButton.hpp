#pragma once
#ifndef WILDCAT_GUI_GUI_CYCLEBUTTON_HPP
#define WILDCAT_GUI_GUI_CYCLEBUTTON_HPP

/* Wildcat: GUI_CycleButton
	#include <GUI/GUI_CycleButton.hpp>
	
	A CycleButton is used to cycle through pre-set options. Some pixels to the left and right of the defined coordinates are used for the cycle buttons. They are square, so their width matches the height of the defined area. The text is displayed centered between the two cycle buttons. The control should be sized so that all options will fit in the text area.
*/

#include <Graphics/GUI/GUI_Interface.hpp>
#include <Graphics/Region/Region.hpp>

class GUI_CycleButton: public GUI_Interface
{
	public:

	Vector <std::string> vOptions;
	int currentOption;
	//Font* font;

	Texture* texNormal;
	Texture* texCycleButton;

	//int x1,y1,x2,y2;
	int squareSize;

	Wildcat::Region regionCyclePrevious;
	Wildcat::Region regionCycleNext;

	ColourRGB <unsigned char> cNormal;
	ColourRGB <unsigned char> cHovered;
	ColourRGB <unsigned char> cSelected;

	bool wrap;


	GUI_CycleButton(const int _x1, const int _y1, const int _x2, const int _y2) : GUI_Interface() /* super */
	{
		panelX1=_x1; panelY1=_y1; panelX2=_x2; panelY2=_y2;
		//x1=_x1; y1=_y1; x2=_x2; y2=_y2;
		currentOption=0;
		squareSize = panelY2-panelY1; /* The size of the cycle buttons. */
		wrap=true;
	}
	GUI_CycleButton() : GUI_Interface() /* super() */
	{
	}

	void movePanel ( const int moveX, const int moveY )
	{
		panelX1+=moveX; panelX2+=moveX; panelY1+=moveY; panelY2+=moveY;
		regionCyclePrevious.move(moveX,moveY);
		regionCycleNext.move(moveX,moveY);
	}

	void setColours(const ColourRGB <unsigned char>& _cNormal, const ColourRGB <unsigned char>& _cSelected,
		const ColourRGB <unsigned char>& _cHovered)
	{
		cNormal.set(_cNormal);
		cSelected.set(_cSelected);
		cHovered.set(_cHovered);
	}

	// void setPanel(const int _x1, const int _y1, const int _x2, const int _y2)
	// {
		// panelX1=_x1; panelY1=_y1; panelX2=_x2; panelY2=_y2;
		// //x1=_x1; y1=_y1; x2=_x2; y2=_y2;
		// squareSize = panelY2-panelY1;
	// }

	void eventResize()
	{
		squareSize = panelY2-panelY1;
		//std::cout<<"Squaresize: "<<squareSize<<".\n";
	}

	// void init (const int _x1, const int _y1, const int _x2, const int _y2)
	// {
		// panelX1=_x1; panelY1=_y1; panelX2=_x2; panelY2=_y2;
		// //x1=_x1; y1=_y1; x2=_x2; y2=_y2;
		// currentOption=0;
		// squareSize = panelY2-panelY1; /* The size of the cycle buttons. */
	// }

	std::string getCurrentOption()
	{
		return vOptions(currentOption);
	}

	void setTextures(Texture* _texNormal)
	{
		texNormal=_texNormal;
	}

	void clear()
	{
		vOptions.clear();
	}

	void render()
	{

		if(active==true)
		{

		//	std::cout<<"Trying to render.\n";

			Renderer::placeColour4(cNormal,panelX1,panelY1,panelX2,panelY2);

			/* Draw text background texture. */
			//Renderer::placeTexture4(panelX1,panelY1,panelX2,panelY2,texNormal);
			/* Draw cycle button background texture. */
			//Renderer::placeColour4(255,0,0,panelX1,panelY1,panelX1+squareSize,panelY2);
			//Renderer::placeColour4(255,0,0,0,0,200,200);
			Renderer::placeTexture4(panelX1,panelY1,panelX1+squareSize,panelY2,texCycleButton);
			regionCyclePrevious.set(panelX1,panelY1,panelX1+squareSize,panelY2);
			/* The texture is mirrored here. */
			Renderer::placeTexture4(panelX2,panelY1,panelX2-squareSize,panelY2,texCycleButton);
			regionCycleNext.set(panelX2-squareSize,panelY1,panelX2,panelY2);

			if ( font!=0 && vOptions.size()>0 && currentOption>=0 && currentOption<vOptions.size())
			{
				//font->drawText("<",panelX1,panelY2);
				//font->drawText(vOptions(currentOption),panelX1+squareSize+2,panelY2,true,true);
				font->drawText(vOptions(currentOption),panelX1+squareSize+2,panelY2,panelX2-squareSize-2,panelY1,true,true);
				//font->drawText(">",panelX2-squareSize,panelY2);
			}

		}
	}

	void addOption ( const std::string _option )
	{
		vOptions.push(_option);
	}


	bool mouseEvent (Mouse* mouse)
	{
		if(active==true)
		{
			/* if the user had left-clicked */
			if (mouse->isLeftClick)
			{
				/* If the user left-clicked the cycle next button. */
				if ( regionCycleNext.liesOn(mouse->x,mouse->y) )
				{
					++currentOption;
					if ( currentOption >= vOptions.size() )
					{
						if (wrap==true)
						{
							currentOption=0;
						}
						else
						{
							currentOption=vOptions.size()-1;
						}

					}
					mouse->isLeftClick=false;
				}
				/* If the user left-clicked the cycle previous button. */
				else if ( regionCyclePrevious.liesOn(mouse->x,mouse->y) )
				{
					--currentOption;
					if (currentOption<0)
					{
						if ( wrap==true )
						{
							currentOption = vOptions.size()-1;
						}
						else
						{
							currentOption = 0;
						}
					}
					mouse->isLeftClick=false;
				}
			}
		}
		return false;
	}

};

#endif
