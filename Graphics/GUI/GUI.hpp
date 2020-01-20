#pragma once
#ifndef WILDCAT_GUI_GUI_HPP
#define WILDCAT_GUI_GUI_HPP

/* Wildcat: GUI
  #include <GUI/GUI.hpp>
  
	Parent file for all GUI libraries.
  This includes all GUI controls so you don't need to bother listing all of them.
  Also contains general notes pertaining to GUI system as a whole.
  
  Notes:
  
  Some of these controls could be combined into one. For example a textbox and link.
*/

#include <Graphics/GUI/GUI_Link.hpp>
#include <Graphics/GUI/GUI_DropList.hpp>
#include <Graphics/GUI/GUI_Notification.hpp>
#include <Graphics/GUI/GUI_NumberInput.hpp>
#include <Graphics/GUI/GUI_TextBox.hpp>
#include <Graphics/GUI/GUI_CycleButton.hpp>
#include <Graphics/GUI/GUI_Button.hpp>
#include <Graphics/GUI/GUI_TextEntry.hpp>

/* REF: GUI, <GUI/GUI.hpp>
	bool active: Whether the GUI should be rendering and taking input, or is just waiting in memory.
	int panelX1, panelX2, panelY1, panelY2: Absolute coordinates that the control takes up on the screen.
	Font* font: The font to use for text rendering.
	virtual void setPanel(const int _panelX1, const int _panelY1, const int _panelX2, const int _panelY2)
	virtual void movePanel ( const int moveX, const int moveY )
	virtual bool keyboardEvent ( Keyboard* )
*/

// #include <Graphics/Region/Region.hpp>
// #include <Graphics/Colour/Colour.hpp>

// #include <Render/Renderer.hpp>

// #include <GUI/GUI_Interface.hpp>

	// AVOID SOME FUNCTION NAME COLLISIONS.
//using namespace Wildcat;









// class GUI_ScrollBar: public GUI_Interface
// {
	// public:

	// int minScroll, maxScroll, currentMinScroll, currentMaxScroll;
	// Colour colourBackground;
	// Colour colourScrollBar;

	// /* NOTE 0221683311: GUI_ScrollBar now uses Texture instead of GLuint. */

	// //GLuint texScrollDown;
	// //GLuint texScrollUp;

	// Texture* texScrollDown;
	// Texture* texScrollUp;


	// GUI_ScrollBar(): GUI_Interface() /* Super */
	// {
		// minScroll=0;
		// maxScroll=0;
		// currentMinScroll=0;
		// currentMaxScroll=0;
	// }

	// void setColours(const Colour* _colourBackground, const Colour* _colourScrollBar)
	// {
		// colourBackground.set(_colourBackground);
		// colourScrollBar.set(_colourScrollBar);
	// }

	// void render()
	// {
		// Renderer::placeColour4(&colourBackground,panelX1,panelY1,panelX2,panelY2);


		// /* Draw scroll up button. */
		// Renderer::placeTexture4(panelX1,panelY1-10,panelX2,panelY1,texScrollUp);
		// /* Draw scroll down button. */
		// Renderer::placeTexture4(panelX1,panelY2,panelX2,panelY2+10,texScrollDown);

		// const int scrollButtonSize = 10;
		// /* Figure out where to put scroll button. */

		// //const int sizeY = panelY1-panelY2;
		// //std::cout<<"SizeY: "<<sizeY<<".\n";

		// const int scrollSize = maxScroll - minScroll;
		// //std::cout<<"scrollSize: "<<scrollSize<<".\n";

		// if(scrollSize==0) { return; }


		// const double minScrollOffset = currentMinScroll-minScroll;


		// const double percentMinScroll = minScrollOffset/scrollSize;

		// //std::cout<<"Percentminscroll: "<<percentMinScroll<<".\n";


		// const double maxScrollOffset = currentMaxScroll-minScroll;
		// const double percentMaxScroll = maxScrollOffset/scrollSize;
		// //std::cout<<"Percentmaxscroll: "<<percentMaxScroll<<".\n";


		// //const int panelSizeY = (panelY1-panelY2)+(scrollButtonSize*2);
		// const int panelSizeY = (panelY1-panelY2)-(scrollButtonSize*2);
		// //std::cout<<"PanelSizeY: "<<panelSizeY<<".\n";

		// const int pxScrollY1 = (percentMinScroll*panelSizeY) + panelY2 + scrollButtonSize;
		// //std::cout<<"pxScrollY1: "<<pxScrollY1<<".\n";

		// const int pxScrollY2 = (percentMaxScroll*panelSizeY) + panelY2 + scrollButtonSize;

		// /* Draw scrollbar */

		// Renderer::placeColour4(&colourScrollBar,panelX1,pxScrollY1,panelX2,pxScrollY2);
	// }

	// void scroll (const int scrollPixels)
	// {
		// currentMinScroll+=scrollPixels;
		// currentMaxScroll+=scrollPixels;

		// if(currentMinScroll<minScroll)
		// {
			// const int diff = minScroll-currentMinScroll;
			// currentMinScroll+=diff;
			// currentMaxScroll+=diff;
		// }
		// if(currentMaxScroll>maxScroll)
		// {
			// const int diff = currentMaxScroll-maxScroll;
			// currentMinScroll-=diff;
			// currentMaxScroll-=diff;
		// }
		// std::cout<<"Currentminscroll: "<<currentMinScroll<<"\n";
		// std::cout<<"Currentmaxscroll: "<<currentMaxScroll<<"\n";

	// }

	// /* NOTE: Now uses Texture instead of GLuint */


	// //void setTextures(GLuint _texScrollDown, GLuint _texScrollUp)
	// void setTextures(Texture* _texScrollDown, Texture* _texScrollUp)
	// {
		// texScrollDown=_texScrollDown;
		// texScrollUp=_texScrollUp;
	// }

// };

#endif
