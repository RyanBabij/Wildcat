#pragma once
#ifndef WILDCAT_GUI_GUI_NUMBERINPUT_HPP
#define WILDCAT_GUI_GUI_NUMBERINPUT_HPP

/* Wildcat: GUI_NumberInput
	#include <GUI/GUI_NumberInput.hpp>
	
	Number input. Allows the user to select from a range of possible numbers.
  Allows the user to manually input a number using the keyboard,
  or to increase or decrease the number using buttons.
*/

#include <Graphics/GUI/GUI_Interface.hpp>

#include <Graphics/Region/Region.hpp>
#include <Graphics/Colour/Colour.hpp>

class GUI_NumberInput: public GUI_Interface
{
	private:

		bool regionDecrement1hovered;
		bool regionDecrement10hovered;
		bool regionDecrement100hovered;
		bool regionIncrement1hovered;
		bool regionIncrement10hovered;
		bool regionIncrement100hovered;

		ColourRGB <unsigned char> cNormal;
		ColourRGB <unsigned char> cHovered;
		ColourRGB <unsigned char> cSelected;

		Wildcat::Region regionDecrement1;
		Wildcat::Region regionDecrement10;
		Wildcat::Region regionDecrement100;

		Wildcat::Region regionIncrement1;
		Wildcat::Region regionIncrement10;
		Wildcat::Region regionIncrement100;

		Wildcat::Region regionNumberEntry;
		Wildcat::Region regionControl;

		int increment1, increment2, increment3;
		bool hasIncrement1, hasIncrement2, hasIncrement3;

	public:

	int currentValue;
	int defaultValue;
	int maxValue;
	int minValue;

	bool selected;
	bool active;

	Texture* texIncrement;
	Texture* texDecrement;

		// MAXIMUM VALUE POOL: USEFUL FOR CREATING A STATS MENU. GIVE IT A POINTER TO A NUMBER, AND IT WILL SUBTRACT AND ADD TO IT, AND NOT INCREMENT IF THE VALUE IS 0.
	int* maximumValuePool;

	GUI_NumberInput() : GUI_Interface() /* super */
	{
		maxValue=0; minValue=0; defaultValue=0; currentValue=0;
		selected=false; active=true;
		texIncrement=0; texDecrement=0;
		increment1=1;
		increment2=10;
		increment3=100;

		maximumValuePool=0;

		hasIncrement1=true;
		hasIncrement2=true;
		hasIncrement3=true;
      
		regionDecrement1hovered=false;
		regionDecrement10hovered=false;
		regionDecrement100hovered=false;
		regionIncrement1hovered=false;
		regionIncrement10hovered=false;
		regionIncrement100hovered=false;
	}
	void setNumbers ( const int _minValue, const int _maxValue, const int _defaultValue)
	{
		minValue=_minValue;
		maxValue=_maxValue;
		defaultValue=_defaultValue;
		currentValue=defaultValue;
	}

	/* UNTESTED. */
	/* Set to 0 to disable the button. */
	void setIncrements(const int _increment1=1, const int _increment2=10, const int _increment3=100)
	{
		increment1=_increment1;
		increment2=_increment2;
		increment3=_increment3;

		if ( increment1==0 ) { hasIncrement1=false; }
		if ( increment2==0 ) { hasIncrement2=false; }
		if ( increment3==0 ) { hasIncrement3=false; }
	}

	void movePanel ( const int moveX, const int moveY )
	{
		panelX1+=moveX; panelX2+=moveX; panelY1+=moveY; panelY2+=moveY;
		regionDecrement1.move(moveX,moveY);
		regionDecrement10.move(moveX,moveY);
		regionDecrement100.move(moveX,moveY);
		regionIncrement1.move(moveX,moveY);
		regionIncrement10.move(moveX,moveY);
		regionIncrement100.move(moveX,moveY);
		regionNumberEntry.move(moveX,moveY);
		regionControl.move(moveX,moveY);
	}

