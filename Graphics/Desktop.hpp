#ifndef GRAPHICS_DESKTOP_HPP
#define GRAPHICS_DESKTOP_HPP

#include <vector>

#include <Container/ArrayS3.hpp>
#include <Graphics/Window.hpp>
#include <Data/Font.hpp>
#include <Misc/TempFont.hpp>

//We manage the dragbars and whatnot from here.

class Desktop
{
	public:
	std::vector <Window*> vWindow;
	int pX, pY;
	int nX, nY;
	Window * dragged;
	
	Font font;
	
	Desktop()
	{
		pX=0;
		pY=0;
		nX=0;
		nY=0;
		dragged=0;
		//Load my temp font
		font.unpack(TempFont::getFont());	
	}
	inline void position(int x, int y)
	{
		pX=x;
		pY=y;
	}
	inline void size(int x, int y)
	{
		nX=x;
		nY=y;
	}
	inline void addWindow(Window &window)
	{
		window.font=&font;
		vWindow.push_back(&window);
	}
	inline void mouseClick(int clickType, int state, int mouseX, int mouseY)
	{
		for(unsigned int i=0;i<vWindow.size();++i)
		{
			vWindow.at(i)->mouseClick(clickType, state, mouseX, mouseY);
			if(vWindow.at(i)->isDragged) { dragged=vWindow.at(i); }
			if(vWindow.at(i)->flagKill) { vWindow.erase(vWindow.begin()+i); }
		}
		if(state==1) { dragged=0; }
	}
	void buffer(ArrayS3 <unsigned char> &pixel)
	{
		for(unsigned char i=0;i<vWindow.size();++i)
		{
			vWindow.at(i)->buffer(pixel);
		}
	}
	inline void mouseMove(int mouseX, int mouseY)
	{
		if(dragged)
		{
			dragged->position(mouseX-dragged->dragX,nY-mouseY-dragged->dragY);
		}
	}
};

#endif
