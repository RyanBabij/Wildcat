#ifndef GRAPHICS_SCREEN_H
#define GRAPHICS_SCREEN_H

#include <GL/glut.h>
#include <string>

#include <Container/ArrayS3.hpp>
#include <Graphics/UI/Button.hpp>
#include <Graphics/Image/Animation.hpp>
#include <Time/Timer.hpp>
#include <Device/Mouse.hpp>
#include <Data/Font.hpp>
#include <Data/String.hpp>
#include <Math/Operation.hpp>

class Screen
{
	public:
	
	bool active;
	
	unsigned int nX;
	unsigned int nY;
	unsigned int gridSize;
	
	//value from 0-100;
	double relativeX;
	double relativeWidth;
	double relativeY;
	double relativeHeight;
	
	unsigned int actualX;
	unsigned int actualY;
	
	unsigned int emptyRed;
	unsigned int emptyGreen;
	unsigned int emptyBlue;
	
	Font font;
	
	ArrayS3 <BYTE> pixel;
	//std::vector <DynamicArray <BYTE> > tileSet;
	
	std::vector <Animation> animation;
	std::vector <Button> button;
	std::vector <Image> image;
	
	double refreshSpeed;
	Timer timer;
	
	Screen()
	{
		active=false;
		
		nX=0;
		nY=0;
		gridSize=0;
		
		emptyRed=0;
		emptyGreen=0;
		emptyBlue=0;
		
		relativeX=0;
		relativeWidth=0;
		relativeY=0;
		relativeHeight=0;
		
		refreshSpeed=0;
		timer.init();
		timer.start();
	}
	void init(unsigned int x, unsigned int y)
	{
		nX=x;
		nY=y;
		pixel.init(x,y,4,0);
	}
	
	void useFont(unsigned char * fontData)
	{
		font.unpack(fontData);
	}
	
	void text(unsigned int startX, unsigned int startY, String text)
	{
		font.buffer(startX, startY, text, &pixel);
	}
	
	void fillColour(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int r, unsigned int g, unsigned int b)
	{
		pixel.fill(x1,y1,0,x2,y2,0,r);
		pixel.fill(x1,y1,1,x2,y2,1,g);
		pixel.fill(x1,y1,2,x2,y2,2,b);
	}
	
	
	virtual void handleEvent(unsigned int menuItemID){}
	virtual void handleMouse(Mouse mouse){}

	//void useFont(CharacterSet f)
	//{
	//	font=f;
	//}
	//void loadTextBox(
	void updatePosition(double x1, double y1, double x2, double y2)
	{
		//These are generally just set once during init.
		relativeX=x1;
		relativeY=y1;
		relativeWidth=x2-x1;
		relativeHeight=y2-y1;
	}
	void setBackgroundColour(unsigned int red, unsigned int green, unsigned int blue)
	{
		emptyRed=red;
		emptyGreen=green;
		emptyBlue=blue;
		//pixel.fillZ(0,red);
		//pixel.fillZ(1,green);
		//pixel.fillZ(2,blue);
	}
	//Load a tile into buffer
	void putTile(unsigned int x, unsigned int y, unsigned int tileID)
	{
		for(unsigned int y2=0;y2<gridSize;y2++)
		{
			for(unsigned int x2=0;x2<gridSize;x2++)
			{
				
				//pixel(x+x2,y+y2,0,255);
				//pixel(x+x2,y+y2,1,255);
				//pixel(x+x2,y+y2,2,255);
				
				//pixel.write(x+x2,y+y2,0,tileSet.at(tileID).read(x2,y2,0));
				//pixel.write(x+x2,y+y2,1,tileSet.at(tileID).read(x2,y2,1));
				//pixel.write(x+x2,y+y2,2,tileSet.at(tileID).read(x2,y2,2));
			}
		}
	}
	//void textBox(unsigned int x, unsigned int y, std::string text)
	//{
	//	DynamicArray <bool> text = font.getText(text);
	//}
	void loadTileset(std::string filename)
	{
		BMPClass tileBmp;
		BMPLoad(filename,tileBmp);
		for(int startY=tileBmp.height-gridSize;startY>0;startY-=gridSize)
		{
			for(unsigned int startX=0;startX<=tileBmp.width-gridSize;startX+=gridSize)
			{
				//ArrayStatic3D <BYTE> tempTile;
				//tempTile.init(gridSize, gridSize, 3);
				
				for(unsigned int x=0;x<gridSize;x++)
				{
					for(unsigned int y=0;y<gridSize;y++)
					{
						//tempTile.write(x,y,0,tileBmp.pixel(startX+x,startY+y,0));
						//tempTile.write(x,y,1,tileBmp.pixel(startX+x,startY+y,1));
						//tempTile.write(x,y,2,tileBmp.pixel(startX+x,startY+y,2));
					}
				}
				//tileSet.push_back(tempTile);
			}
		}
	}
	void putImage(unsigned int x, unsigned int y, unsigned int pictureID)
	{
		/*
		for(unsigned int y2=0;y2<image.at(pictureID).numY;y2++)
		{
			for(unsigned int x2=0;x2<image.at(pictureID).numX;x2++)
			{
				//pixel.write(x+x2,y+y2,0,image.at(pictureID).read(x2,y2,0));
				//pixel.write(x+x2,y+y2,1,image.at(pictureID).read(x2,y2,1));
				//pixel.write(x+x2,y+y2,2,image.at(pictureID).read(x2,y2,2));
			}
		}
		*/
	}

