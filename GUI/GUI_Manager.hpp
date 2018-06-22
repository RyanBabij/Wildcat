#pragma once
#ifndef WILDCAT_GUI_MANAGER_HPP
#define WILDCAT_GUI_MANAGER_HPP

/*
# include <GUI/GUI_Manager.hpp>

GUI Manager merges three basic device functionalities: Display, mouse and keyboard.
Most projects will probably be better off just using GUI Manager than trying to
implement the three basic functionalities seperately.

Currently only KeyboardInterfaceManager is implemented properly.

Notes:
	* KeyboardInterface has stealKeyboard() for instances where it wants to get all
	keyboard input. Consider stealRender() and stealMouse() for the other two.
*/

#include <GUI/GUI_Interface.hpp>
#include <Device/Display/DisplayInterface.hpp>
#include <Device/Mouse/MouseInterface.hpp>
#include <Device/Keyboard/KeyboardInterface.hpp>

#include <Device/Keyboard/KeyboardInterfaceManager.hpp>

/* GUI_Manager is the container for GUI objects. */
class GUI_Manager: public DisplayInterface, public MouseInterface, public KeyboardInterface
{
	public:
	bool active; /* Used to switch the menu off and on. */
	/* Vector of all GUI controls. */
	Vector <GUI_Interface*> vGUI;
	
	KeyboardInterfaceManager keyboardManager;
	
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

		// Make sure to update this for mouse etc later.
	void clearMemory()
	{
		vGUI.clear();
		keyboardManager.clear();
	}
		inline void clear()
		{ clearMemory(); }

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
	void add(GUI_Interface* gui)
	{
		vGUI.push(gui);
		keyboardManager.add(gui);
	}
		void addControl(GUI_Interface* gui)
		{ add(gui); }


	/* Remove a control from the manager. It will no longer recieve events or render. */
	void remove(GUI_Interface* gui)
	{
		vGUI.erase(gui);
		keyboardManager.remove(gui);
		selectedControl=0; /* deprecated */
	}
		void removeControl(GUI_Interface* gui)
		{ remove(gui); }

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
			if (keyboardManager.keyboardEventAll(keyboard))
			{
				return true;
			}
			
			/* Selectedcontrol is now outdated and replaced with stealKeyboard() and other steal functions. */
			
			/* Let the selectedcontrol get the first chance at input. */
			// if(selectedControl!=0 && selectedControl->keyboardEvent(keyboard)==true)
			// { return true; }
			// for(int i=0;i<vGUI.size();++i)
			// {
				// if(vGUI(i)->keyboardEvent(keyboard)==true)
				// {
					// /* Interesting idea. If a control uses device input, then it is selected. */
					// selectedControl=vGUI(i);
					// return true;
				// }
			// }
		}
		return false;
	}
	
	bool stealKeyboard()
	{
		if (active)
		{
			return keyboardManager.stealKeyboard();
		}
		return false;
	}

};

#endif