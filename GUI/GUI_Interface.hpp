#pragma once
#ifndef WILDCAT_GUI_GUI_INTERFACE_HPP
#define WILDCAT_GUI_GUI_INTERFACE_HPP

/* Wildcat: GUI_Interface
	#include <GUI/GUI_Interface.hpp>
  

  
*/

#include <Device/Display/DisplayInterface.hpp>
#include <Device/Mouse/MouseInterface.hpp>
#include <Device/Keyboard/KeyboardInterface.hpp>

#include <Graphics/Font/Font.hpp>

/* GUI interface. */
class GUI_Interface: public DisplayInterface, public MouseInterface, public KeyboardInterface
{
	public:
	
	bool active; /* Whether the GUI should be rendering and taking input, or is just waiting in memory. */
	
	/* Absolute coordinates that the control takes up on the screen. */
	/* All rendering should ideally use these coords to draw, so that the GUI can be resized on the fly. */
	int panelX1, panelX2, panelY1, panelY2;
	/* Width and height of the panel. */
	int panelNX, panelNY;
	/* Center point of the panel. Useful for placing centered stuff. */
	int panelCenterX, panelCenterY;

	Wildcat::Font* font; /* The font to use for text rendering. */
	
	GUI_Interface()
	{
		active=true;
		panelX1=0; panelX2=0; panelY1=0; panelY2=0; panelNX=0; panelNY=0;
	}
	
	~GUI_Interface()
	{
	}
	
	/* This is called to init the panel, but can also be called when the window is resized. */
	/* Probably make this non-virtual, but throw an eventResize() virtual function. */
	virtual void setPanel(const int _panelX1, const int _panelY1, const int _panelX2, const int _panelY2)
	{
		/* Sort the x and y coords. */
		if(_panelX1<=_panelX2)
		{ panelX1=_panelX1; panelX2=_panelX2; }
		else
		{ panelX2=_panelX1; panelX1=_panelX2; }
		if(_panelY1<=_panelY2)
		{ panelY1=_panelY1; panelY2=_panelY2; }
		else
		{ panelY2=_panelY1; panelY1=_panelY2; }
		
		panelNX=panelX2-panelX1;
		panelNY=panelY2-panelY1;
		
		panelCenterX = panelX1 + (panelNX/2);
		panelCenterY = panelY1 + (panelNY/2);
		
		eventResize();
	}
	
	/* Called when the GUI control or panel is resized, for example if the entire app window is resized. Controls should be resized and moved to appropriate relative positions. */
	virtual void eventResize()
	{
	}
	
	virtual void movePanel ( const int moveX, const int moveY )
	{ panelX1+=moveX; panelX2+=moveX; panelY1+=moveY; panelY2+=moveY; }
	
	/* Pass a mouse event to the GUI control. Return true if something happened to the control, which will stop other controls from getting the mouse event. The function doesn't need to be inherited if the control doesn't use mouse input. */
	//virtual bool mouseEvent( Mouse* )
	//{ return false; }
	
	//virtual bool keyboardEvent ( Keyboard* )
	//{ return false; }
	
	virtual void setFont(Wildcat::Font* _font)
	{ font=_font; }
	
	//DisplayInterface::mouseEvent()
	//DisplayInterface::render()
	
};


#endif
