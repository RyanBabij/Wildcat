#pragma once
#ifndef WILDCAT_GUI_GUI_DROPLIST_HPP
#define WILDCAT_GUI_GUI_DROPLIST_HPP

/* Wildcat: GUI_DropList
	#include <GUI/GUI_DropList.hpp>
	
  DropList. The user clicks the droplist, and a list drops down showing all selectable options, 
  with a scrollbar, if all the options can't fit on the screen. The user then clicks on the option 
  he wishes to select. The list then disappears, and only the selected option is displayed. 
  The user can use the keyboard to narrow down the options on the list. The user can use the arrow 
  keys to move through the options without using a mouse. 
*/

#include <GUI/GUI_Interface.hpp>


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


				Wildcat::Region* region = new Wildcat::Region (panelX1,currentY-(font->nY+1),panelX2,currentY+1);
				vListOption.push(region);

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

#endif
