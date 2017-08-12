#pragma once

/* GUI.hpp
	Parent file for all GUI libraries.
	Allows fast and easy creation of GUIs.
	A GUI will not typically be a performance bottleneck, so I'll focus on readability over performance.
*/

/* REF: GUI, <GUI/GUI.hpp>
	bool active: Whether the GUI should be rendering and taking input, or is just waiting in memory.
	int panelX1, panelX2, panelY1, panelY2: Absolute coordinates that the control takes up on the screen.
	Font* font: The font to use for text rendering.
	virtual void setPanel(const int _panelX1, const int _panelY1, const int _panelX2, const int _panelY2)
	virtual void movePanel ( const int moveX, const int moveY )
	virtual bool keyboardEvent ( Keyboard* )
*/

#include <Graphics/Region/Region.hpp>
#include <Graphics/Colour/Colour.hpp>

#include <Render/Renderer.hpp>

#include <GUI/GUI_Interface.hpp>

	// AVOID SOME FUNCTION NAME COLLISIONS.
//using namespace Wildcat;


/* GUI_Manager is the container for GUI objects. */
class GUI_Manager: public DisplayInterface, public MouseInterface
{
	public:
	bool active; /* Used to switch the menu off and on. */
	/* Vector of all GUI controls. */
	Vector <GUI_Interface*> vGUI;
	GUI_Interface* selectedControl; /* selectedControl gets rendered last, to put it on top. It also recieves events first. */

	int panelX1, panelY1, panelX2, panelY2; /* Optional. Sets the coordinates that the GUI panel occupies. */


	GUI_Manager()
	{ selectedControl=0;
		active = true;
	}

	int panelNX()
	{
		return panelX2-panelX1;
	}

	void setFont(Wildcat::Font* _font)
	{
		for(int i=0;i<vGUI.size();++i)
		{
			vGUI(i)->setFont(_font);
		}
	}

	void clearMemory()
	{
		vGUI.clear();
	}

	void moveAllControls (const int moveX, const int moveY)
	{
		for(int i=0;i<vGUI.size();++i)
		{ vGUI(i)->movePanel(moveX,moveY);
		}
	}

	void setPanel( const int _x1, const int _y1, const int _x2, const int _y2)
	{
		panelX1=_x1;
		panelY1=_y1;
		panelX2=_x2;
		panelY2=_y2;
	}

	/* Add a control to the manager. It will then recieve events and can render. */
	void addControl(GUI_Interface* gui)
	{ vGUI.push(gui); }
	void add(GUI_Interface* gui)
	{ vGUI.push(gui); }

	/* Remove a control from the manager. It will no longer recieve events or render. */
	bool removeControl(GUI_Interface* gui)
	{ return vGUI.erase(gui); selectedControl=0; }

	/* Render all controls in the control vector. */
	void render()
	{
		if(active)
		{
			for(int i=0;i<vGUI.size();++i)
			{
				if (vGUI(i)!=0)
				{ vGUI(i)->render();
				}
				else
				{
					std::cout<<"Bad render.\n";
				}
			}

			if(selectedControl!=0)
			{ selectedControl->render();
			}
		}
	}

	bool mouseEvent(Mouse* mouse)
	{
		if(active)
		{

			/* Let the selectedcontrol get the first chance at input. */
			if(selectedControl!=0 && selectedControl->mouseEvent(mouse)==true)
			{ return true; }
			for(int i=0;i<vGUI.size();++i)
			{
				if(vGUI(i)->mouseEvent(mouse)==true)
				{
					/* Interesting idea. If a control uses device input, then it is selected. */
					selectedControl=vGUI(i);
					return true;
				}
			}
		}
		return false;
	}

	bool keyboardEvent(Keyboard* keyboard)
	{
		if(active)
		{
			/* Let the selectedcontrol get the first chance at input. */
			if(selectedControl!=0 && selectedControl->keyboardEvent(keyboard)==true)
			{ return true; }
			for(int i=0;i<vGUI.size();++i)
			{
				if(vGUI(i)->keyboardEvent(keyboard)==true)
				{
					/* Interesting idea. If a control uses device input, then it is selected. */
					selectedControl=vGUI(i);
					return true;
				}
			}
		}
		return false;
	}

};






/*
	Number input. Allows the user to select from a range of possible numbers. Allows the user to manually input a number using the keyboard, or to increase or decrease the number using buttons.
*/

class GUI_NumberInput: public GUI_Interface
{
	private:

