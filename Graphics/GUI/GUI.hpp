#pragma once
#ifndef WILDCAT_GRAPHICS_GUI_GUI_HPP
#define WILDCAT_GRAPHICS_GUI_GUI_HPP

/* Wildcat: GUI
  #include <Graphics/GUI/GUI.hpp>
  
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

#endif
