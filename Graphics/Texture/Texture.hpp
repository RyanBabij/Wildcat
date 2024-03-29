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

#ifdef WILDCAT_USE_OPENGL
	#include <Graphics/OpenGL/glew-typedefs.hpp>
#endif

/* Max texture dimensions: 2^16 * 2^16 */
class Texture: public CanLoadSave, public HasTexture
{
	public:
	int nX;
	int nY;
	unsigned char type; /* RGB, RGBA */
	unsigned char* data;
	
		// Get the average colour of the texture. This allows faster rendering if the texture is very small.
	unsigned char averageRed, averageGreen, averageBlue;

	/* OpenGL pointer/reference to the texture. */
	// This reliance on OpenGL libs is kind of an issue because we can't really forward-declare typedefs like GLUint.
	// And glew.h is a pretty big header.
#ifdef WILDCAT_USE_OPENGL
	GLuint textureID;
#endif

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
		
		averageRed=0;
		averageGreen=0;
		averageBlue=0;
	}

	Texture* currentTexture() { return this; }

	void create(const int _nX, const int _nY, unsigned char _type, bool initialise=false)
	{
		nX=_nX;
		nY=_nY;
		type=_type;

      if (initialise)
      {
         data=new unsigned char[nX*nY*4]();
      }
      else
      {
         data=new unsigned char[nX*nY*4];
      }

	}
   
   void fillChannel(const unsigned char _channel, const unsigned char _val)
   {
		for(int y=0;y<nY;++y)
		{
			for(int x=0;x<nX;++x)
			{
            setPixel(x,y,_channel,_val);
			}
		}
   }
   void fill(const unsigned char value)
   {
      int nPixels = nX*nY*4;
      for (int i=0;i<nPixels;++i)
      {
         data[i]=value;
      }
   }
   
	bool load (unsigned char* data2)
	{
		DataTools::mergeOutOfArray(&nX,&data2);
		DataTools::mergeOutOfArray(&nY,&data2);
		DataTools::mergeOutOfArray(&type,&data2);

		//delete[] data;
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
   
   // bool loadArray(ArrayS3 <unsigned char> aTex)
   // {
      // //delete [] data;
   // }
   
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
   
   //copy the texture data from the given coordinates of this texture.
   void copyDown ( Texture * _tex, const int startX, const int startY)
   {
      if ( _tex==0 )
      {
         std::cout<<"WARNING: Texture::copyDown nullptr.\n";
         return;
      }
      int endX = startX+_tex->nX;
      int endY = startY+_tex->nY;
      
      int x2 = 0;
      int y2 = 0;
      
      for (int y=startY;y<endY;++y)
      {
         for (int x=startX;x<endX;++x)
         {
            if ( _tex->uPixel(x2,y2,3) != 0 )
            {
               setPixel(x,y,0,_tex->uPixel(x2,y2,0));
               setPixel(x,y,1,_tex->uPixel(x2,y2,1));
               setPixel(x,y,2,_tex->uPixel(x2,y2,2));
               setPixel(x,y,3,_tex->uPixel(x2,y2,3));
            }
            
            ++x2;
         }
         
         x2=0;
         ++y2;
      }
   }
	
   //copy with RGB value.
	// This copies any pixel of the passed texture down to this texture, but sets every pixel to the given RGB values.
	// Useful for example if you are blitting some text that you want in a certain colour.
   void copyDown ( Texture * _tex, const int startX, const int startY, unsigned char red, unsigned char green, unsigned char blue)
   {
      if ( _tex==0 )
      {
         std::cout<<"WARNING: Texture::copyDown nullptr.\n";
         return;
      }
      int endX = startX+_tex->nX;
      int endY = startY+_tex->nY;
      
      int x2 = 0;
      int y2 = 0;
      
      for (int y=startY;y<endY;++y)
      {
         for (int x=startX;x<endX;++x)
         {
            if ( _tex->uPixel(x2,y2,3) != 0 )
            {
               setPixel(x,y,0,red);
               setPixel(x,y,1,green);
               setPixel(x,y,2,blue);
               setPixel(x,y,3,_tex->uPixel(x2,y2,3));
            }
            
            ++x2;
         }
         
         x2=0;
         ++y2;
      }
   }
   // same as copydown but average the source and target pixel. (need to account for alpha)
   void blendDown ( Texture * _tex, const int startX, const int startY)
   {
      if ( _tex==0 )
      {
         std::cout<<"WARNING: Texture::copyDown nullptr.\n";
         return;
      }
      int endX = startX+_tex->nX;
      int endY = startY+_tex->nY;
      
      int x2 = 0;
      int y2 = 0;
      
      for (int y=startY;y<endY;++y)
      {
         for (int x=startX;x<endX;++x)
         {
            if ( _tex->uPixel(x2,y2,3) != 0 )
            {
               const unsigned char r = (int)uPixel(x,y,0)+_tex->uPixel(x2,y2,0)/2;
               const unsigned char g = (int)uPixel(x,y,1)+_tex->uPixel(x2,y2,1)/2;
               const unsigned char b = (int)uPixel(x,y,2)+_tex->uPixel(x2,y2,2)/2;
               const unsigned char a = (int)uPixel(x,y,3)+_tex->uPixel(x2,y2,3)/2;
               
               setPixel(x,y,0,r);
               setPixel(x,y,1,g);
               setPixel(x,y,2,b);
               setPixel(x,y,3,a);
            }
            
            ++x2;
         }
         
         x2=0;
         ++y2;
      }
   }
   // Similar to copydown, but only changes the pixel values by a certain amount.
   void morphDown( Texture* _tex, const int startX, const int startY, const unsigned char morphAmount)
   {
      if ( _tex==0 )
      {
         std::cout<<"WARNING: Texture::copyDown nullptr.\n";
         return;
      }
      int endX = startX+_tex->nX;
      int endY = startY+_tex->nY;
      
      int x2 = 0;
      int y2 = 0;
      
      for (int y=startY;y<endY;++y)
      {
         for (int x=startX;x<endX;++x)
         {
            
            // setPixel(x,y,0,_tex->uPixel(x2,y2,0));
            // setPixel(x,y,1,_tex->uPixel(x2,y2,1));
            // setPixel(x,y,2,_tex->uPixel(x2,y2,2));
            //setPixel(x,y,3,_tex->uPixel(x2,y2,3));
            
            //unsigned char moveVal = DataTools::moveTo(_tex->uPixel(x,y,0),uPixel(x,y,0),10);
            
            setPixel(x,y,0,DataTools::moveTo(uPixel(x,y,0),_tex->uPixel(x2,y2,0),morphAmount));
            setPixel(x,y,1,DataTools::moveTo(uPixel(x,y,1),_tex->uPixel(x2,y2,1),morphAmount));
            setPixel(x,y,2,DataTools::moveTo(uPixel(x,y,2),_tex->uPixel(x2,y2,2),morphAmount));
            setPixel(x,y,3,DataTools::moveTo(uPixel(x,y,3),_tex->uPixel(x2,y2,3),morphAmount));

            
            ++x2;
         }
         
         x2=0;
         ++y2;
      }
   }
   void bloom() // currently just provides a basic outline effect for demo
   {
      // add brightness to pixels next to other bright pixels
      for (int y=1;y<nY-1;++y)
      {
         for (int x=1;x<nX-1;++x)
         {
            for (int channel=0;channel<4;++channel)
            {
               
               unsigned char currentPixel = uPixel(x,y,channel);
               if ( currentPixel<100 )
               {
                  // get brightest neighbor.
                  unsigned char brightestNeighbor = 0;
                  if ( uPixel(x-1,y-1,channel) > brightestNeighbor )
                  {
                     brightestNeighbor=uPixel(x-1,y-1,channel);
                  }
                  if ( uPixel(x-1,y,channel) > brightestNeighbor )
                  {
                     brightestNeighbor=uPixel(x-1,y,channel);
                  }
                  if ( uPixel(x-1,y+1,channel) > brightestNeighbor )
                  {
                     brightestNeighbor=uPixel(x-1,y+1,channel);
                  }
                  if ( uPixel(x+1,y,channel) > brightestNeighbor )
                  {
                     brightestNeighbor=uPixel(x+1,y,channel);
                  }
                  if ( uPixel(x+1,y-1,channel) > brightestNeighbor )
                  {
                     brightestNeighbor=uPixel(x+1,y-1,channel);
                  }
                  if ( uPixel(x+1,y+1,channel) > brightestNeighbor )
                  {
                     brightestNeighbor=uPixel(x+1,y+1,channel);
                  }
                  if ( uPixel(x,y+1,channel) > brightestNeighbor )
                  {
                     brightestNeighbor=uPixel(x,y+1,channel);
                  }
                  if ( uPixel(x,y-1,channel) > brightestNeighbor )
                  {
                     brightestNeighbor=uPixel(x,y-1,channel);
                  }
                  if (brightestNeighbor>=200)
                  {
                     setPixel(x,y,channel,100);
                  }
               }
               
               
            }
            

            

            
         }
      }
   }
};

#endif
