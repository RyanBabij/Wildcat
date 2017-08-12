#ifndef GRAPHICS_2D_CURSES_HPP
#define GRAPHICS_2D_CURSES_HPP

#include <Data/Font.hpp>
#include <Container/ArrayS2.hpp>

#include <Graphics/Buffer.hpp>

class Curses: public Buffer
{
	private:
	//Size of the panel in curses.
	int nCurseX, nCurseY;
	unsigned int nPixelX, nPixelY;
	//What to display if we run off the array.
	unsigned char nullCurse;
	//Spacing between symbols
	unsigned int hPadding;
	unsigned int vPadding;
	//where to buffer from
	int x,y;
	ArrayS2 <unsigned char> * screen;
	int xOffset, yOffset;
	
	int panX, panY;
	int moveX, moveY;
	
	public:
	Curses()
	: Buffer()
	{
		screen=0;
		
		nullCurse=0;
		hPadding=0;
		vPadding=0;
		x=0;
		y=0;
		nCurseX=0;
		nCurseY=0;
		nPixelX=0;
		nPixelY=0;
		xOffset=0;
		yOffset=0;
		
		panX=-1;
		panY=-1;
		
		moveX=0;
		moveY=0;

	}
	void init(unsigned int width, unsigned int height, unsigned int hSpacing, unsigned int vSpacing, ArrayS2 <unsigned char> *pixel,unsigned char nullCurse)
	{
		Buffer::init(0,0,width,height);
		
		this->nullCurse=nullCurse;

		this->screen=pixel;
		hPadding=hSpacing;
		vPadding=vSpacing;
		nPixelX=width;
		nPixelY=height;
		nCurseX=width/(hSpacing+font->nX);
		nCurseY=height/(vSpacing+font->nY);
		
		xOffset=nCurseX/2;
		yOffset=nCurseY/2;
	}
	void init(unsigned int x, unsigned int y, unsigned int endX, unsigned int endY)
	{
		Buffer::init(x,y,endX,endY);
		
		nPixelX=endX-x;
		nPixelY=endY-y;
		nCurseX=nPixelX/(hPadding+font->nX);
		nCurseY=nPixelY/(vPadding+font->nY);
		
		xOffset=nCurseX/2;
		yOffset=nCurseY/2;
	}
	void resize(int newX, int newY)
	{
		init(0,0,newX,newY);
		resizeAll(newX,newY);
	}
	
	void buffer()
	{
		pixel->fill(panelX,panelY,0,panelEndX,panelEndY,2,0);
		for(int y=0;y<nCurseY;++y)
		{
			for(int x=0;x<nCurseX;++x)
			{
				if(x+this->x < screen->nX && y+this->y < screen->nY && x+this->x >= 0 && y+this->y >= 0)
				{
					font->bufferCharacter(x*(font->nX+hPadding),y*(font->nY+vPadding),*screen->index(x+this->x,y+this->y),pixel);
				}
				else
				{
					font->bufferCharacter(x*(font->nX+hPadding),y*(font->nY+vPadding),nullCurse,pixel);
				}
			}
		}
		bufferAll();
	}
	void centerOn(unsigned int x, unsigned int y)
	{
		this->x=x;
		this->y=y;
		if(this->x>=xOffset)
		{
			this->x-=xOffset;
		}
		else { this->x=0; }
		if(this->y>=yOffset)
		{
			this->y-=yOffset;
		}
		else { this->y=0; }
	}
	void mouseClick()
	{
		if(mouse->isLeftClick)
		{
			panX=mouse->x;
			panY=mouse->y;
		}
	}
	void mouseMove()
	{
		if(mouse->isLeftClick)
		{
			moveX += panX-mouse->x;
			panX=mouse->x;
			moveY += panY-mouse->y;
			panY=mouse->y;
			while(moveX>(int)(font->nX+hPadding))
			{
				x+=1;
				moveX-=(font->nX+hPadding);
			}
			while(moveX<-(int)(font->nX+hPadding))
			{
				x-=1;
				moveX+=(font->nX+hPadding);
			}
			while(moveY>(int)(font->nY+vPadding))
			{
				y+=1;
				moveY-=(font->nY+vPadding);
			}
			while(moveY<-(int)(font->nY+vPadding))
			{
				y-=1;
				moveY+=(font->nY+vPadding);
			}
		}
	}
};

#endif
