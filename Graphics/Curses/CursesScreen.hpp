#pragma once
#ifndef WILDCAT_GRAPHICS_CURSES_CURSESSCREEN_HPP
#define WILDCAT_GRAPHICS_CURSES_CURSESSCREEN_HPP

/*
   This is extremely old terrible code which doesn't work. I mean, it uses
   glDrawPixels ffs. However I like the idea of a Curses emulator so I'll
   keep this here as a placeholder.
*/

#include <Data/Font.hpp>

class CursesScreen
{
	public:
	ArrayS3 <unsigned char> screen;
	Font * font;
	unsigned int nX;
	unsigned int nY;
	
	unsigned int padding;
	
	CursesScreen()
	{
		nX=0;
		nY=0;
		padding=0;
	}

	void init(unsigned int width, unsigned int height, Font * font)
	{
		this->font=font;
		nX=width/font->nX;
		nY=height/font->nY;
		
		if(nX%2==1) { nX--; }
		if(nY%2==1) { nY--; }
		
		screen.init(nX*(font->nX+padding),nY*(font->nY+padding),4,0);
	}
	void render(unsigned int startX, unsigned int startY)
	{
		glRasterPos2i(startX,startY);
		glDrawPixels(screen.nX, screen.nY, GL_RGBA, GL_UNSIGNED_BYTE, screen.data);
		glFlush();
	}
	void curse(unsigned int x, unsigned int y, unsigned char curseID)
	{
		//The coords are gridded down.
		if(x<nX&&y<nY)
		{
			unsigned char text [2] = { curseID, 0x24 };
			font->buffer(x*(font->nX+padding),y*(font->nY+padding),text,&screen);
		}
	}
	void text(unsigned int x, unsigned int y, String string)
	{
		for(unsigned int i=0;i<string.length();i++)
		{
			curse(x+i,y,string.string[i]);
		}
	}
};

#endif
