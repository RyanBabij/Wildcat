#ifndef GRAPHICS_WINDOW_HPP
#define GRAPHICS_WINDOW_HPP

#include <vector>
#include <string>

#include <Graphics/Screen.hpp>
#include <Container/ArrayS3.hpp>
#include <Data/String.hpp>

#include <Data/Font.hpp>

#include <Graphics/BufferInterface.hpp>

class Window: public BufferInterface
{
	public:
	ArrayS3 <unsigned char> pixel;

	std::vector <BufferInterface*> vBuffer;
	String title;
	
	bool canClose;
	bool flagKill;
	
	bool hasDragBar;
	bool isDragged;
	int dragX;
	int dragY;
	
	Font *font;
	
	unsigned char backgroundColour[3];
	
	Window()
	{
		hasDragBar=true;
		isDragged=false;
		canClose=true;
		flagKill=false;
		
		dragX=0;
		dragY=0;
		
		backgroundColour[0]=100;
		backgroundColour[1]=100;
		backgroundColour[2]=100;
		
		font=0;
	}
	
	void setTitle(std::string stitle)
	{
		title.equals(stitle);
	}
	void setBackgroundColour(unsigned char red, unsigned char green, unsigned char blue)
	{
		backgroundColour[0]=red;
		backgroundColour[1]=green;
		backgroundColour[2]=blue;
	}
	void init(int startX, int startY, int endX, int endY)
	{
		BufferInterface::init(startX,startY,endX,endY);
		this->pixel.init(panelNX,panelNY,4,0);
	}
	void addBuffer(BufferInterface *buffer)
	{
		vBuffer.push_back(buffer);
	}
	void buffer(ArrayS3 <unsigned char> &pixel)
	{
		//This can be improved
		if(!flagKill)
		{
			//Background
			for(int tX=0;tX<panelNX;++tX)
			{
				for(int tY=0;tY<panelNY;++tY)
				{
					this->pixel(tX,tY,0)=backgroundColour[0];
					this->pixel(tX,tY,1)=backgroundColour[1];
					this->pixel(tX,tY,2)=backgroundColour[2];
				}
			}
			//Dragbar
			if(hasDragBar)
			{
				for(int tX=0;tX<panelNX;++tX)
				{
					for(int tY=panelNY-1;tY>panelNY-12;--tY)
					{
						this->pixel(tX,tY,0)=120;
						this->pixel(tX,tY,1)=120;
						this->pixel(tX,tY,2)=120;
					}
				}
				//Dragbar text
				if(title.string[0]!=0x24)
				{
					font->buffer(2,panelNY-10,panelNX-20,panelNY,title,&this->pixel);
				}
				if(canClose)
				{
					for(unsigned int i=2;i<11;++i)
					{
						this->pixel(panelNX-i,panelNY-i,0)=255;
						this->pixel(panelNX-i,panelNY-i,1)=255;
						this->pixel(panelNX-i,panelNY-i,2)=255;

						this->pixel(panelNX-12+i,panelNY-i,0)=255;
						this->pixel(panelNX-12+i,panelNY-i,1)=255;
						this->pixel(panelNX-12+i,panelNY-i,2)=255;
					}
				}
			}
			
			//Now copy the local pixeldata onto global pixeldata
			unsigned int tY=0;
			for(int i=panelY;i<(int)pixel.nY;++i)
			{
				unsigned int tX=0;
				for(int j=panelX;j<(int)pixel.nX;++j)
				{
					if(i>=0&&j>=0&&  tX<this->pixel.nX && tY<this->pixel.nY)
					{
						pixel(j,i,0) = this->pixel(tX,tY,0);
						pixel(j,i,1) = this->pixel(tX,tY,1);
						pixel(j,i,2) = this->pixel(tX,tY,2);
					}
					++tX;
				}
				++tY;
			}
		}
		
		for(unsigned int i=0;i<vBuffer.size();++i)
		{
			vBuffer.at(i)->buffer(pixel);
		}
		
	}
	void mouseClick(int clickType,int state,int mouseX,int mouseY)
	{
		if((mouseX>=panelX&&mouseX<panelX+panelNX) && (mouseY>=panelY&&mouseY<panelY+panelNY) && state==0)
		{
			if(hasDragBar && mouseY<panelNY+panelY && mouseY>panelY+(panelNY-12)) //Dragbar
			{
				if(canClose && mouseX > panelX+panelNX-12)
				{
					flagKill=true;
				}
				else
				{
					isDragged=true;
					dragX=mouseX-panelX;
					dragY=mouseY-panelY;
				}
			}
			else
			{
				isDragged=false;
			}
		}
		if(state==1) { isDragged=false; }
	}
	void mouseMove(int mouseX,int mouseY)
	{
		if(isDragged)
		{
			BufferInterface::move(mouseX-dragX,mouseY-dragY);
		}
	};
	/*
	void sendInput(int mouseX,int mouseY,int clickType,int state)
	{
		//Move the origin
		mouseY=height-mouseY;
		mouse.sendInput(mouseX,mouseY,clickType,state);
		
		for(unsigned int i=0;i<screenVector.size();i++)
		{
			if(screenVector.at(i)->active)
			{
				screenVector.at(i)->sendMouse(mouse);
			}
		}
		for(unsigned int i=0;i<screenVector.size();i++)
		{
			if(screenVector.at(i)->active)
			{
				screenVector.at(i)->handleEvents();
			}
		}
	}
	*/
};

#endif