	//put all data into a pixel map.
	void flatten()
	{
		//squash all the objects onto a pixelmap
		for(unsigned int i=0;i<animation.size();i++)
		{
			//pixel.merge(animation.at(i).x, animation.at(i).y, 0, animation.at(i).getArray());
		}
		for(unsigned int i=0;i<image.size();i++)
		{
			//pixel.merge(image.at(i).x, image.at(i).y, 0, image.at(i).getArray());
		}
		for(unsigned int i=0;i<button.size();i++)
		{
			//pixel.merge(button.at(i).x, button.at(i).y, 0, button.at(i).getArray());
		}
	}
	void loadAnimation(Animation _animation)
	{
		animation.push_back(_animation);
	}
	void loadImage(Image _image)
	{
		image.push_back(_image);
	}
	void loadImage(std::string fileName)
	{
		Image _image;
		_image.loadBmp(fileName);
		image.push_back(_image);
	}
	void loadButton(Button _button)
	{
		button.push_back(_button);
	}
	unsigned int loadButton(std::string normal, std::string mouseOver)
	{
		Button _button;
		_button.load("Begin1.bmp","Begin2.bmp");
		_button.ID=button.size();
		button.push_back(_button);
		return _button.ID;
	}
	void putAnimation(unsigned int x, unsigned int y, unsigned int animationID)
	{
		animation.at(animationID).x=x;
		animation.at(animationID).y=y;
	}
	std::wstring toString(unsigned int scaleX, unsigned int scaleY, unsigned int scaleZ)
	{
		//return pixel.toString(scaleX, scaleY, scaleZ);
		return L"no";
	}
	void tick()
	{
		timer.update();
		if(timer.fullSeconds>=refreshSpeed)
		{
			for(unsigned int i=0;i<animation.size();i++)
			{
				animation.at(i).tick();
				timer.start();
			}
		}
	}
	void sendMouse(Mouse mouse)
	{
		if(mouse.inBounds(actualX,actualY,actualX+nX,actualY+nY))
		{
			mouse.adjustRelative(actualX,actualY);
			for(unsigned int i=0;i<button.size();i++)
			{
				button.at(i).sendMouse(mouse);
			}
			handleMouse(mouse);
		}
	}
	void handleEvents()
	{
		int eventID=nextEvent();
		while(eventID!=-1)
		{
			handleEvent(eventID);
			eventID=nextEvent();
		}
	}
	int nextEvent()
	{
		for(unsigned int i=0;i<button.size();i++)
		{
			if(button.at(i).isClicked)
			{
				button.at(i).isClicked=false;
				return i;
			}
		}
		return -1;
	}
	virtual ~Screen(){}
};
#endif