	// void setPanel(const int _panelX1, const int _panelY1, const int _panelX2, const int _panelY2)
	// {
		// panelX1=_panelX1;
		// panelY1=_panelY1;
		// panelX2=_panelX2;
		// panelY2=_panelY2;
		// //regionTextEntry.set(panelX1,panelY1,panelX2,panelY2);
	// }

	void setColours(const ColourRGB <unsigned char>& _cNormal, const ColourRGB <unsigned char>& _cSelected,
		const ColourRGB <unsigned char>& _cHovered)
	{
		cNormal.set(_cNormal);
		cSelected.set(_cSelected);
		cHovered.set(_cHovered);
	}

	void render()
	{
		if ( active==true )
		{
			if ( texIncrement == 0 || texDecrement == 0 )
			{
				std::cout<<"Error rendering GUI_NumberInput. No textures.\n";
				return;
			}

			if(selected==false)
			{ Renderer::placeColour4(cNormal,panelX1,panelY1,panelX2,panelY2); }
			else
			{ Renderer::placeColour4(cSelected,panelX1,panelY1,panelX2,panelY2); }

			/* Draw decrement buttons, on the left side of the control. */
			int squareSize = panelY1-panelY2;
			if (squareSize<0) { squareSize=-squareSize; }
			int posX=panelX1;

			if ( hasIncrement3 == true )
			{
				/* Decrement 100. */
				if ( regionDecrement100hovered == true )
				{ Renderer::placeColour4(cHovered,posX,panelY1,posX+squareSize,panelY2); }
				else
				{ Renderer::placeColour4(cNormal,posX,panelY1,posX+squareSize,panelY2); }
				Renderer::placeTexture4(posX,panelY1,posX+squareSize,panelY2,texDecrement);
				regionDecrement100.set(posX,panelY1,posX+squareSize,panelY2);
				posX+=squareSize;
			}

			if ( hasIncrement2 == true )
			{
				/* Decrement 10. */
				if ( regionDecrement10hovered == true )
				{ Renderer::placeColour4(cHovered,posX,panelY1,posX+squareSize,panelY2); }
				else
				{ Renderer::placeColour4(cNormal,posX,panelY1,posX+squareSize,panelY2); }
				Renderer::placeTexture4(posX,panelY1,posX+squareSize,panelY2,texDecrement);
				regionDecrement10.set(posX,panelY1,posX+squareSize,panelY2);
				posX+=squareSize;
			}

			if ( hasIncrement1 == true )
			{
				/* Decrement 1. */
				if ( regionDecrement1hovered == true )
				{ Renderer::placeColour4(cHovered,posX,panelY1,posX+squareSize,panelY2); }
				else
				{ Renderer::placeColour4(cNormal,posX,panelY1,posX+squareSize,panelY2); }
				Renderer::placeTexture4(posX,panelY1,posX+squareSize,panelY2,texDecrement);
				regionDecrement1.set(posX,panelY1,posX+squareSize,panelY2);
				posX+=squareSize;
			}

			/* Draw increment buttons, on the right side of the control. */
			posX=panelX2;

			if ( hasIncrement3 == true )
			{
				/* Increment 100. */
				if ( regionIncrement100hovered == true )
				{ Renderer::placeColour4(cHovered,posX-squareSize,panelY1,posX,panelY2); }
				else
				{ Renderer::placeColour4(cNormal,posX-squareSize,panelY1,posX,panelY2); }
				Renderer::placeTexture4(posX-squareSize,panelY1,posX,panelY2,texIncrement);
				regionIncrement100.set(posX-squareSize,panelY1,posX,panelY2);
				posX-=squareSize;
			}

			if ( hasIncrement2 == true )
			{
				/* Increment 10. */
				if ( regionIncrement10hovered == true )
				{ Renderer::placeColour4(cHovered,posX-squareSize,panelY1,posX,panelY2); }
				else
				{ Renderer::placeColour4(cNormal,posX-squareSize,panelY1,posX,panelY2); }
				Renderer::placeTexture4(posX-squareSize,panelY1,posX,panelY2,texIncrement);
				regionIncrement10.set(posX-squareSize,panelY1,posX,panelY2);
				posX-=squareSize;
			}

			if ( hasIncrement1 == true )
			{
				/* Increment 1. */
				if ( regionIncrement1hovered == true )
				{ Renderer::placeColour4(cHovered,posX-squareSize,panelY1,posX,panelY2); }
				else
				{ Renderer::placeColour4(cNormal,posX-squareSize,panelY1,posX,panelY2); }
				Renderer::placeTexture4(posX-squareSize,panelY1,posX,panelY2,texIncrement);
				regionIncrement1.set(posX-squareSize,panelY1,posX,panelY2);
			}

			// DRAW THE NUMBER
			if (font!=0)
			{ font->drawText(DataTools::toString(currentValue),panelX1,panelY1,panelX2,panelY2,true,true); }
		}
	}