		bool regionDecrement1hovered;
		bool regionDecrement10hovered;
		bool regionDecrement100hovered;
		bool regionIncrement1hovered;
		bool regionIncrement10hovered;
		bool regionIncrement100hovered;

		Colour cNormal;
		Colour cHovered;
		Colour cSelected;

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

	void setColours(const Colour* _cNormal, const Colour* _cSelected, const Colour* _cHovered)
	{
		cNormal.set(_cNormal);
		cSelected.set(_cSelected);
		cHovered.set(_cHovered);
	}

	void render()
	{
		if ( active==true)
		{
			if ( texIncrement == 0 || texDecrement == 0 )
			{
				std::cout<<"Error rendering GUI_NumberInput. No textures.\n";
				return;
			}

			if(selected==false)
			{ Renderer::placeColour4(&cNormal,panelX1,panelY1,panelX2,panelY2); }
			else
			{ Renderer::placeColour4(&cSelected,panelX1,panelY1,panelX2,panelY2); }

			/* Draw decrement buttons, on the left side of the control. */
			int squareSize = panelY1-panelY2;
			if (squareSize<0) { squareSize=-squareSize; }
			int posX=panelX1;

			if ( hasIncrement3 == true )
			{
				/* Decrement 100. */
				if ( regionDecrement100hovered == true )
				{ Renderer::placeColour4(&cHovered,posX,panelY1,posX+squareSize,panelY2); }
				else
				{ Renderer::placeColour4(&cNormal,posX,panelY1,posX+squareSize,panelY2); }
				Renderer::placeTexture4(posX,panelY1,posX+squareSize,panelY2,texDecrement);
				regionDecrement100.set(posX,panelY1,posX+squareSize,panelY2);
				posX+=squareSize;
			}

			if ( hasIncrement2 == true )
			{
				/* Decrement 10. */
				if ( regionDecrement10hovered == true )
				{ Renderer::placeColour4(&cHovered,posX,panelY1,posX+squareSize,panelY2); }
				else
				{ Renderer::placeColour4(&cNormal,posX,panelY1,posX+squareSize,panelY2); }
				Renderer::placeTexture4(posX,panelY1,posX+squareSize,panelY2,texDecrement);
				regionDecrement10.set(posX,panelY1,posX+squareSize,panelY2);
				posX+=squareSize;
			}

			if ( hasIncrement1 == true )
			{
				/* Decrement 1. */
				if ( regionDecrement1hovered == true )
				{ Renderer::placeColour4(&cHovered,posX,panelY1,posX+squareSize,panelY2); }
				else
				{ Renderer::placeColour4(&cNormal,posX,panelY1,posX+squareSize,panelY2); }
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
				{ Renderer::placeColour4(&cHovered,posX-squareSize,panelY1,posX,panelY2); }
				else
				{ Renderer::placeColour4(&cNormal,posX-squareSize,panelY1,posX,panelY2); }
				Renderer::placeTexture4(posX-squareSize,panelY1,posX,panelY2,texIncrement);
				regionIncrement100.set(posX-squareSize,panelY1,posX,panelY2);
				posX-=squareSize;
			}

			if ( hasIncrement2 == true )
			{
				/* Increment 10. */
				if ( regionIncrement10hovered == true )
				{ Renderer::placeColour4(&cHovered,posX-squareSize,panelY1,posX,panelY2); }
				else
				{ Renderer::placeColour4(&cNormal,posX-squareSize,panelY1,posX,panelY2); }
				Renderer::placeTexture4(posX-squareSize,panelY1,posX,panelY2,texIncrement);
				regionIncrement10.set(posX-squareSize,panelY1,posX,panelY2);
				posX-=squareSize;
			}

			if ( hasIncrement1 == true )
			{
				/* Increment 1. */
				if ( regionIncrement1hovered == true )
				{ Renderer::placeColour4(&cHovered,posX-squareSize,panelY1,posX,panelY2); }
				else
				{ Renderer::placeColour4(&cNormal,posX-squareSize,panelY1,posX,panelY2); }
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

/* Text Box. Displays text. */
class GUI_TextBox: public GUI_Interface
{
	public:
	std::string text;
	//Font* font;
	Colour boxColour;

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

	void setColours(const Colour* bColour)
	{
		boxColour.set(bColour);
	}

	void render()
	{
		if(active==true)
		{
			if ( drawBox==true)
			{ Renderer::placeColour4(&boxColour, panelX1, panelY1, panelX2, panelY2); }

			font8x8.drawText(text,panelX1,panelY2,panelX2,panelY1,centeredX,centeredY);
		}
	}

};

/* DropList. The user clicks the droplist, and a list drops down showing all selectable options, with a scrollbar, if all the options can't fit on the screen. The user then clicks on the option he wishes to select. The list then disappears, and only the selected option is displayed. The user can use the keyboard to narrow down the options on the list. The user can use the arrow keys to move through the options without using a mouse. */

class GUI_DropList: public GUI_Interface
{
	public:
	bool active;
	bool selected;
	Vector <std::string> vOption;
	int currentOption; /* index of vOption */
	//int x1,x2,y1,y2;
	//GLuint texNormal;



	/* NOTE: GUI_DropList now uses Texture object instead of GLuint. */
	//GLuint texDropDownButton;
	Texture* texDropDownButton;


	//GLuint texHoveredItem;
	//GLuint texDropDownBackground;

	Colour colourNormal;
	Colour colourSelected;
	Colour colourDropDown;
	Colour colourOptionHighlight;

	Wildcat::Region regionDropDownButton;
	Wildcat::Font* font;
	int squareSize;
	int hoveredItem;

	Vector <Wildcat::Region*> vListOption;

	GUI_DropList() : GUI_Interface() /* Super */
	{
		active=true;
		selected=false;
		currentOption=0;
		hoveredItem=-1;

		colourNormal.set(255,255,255);
		colourSelected.set(255,255,255);
		colourDropDown.set(255,255,255);
		colourOptionHighlight.set(255,255,255);
	}

	void clearMemory()
	{
		vOption.clear();
	}

	void movePanel ( const int moveX, const int moveY )
	{
		panelX1+=moveX; panelX2+=moveX; panelY1+=moveY; panelY2+=moveY;
		regionDropDownButton.move(moveX,moveY);
	}

	void setColours(const Colour* cNormal, const Colour* cSelected, const Colour* cDropDown, const Colour* cHighlight)
	{
		colourNormal.set(cNormal);
		colourSelected.set(cSelected);
		colourDropDown.set(cDropDown);
		colourOptionHighlight.set(cHighlight);
	}
	/* Set the colours by copying the colours from another drop down list. */
	void setColours( GUI_DropList* pDropList )
	{
		colourNormal.set(pDropList->colourNormal);
		colourSelected.set(pDropList->colourSelected);
		colourDropDown.set(pDropList->colourDropDown);
	}

	void setPanel(const int _x1, const int _y1, const int _x2, const int _y2)
	{
		panelX1=_x1; panelY1=_y1; panelX2=_x2; panelY2=_y2;
		//x1=_x1; y1=_y1; x2=_x2; y2=_y2;
		squareSize = panelY2-panelY1;
		regionDropDownButton.set(panelX2-squareSize,panelY1,panelX2,panelY2);
	}

	void render()
	{
		/* Draw text background texture. */
		//Renderer::placeTexture4(x1,y1,x2,y2,texNormal);
		if (selected==false)
		{
			Renderer::placeColour4(&colourNormal,panelX1,panelY1,panelX2,panelY2);
		}
		else
		{
			Renderer::placeColour4(&colourSelected,panelX1,panelY1,panelX2,panelY2);
		}




		if ( font!=0 && vOption.size()>0 && currentOption>=0 && currentOption<vOption.size())
		{
			font->drawText(vOption(currentOption),panelX1,panelY2);
		}
		/* Draw drop-down button. */
		Renderer::placeTexture4(panelX2-squareSize,panelY1,panelX2,panelY2,texDropDownButton);

		if (selected==true)
		{
			int dropListSizePixels = vOption.size()*(font->nY+2);

			/* Draw text background texture. */
			//Renderer::placeTexture4(x1,y1-dropListSizePixels,x2,y1,texDropDownBackground);
			Renderer::placeColour4(&colourDropDown,panelX1,panelY1-dropListSizePixels,panelX2,panelY1);

			/* Draw text for selection options. */
			int currentY = panelY1-1;
			vListOption.clear();

			for (int i=0;i<vOption.size();++i)
			{
				/* If this list option is being hovered by the mouse, highlight it. */
				if (hoveredItem==i)
				{
					Renderer::placeColour4(&colourOptionHighlight,panelX1,currentY-(font->nY+1),panelX2,currentY+1);
					//Renderer::placeTexture4(x1,currentY-(font->nY+1),x2,currentY+1,texHoveredItem);
				}

				font->drawText(vOption(i),panelX1,currentY);


				Wildcat::Region* r = new Wildcat::Region (panelX1,currentY-(font->nY+1),panelX2,currentY+1);
				vListOption.push(r);

				currentY-=(font->nY+2);
			}

		}

	}

	// void mouseEvent(Mouse* mouse)
	// {
		// /* determine if the mouse is hovering over one of the menu options. */
	// }

	void addOption ( const std::string _option )
	{ vOption.push(_option); }

	/* Add options separated by a delimiter. */
	void addOptions (const std::string strOptions, const char delimiter)
	{
		/* Loop through the string of options. */
		std::string strCurrentOption="";
		for (unsigned int i=0;i<strOptions.size();++i)
		{
			/* If the current character in the string of options isn't a delimiter, then add it to the current option string. */
			if(strOptions[i]!=delimiter)
			{
				strCurrentOption+=strOptions[i];
			}
			/* If the delimiter is found and the current option string is not empty, add it to the options. */
			else if (strCurrentOption.size()>0)
			{
				addOption(strCurrentOption);
				strCurrentOption="";
			}
		}
		/* Add the final option. */
		if(strCurrentOption.size()>0)
		{
			addOption(strCurrentOption);
			strCurrentOption="";
		}
	}

	/* Return true if something was done with the mouse. This stops other GUI controls from getting mouse events. */
	bool mouseEvent(Mouse* mouse)
	{
		if (mouse->isLeftClick)
		{
			/* If the player clicked the 'drop down' button, then the control becomes selected. */
			if (selected==false && regionDropDownButton.liesOn(mouse->x,mouse->y))
			{
				mouse->isLeftClick=false;
				selected=true;
				return true;
			}
			/* If the player has left-clicked on an item on the drop-down list, then it becomes the selected option. The drop-down list becomes unselected. */
			else if (selected==true && hoveredItem!=-1)
			{
				currentOption=hoveredItem;
				hoveredItem=-1;
				selected=false;
				mouse->isLeftClick=false;
				return true;
			}
			/* If the control is selected, and the player has clicked away from the control, we deselect it. */
			else if (selected==true)
			{
				hoveredItem=-1;
				selected=false;
				mouse->isLeftClick=false;
				/* End mouse events so that clicking away from this control won't accidentally activate another control. */
				return true;
			}
		}
		/* If the mouse is hovering over an item on the drop-down menu, highlight it. */
		if (mouse->isLeftClick==false && selected==true)
		{
			hoveredItem=-1;
			for (int i=0;i<vListOption.size();++i)
			{
				if (vListOption(i)->liesOn(mouse->x,mouse->y))
				{
					/* Draw a texture over the region. */
					hoveredItem=i;
					return true;
				}
			}
		}
		/* Prevent other controls from getting input, if this input is selected. */
		if(selected==true)
		{ return true; }

		return false;
	}
};

/* Used to cycle through pre-set options. Some pixels to the left and right of the defined coordinates are used for the cycle buttons. They are square, so their width matches the height of the defined area. The text is displayed centered between the two cycle buttons. The control should be sized so that all options will fit in the text area. */
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

	Colour cNormal;
	Colour cHovered;
	Colour cSelected;

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

	void setColours(const Colour* _cNormal, const Colour* _cSelected, const Colour* _cHovered)
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

			Renderer::placeColour4(&cNormal,panelX1,panelY1,panelX2,panelY2);

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

class GUI_Button: public GUI_Interface
{
	public:

	std::string text; /* The text that is displayed on the button, indicating it's function. (optional) */

	Colour cDisabled; /* The texture that is used for the button when the user is unable to click it. (optional) */
	Colour cNormal;
	Colour cHovered; /* The colour to be used if the player is hovering the mouse cursor over the button. (optional) */


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

	void movePanel ( const int _moveX, const int _moveY )
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

	void setColours(const Colour* _cNormal, const Colour* _cHovered, const Colour* _cDisabled)
	{
		cNormal.set(_cNormal);
		cHovered.set(_cHovered);
		cDisabled.set(_cDisabled);
	}

	/* OpenGL. Call whenever the button needs to be rendered. NOTE: Move to GUI parent class. */
	void render()
	{
		if(active==true && panelNX!=0 && panelNY!=0)
		{

			/* If the mouse is hovering over the button, and we have a hovered texture for the button. */
			if (hovered==true)
			{ Renderer::placeColour4(&cHovered,panelX1,panelY1,panelX2,panelY2);
			}
			else
			{ Renderer::placeColour4(&cNormal,panelX1,panelY1,panelX2,panelY2);
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
	bool mouseEvent ( Mouse* mouse )
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

/* A box within which the player can enter text. The text entry can be just a single line, or can be multiple lines, allowing for a large amount of text to be entered.

Features:
* Character limit.
* Alpha-numeric limit.

*/

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
			font8x8.drawText(fieldName+" "+input, panelX1+2, panelY2, panelX2-2, panelY1, false, false);
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

class GUI_ScrollBar: public GUI_Interface
{
	public:

	int minScroll, maxScroll, currentMinScroll, currentMaxScroll;
	Colour colourBackground;
	Colour colourScrollBar;

	/* NOTE 0221683311: GUI_ScrollBar now uses Texture instead of GLuint. */

	//GLuint texScrollDown;
	//GLuint texScrollUp;

	Texture* texScrollDown;
	Texture* texScrollUp;


	GUI_ScrollBar(): GUI_Interface() /* Super */
	{
		minScroll=0;
		maxScroll=0;
		currentMinScroll=0;
		currentMaxScroll=0;
	}

	void setColours(const Colour* _colourBackground, const Colour* _colourScrollBar)
	{
		colourBackground.set(_colourBackground);
		colourScrollBar.set(_colourScrollBar);
	}

	void render()
	{
		Renderer::placeColour4(&colourBackground,panelX1,panelY1,panelX2,panelY2);


		/* Draw scroll up button. */
		Renderer::placeTexture4(panelX1,panelY1-10,panelX2,panelY1,texScrollUp);
		/* Draw scroll down button. */
		Renderer::placeTexture4(panelX1,panelY2,panelX2,panelY2+10,texScrollDown);

		const int scrollButtonSize = 10;
		/* Figure out where to put scroll button. */

		//const int sizeY = panelY1-panelY2;
		//std::cout<<"SizeY: "<<sizeY<<".\n";

		const int scrollSize = maxScroll - minScroll;
		//std::cout<<"scrollSize: "<<scrollSize<<".\n";

		if(scrollSize==0) { return; }


		const double minScrollOffset = currentMinScroll-minScroll;


		const double percentMinScroll = minScrollOffset/scrollSize;

		//std::cout<<"Percentminscroll: "<<percentMinScroll<<".\n";


		const double maxScrollOffset = currentMaxScroll-minScroll;
		const double percentMaxScroll = maxScrollOffset/scrollSize;
		//std::cout<<"Percentmaxscroll: "<<percentMaxScroll<<".\n";


		//const int panelSizeY = (panelY1-panelY2)+(scrollButtonSize*2);
		const int panelSizeY = (panelY1-panelY2)-(scrollButtonSize*2);
		//std::cout<<"PanelSizeY: "<<panelSizeY<<".\n";

		const int pxScrollY1 = (percentMinScroll*panelSizeY) + panelY2 + scrollButtonSize;
		//std::cout<<"pxScrollY1: "<<pxScrollY1<<".\n";

		const int pxScrollY2 = (percentMaxScroll*panelSizeY) + panelY2 + scrollButtonSize;

		/* Draw scrollbar */

		Renderer::placeColour4(&colourScrollBar,panelX1,pxScrollY1,panelX2,pxScrollY2);
	}

	void scroll (const int scrollPixels)
	{
		currentMinScroll+=scrollPixels;
		currentMaxScroll+=scrollPixels;

		if(currentMinScroll<minScroll)
		{
			const int diff = minScroll-currentMinScroll;
			currentMinScroll+=diff;
			currentMaxScroll+=diff;
		}
		if(currentMaxScroll>maxScroll)
		{
			const int diff = currentMaxScroll-maxScroll;
			currentMinScroll-=diff;
			currentMaxScroll-=diff;
		}
		std::cout<<"Currentminscroll: "<<currentMinScroll<<"\n";
		std::cout<<"Currentmaxscroll: "<<currentMaxScroll<<"\n";

	}

	/* NOTE: Now uses Texture instead of GLuint */


	//void setTextures(GLuint _texScrollDown, GLuint _texScrollUp)
	void setTextures(Texture* _texScrollDown, Texture* _texScrollUp)
	{
		texScrollDown=_texScrollDown;
		texScrollUp=_texScrollUp;
	}

};

