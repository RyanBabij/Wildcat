#ifndef GRAPHICS_2D_TILEBOARD_HPP
#define GRAPHICS_2D_TILEBOARD_HPP

#include <Data/Font.hpp>
#include <Container/ArrayS2.hpp>
#include <Container/ArrayS3.hpp>
#include <Container/Vector.hpp>

#include <Graphics/Buffer.hpp>
#include <Graphics/2D/Lif.hpp>
#include <Graphics/2D/HasLif.hpp>

#include <File/FileManager.hpp>

template <class T>
//Uhmm.... All T need to have implemented HasLif. I know, it's retarded.
// I mean, normally you'd just have the function use polymorphism or something,
// but whatever.
class TileBoard: public Buffer
{
	private:
	unsigned int nPixelX, nPixelY;
	//Spacing between symbols
	unsigned int hPadding;
	unsigned int vPadding;
	//where to buffer from
	int x,y,z;
	int xOffset, yOffset;
	
	int panX, panY;
	int moveX, moveY;
	
	public:
	int tileSize;
	Lif* nullTile;
	int nCurseX, nCurseY;
	
	ArrayS3 < Vector < T* > >* world;
	
	TileBoard()
	: Buffer()
	{	
		nullTile=0;
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
		
		tileSize=0;
		
		world=0;
	}
	//T needs to implement HasLif.
	void init(int x, int y, int endX, int endY, ArrayS3 < Vector < T* > >* world, int tileSize)
	{
		Buffer::init(x,y,endX,endY);
		
		this->world=world;
		this->tileSize=tileSize;
		
		nPixelX=endX-x;
		nPixelY=endY-y;
		
		nCurseX=nPixelX/tileSize;
		nCurseY=nPixelY/tileSize;
		
		xOffset=nCurseX/2;
		yOffset=nCurseY/2;
	}
	void resize(int newX, int newY)
	{
		//init(0,0,newX,newY,screen->nZ,tileSize);
		resizeAll(newX,newY);
	}
	
	void buffer()
	{
		pixel->fill(panelX,panelY,0,panelEndX,panelEndY,2,50);

		bool drawBelowLayer=false;
		if(z>0) {drawBelowLayer=true; --z;}
		
		while(true)
		{
		for(int y=0;y<nCurseY&&this->y+y<(int)world->nY;++y)
		{
			for(int x=0;x<nCurseX&&this->x+x<(int)world->nX;++x)
			{
				//Doing this squashes the z layer.
				//for(int z=0;z<(int)world->nZ;++z)
				//{
					if((*world)(this->x+x,this->y+y,z).size()>0)
					{
						for(int i=0;i<(*world)(this->x+x,this->y+y,z).size();++i)
						{
							//if((*world)(this->x+x,this->y+y,z)(i)->onGround)
							//{
								(*world)(this->x+x,this->y+y,z)(i)->getLif()->directBuffer(panelX+(x*tileSize),panelY+(y*tileSize),panelX+(x*tileSize)+tileSize,panelY+(y*tileSize)+tileSize,pixel);
							//}
						}
					}
				//}
			}
		}
		if(drawBelowLayer) { ++z; drawBelowLayer=false; }
		else { break; }
		}
		bufferAll();
	}
	void centerOn(unsigned int x, unsigned int y, unsigned int z)
	{
		this->x=x;
		this->y=y;
		this->z=z;
		this->x-=xOffset;
		this->y-=yOffset;
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
			while(moveX>(int)(tileSize+hPadding))
			{
				x+=1;
				moveX-=(tileSize+hPadding);
			}
			while(moveX<-(int)(font->nX+hPadding))
			{
				x-=1;
				moveX+=(tileSize+hPadding);
			}
			while(moveY>(int)(font->nY+vPadding))
			{
				y+=1;
				moveY-=(tileSize+vPadding);
			}
			while(moveY<-(int)(font->nY+vPadding))
			{
				y-=1;
				moveY+=(tileSize+vPadding);
			}
		}
	}
	int xTileAt(int mouseX)
	{
		return mouseX/tileSize+x;
	}
	int yTileAt(int mouseY)
	{
		return mouseY/tileSize+y;
	}
};

#endif