	bool mouseEvent ( Mouse* mouse)
	{
		if (active==false)
		{ return false; }

		if ( mouse->isLeftClick == false )
		{
			/* Check to see if the mouse is hovering over any of the buttons. */


			/* Decrement 1. */
			if ( hasIncrement1 == true && regionDecrement1.liesOn(mouse->x,mouse->y))
			{
				regionDecrement1hovered = true;
				regionDecrement10hovered = false;
				regionDecrement100hovered = false;
				regionIncrement1hovered = false;
				regionIncrement10hovered = false;
				regionIncrement100hovered = false;
				return true;
			}
			/* Decrement 10. */
			else if( hasIncrement2 == true && regionDecrement10.liesOn(mouse->x,mouse->y))
			{
				regionDecrement1hovered = false;
				regionDecrement10hovered = true;
				regionDecrement100hovered = false;
				regionIncrement1hovered = false;
				regionIncrement10hovered = false;
				regionIncrement100hovered = false;
				return true;
			}
			/* Decrement 100. */
			else if( hasIncrement3 == true && regionDecrement100.liesOn(mouse->x,mouse->y))
			{
				regionDecrement1hovered = false;
				regionDecrement10hovered = false;
				regionDecrement100hovered = true;
				regionIncrement1hovered = false;
				regionIncrement10hovered = false;
				regionIncrement100hovered = false;
				return true;
			}
			/* Increment 1. */
			else if( hasIncrement1 == true && regionIncrement1.liesOn(mouse->x,mouse->y))
			{
				regionDecrement1hovered = false;
				regionDecrement10hovered = false;
				regionDecrement100hovered = false;
				regionIncrement1hovered = true;
				regionIncrement10hovered = false;
				regionIncrement100hovered = false;
				return true;
			}
			/* Increment 10. */
			else if( hasIncrement2 == true && regionIncrement10.liesOn(mouse->x,mouse->y))
			{
				regionDecrement1hovered = false;
				regionDecrement10hovered = false;
				regionDecrement100hovered = false;
				regionIncrement1hovered = false;
				regionIncrement10hovered = true;
				regionIncrement100hovered = false;
				return true;
			}
			/* Increment 100. */
			else if( hasIncrement3 == true && regionIncrement100.liesOn(mouse->x,mouse->y))
			{
				regionDecrement1hovered = false;
				regionDecrement10hovered = false;
				regionDecrement100hovered = false;
				regionIncrement1hovered = false;
				regionIncrement10hovered = false;
				regionIncrement100hovered = true;
				return true;
			}
			else
			{
				regionDecrement1hovered = false;
				regionDecrement10hovered = false;
				regionDecrement100hovered = false;
				regionIncrement1hovered = false;
				regionIncrement10hovered = false;
				regionIncrement100hovered = false;
			}
			return false;

		}

		if ( mouse->isLeftClick )
		{
			/* Decrement 1. */
			if ( hasIncrement1 == true && regionDecrement1.liesOn(mouse->x,mouse->y))
			{
				regionDecrement1hovered = true;
				mouse->isLeftClick=false;

				int actualAmountToSubtract = increment1;

				if ( currentValue - actualAmountToSubtract < minValue )
				{
					actualAmountToSubtract = currentValue - minValue;
				}

				if (maximumValuePool != 0 )
				{
					*maximumValuePool+=actualAmountToSubtract;
				}
				
				currentValue-=actualAmountToSubtract;

				return true;
			}
			/* Decrement 10. */
			else if( hasIncrement2 == true && regionDecrement10.liesOn(mouse->x,mouse->y))
			{
				regionDecrement10hovered = true;
				mouse->isLeftClick=false;

				int actualAmountToSubtract = increment2;

				if ( currentValue - actualAmountToSubtract < minValue )
				{
					actualAmountToSubtract = currentValue - minValue;
				}

				if (maximumValuePool != 0 )
				{
					*maximumValuePool+=actualAmountToSubtract;
				}
				
				currentValue-=actualAmountToSubtract;

				return true;
			}
			/* Decrement 100. */
			else if( hasIncrement3 == true && regionDecrement100.liesOn(mouse->x,mouse->y))
			{
				regionDecrement100hovered = true;
				mouse->isLeftClick=false;

				int actualAmountToSubtract = increment3;

				if ( currentValue - actualAmountToSubtract < minValue )
				{
					actualAmountToSubtract = currentValue - minValue;
				}

				if (maximumValuePool != 0 )
				{
					*maximumValuePool+=actualAmountToSubtract;
				}
				
				currentValue-=actualAmountToSubtract;

				return true;
			}
			/* Increment 1. */
			else if( hasIncrement1 == true && regionIncrement1.liesOn(mouse->x,mouse->y))
			{
				regionIncrement1hovered = true;
				mouse->isLeftClick=false;

				int actualAmountToAdd = increment1;

				if ( currentValue + actualAmountToAdd > maxValue )
				{
					actualAmountToAdd = maxValue - currentValue;
				}

				if (maximumValuePool != 0 )
				{
					if ( *maximumValuePool - actualAmountToAdd < 0 )
					{
						actualAmountToAdd = *maximumValuePool;
					}

					*maximumValuePool-=actualAmountToAdd;
				}
				
				currentValue+=actualAmountToAdd;

				return true;
			}
			/* Increment 10. */
			else if( hasIncrement2 == true && regionIncrement10.liesOn(mouse->x,mouse->y))
			{
				regionIncrement10hovered = true;
				mouse->isLeftClick=false;

				int actualAmountToAdd = increment2;

				if ( currentValue + actualAmountToAdd > maxValue )
				{
					actualAmountToAdd = maxValue - currentValue;
				}

				if (maximumValuePool != 0 )
				{
					if ( *maximumValuePool - actualAmountToAdd < 0 )
					{
						actualAmountToAdd = *maximumValuePool;
					}

					*maximumValuePool-=actualAmountToAdd;
				}
				
				currentValue+=actualAmountToAdd;

				return true;
			}
			/* Increment 100. */
			else if( hasIncrement3 == true && regionIncrement100.liesOn(mouse->x,mouse->y))
			{
				regionIncrement100hovered = true;
				mouse->isLeftClick=false;

				int actualAmountToAdd = increment3;

				if ( currentValue + actualAmountToAdd > maxValue )
				{
					actualAmountToAdd = maxValue - currentValue;
				}

				if (maximumValuePool != 0 )
				{
					if ( *maximumValuePool - actualAmountToAdd < 0 )
					{
						actualAmountToAdd = *maximumValuePool;
					}

					*maximumValuePool-=actualAmountToAdd;
				}
				
				currentValue+=actualAmountToAdd;

				return true;
			}
		}

		return false;
	}


};

#endif
