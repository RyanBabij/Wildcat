#pragma once
#ifndef WILDCAT_GRAPHICS_TEXTURE_HPP
#define WILDCAT_GRAPHICS_TEXTURE_HPP

/*
	#include <Graphics/Texture/Texture.hpp>

	DESCRIPTION

	Internal texture and image format
*/

#include <Data/DataTools.hpp>
#include <File/CanLoadSave.hpp>
#include <Interface/HasTexture.hpp> // TEXTURE HAS A TEXTURE...

#include <cmath>

/* Max texture dimensions: 2^16 * 2^16 */
class Texture: public CanLoadSave, public HasTexture
{
	public:
	int nX;
	int nY;
	unsigned char type; /* RGB, RGBA */
	unsigned char* data;

	/* OpenGL pointer/reference to the texture. */
	GLuint textureID;

		// LINUX G++ GIVES WARNING IF I TRY TO DELETE A POLYMORPHIC OBJECT WITHOUT A VIRTUAL DESTRUCTOR.
	virtual ~Texture()
	{
		// GARBAGE COLLECTION MUST BE DONE AT SOME POINT.
	}

	Texture()
	{
		data=0;

		/* NOTE: Earlier notes suggest that GLuint texture references need to be initialised. */
		textureID=0;
	}

	Texture* currentTexture() { return this; }

	void create(const int _nX, const int _nY, unsigned char _type)
	{
		nX=_nX;
		nY=_nY;
		type=_type;

		data=new unsigned char[nX*nY*4];
	}
	bool load (unsigned char* data2)
	{
		DataTools::mergeOutOfArray(&nX,&data2);
		DataTools::mergeOutOfArray(&nY,&data2);
		DataTools::mergeOutOfArray(&type,&data2);

		delete[] data;
		if(type==1)
		{
			const int _nData=nX*nY*4;
			data=new unsigned char[_nData];
			unsigned char* iData=data2;
			for(int i=0;i<_nData;++i)
			{
				DataTools::mergeOutOfArray(&data[i],&iData);
			}
			return true;
		}
		return false;
	}
	unsigned char *save()
	{
		unsigned char* objectData=new unsigned char[9+(nX*nY*4)];
		unsigned char* dataPos=objectData;
		DataTools::mergeIntoArray(&nX,&dataPos);
		DataTools::mergeIntoArray(&nY,&dataPos);
		DataTools::mergeIntoArray(&type,&dataPos);
		for(int i=0;i<nX*nY*4;++i)
		{ DataTools::mergeIntoArray(&data[i],&dataPos);
		}
		return objectData;
	}
	unsigned int getObjectSize()
	{ return 9+(nX*nY*4); }


	/*inline char pixel(int x, int y, int channel)
	{
		//return data[(x*4)*(nY*y)+z];
		return data [((y*nX)*4)+(x*4)+channel];
	}*/

	inline unsigned char getPixel(const int _x, const int _y, const int channel)
	{ return (unsigned char)data [((_y*nX)*4)+(_x*4)+channel];
	}

	/* deprecated */
	inline unsigned char uPixel(int x, int y, int channel)
	{
		return (unsigned char)data [((y*nX)*4)+(x*4)+channel];
	}
	inline void setPixel(const int _x, const int _y, const int _channel, const unsigned int value)
	{
		data [((_y*nX)*4)+(_x*4)+_channel] = value;
	}

	Texture* createMipMap()
	{	/* creates a texture that is exactly 1/2 x 1/2. Returns 0 if texture is 1x1. */
		if(nX==1||nY==1) { return 0; }

		Texture* texture = new Texture;
		texture->create(nX/2,nY/2,type);

		for(int i=0;i<texture->nX*texture->nY*4;++i)
		{
			texture->data[i]=0;
		}
		int i=0;
		for(int y=0;y<nY-1;y+=2)
		{
			for(int x=0;x<nX-1;x+=2)
			{
				for(int z=0;z<4;++z)
				{
					double result=0;
					result=uPixel(x,y,z)+uPixel(x+1,y,z)+uPixel(x,y+1,z)+uPixel(x+1,y+1,z);
					result/=4;
					unsigned char cResult = result;
					texture->data[i++]=cResult;
				}
			}
		}
		return texture;
	}

	Texture* rotate90Clockwise()
	{
		Texture* texture = new Texture;
		texture->create(nY,nX,type);


		for(int _y=0;_y<nY;++_y)
		{
			for(int _x=0;_x<nX;++_x)
			{
				texture->setPixel(nY-_y-1,_x,0,getPixel(_x,_y,0));
				texture->setPixel(nY-_y-1,_x,1,getPixel(_x,_y,1));
				texture->setPixel(nY-_y-1,_x,2,getPixel(_x,_y,2));
				texture->setPixel(nY-_y-1,_x,3,getPixel(_x,_y,3));
			}
		}

		return texture;
	}

	Texture* cropCentered(const int cropX, const int cropY)
	{
		Texture* texture = new Texture;
		texture->create(cropX,cropY,type);

		const int startX = (nX/2)-(cropX/2);
		//const int endX = (nX/2)+(cropX/2);

		const int startY = (nY/2)-(cropY/2);
		//const int endY = (nY/2)+(cropY/2);

		int x1=startX;
		int y1=startY;

		std::cout<<"Crop loop... ";
		for(int x2=0;x2<cropX;++x2)
		{
			for(int y2=0;y2<cropY;++y2)
			{
				if(x1<0||y1<0||x1>=nX||y1>=nY)
				{
					texture->setPixel(x2,y2,0,0);
					texture->setPixel(x2,y2,1,0);
					texture->setPixel(x2,y2,2,0);
					texture->setPixel(x2,y2,3,0);
				}
				else
				{
					texture->setPixel(x2,y2,0, getPixel(x1,y1,0));
					texture->setPixel(x2,y2,1, getPixel(x1,y1,1));
					texture->setPixel(x2,y2,2, getPixel(x1,y1,2));
					texture->setPixel(x2,y2,3, getPixel(x1,y1,3));
				}
				++y1;
			}
			++x1;
			y1=startY;
		}
		std::cout<<"End crop loop\n";
		return texture;
	}

	void print()
	{
		for(int y=0;y<nY;++y)
		{
			for(int x=0;x<nX;++x)
			{
				for(int z=0;z<4;++z)
				{
					std::cout<<(int)uPixel(x,y,z);
				}std::cout<<"*";
			}std::cout<<"\n";
		}std::cout<<"\n\n";
	}
	void printRed()
	{
		for(int y=0;y<nY;++y)
		{
			for(int x=0;x<nX;++x)
			{
				std::cout<<(int)uPixel(x,y,0);
				std::cout<<" ";
			}std::cout<<"\n";
		}std::cout<<"\n\n";
	}
};

#endif
