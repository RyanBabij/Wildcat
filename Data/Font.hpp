#define FONT_HPP
#ifndef FONT_HPP
#define FONT_HPP

#include <iostream>

#include <GL/glut.h>

#include <Data/String.hpp>
#include <Container/ArrayS3/ArrayS3.hpp>

typedef unsigned char byte;

class Font
{
/*
	public:
	static const unsigned char header[];
	ArrayS3 <byte> * array;
	unsigned int nCharacters;

	unsigned int nX;
	unsigned int nY;

	Font()
	{
	}
	void unpack(unsigned char * stream)
	{
		unsigned int width=stream[0];
		unsigned int height=stream[1];

		nX=width;
		nY=height;

		unsigned int length=DataTools::find<unsigned char>(stream,2,2);
		nCharacters=(length-2)/(width*height);

		ArrayS3 <unsigned char> tArray;
		tArray.init(width,height,nCharacters,0);
		tArray.loadXMajor(stream+2);

		array = new ArrayS3 <unsigned char> [nCharacters];
		for(unsigned int i=0;i<nCharacters;i++)
		{
			array[i].init(width,height,4,0);
		}
		//Load into raw data for OpenGL
		for(unsigned int z=0;z<tArray.nZ;z++)
		{
			for(unsigned int y=0;y<tArray.nY;y++)
			{
				for(unsigned int x=0;x<tArray.nX;x++)
				{
					if(tArray(x,y,z)==1)
					{
						array[z](x,y,0)=255;
						array[z](x,y,1)=255;
						array[z](x,y,2)=255;
						array[z](x,y,3)=1;
					}
				}
			}
			//For some reason shit renders upside down
			array[z].flipHorizontal();
		}
	}
	void test()
	{
			for(unsigned int i=0;i<nCharacters;i++)
			{
				glRasterPos2i(i*10+2,100);
				glDrawPixels(array[i].nX, array[i].nY, GL_RGBA, GL_UNSIGNED_BYTE, array[i].data);
			}
		//glFlush();
	}
	void render(unsigned int x, unsigned int y, byte * text)
	{
		unsigned int i=0;
		while(text[i]!=0x24)
		{
				glRasterPos2i(x+i*10,y);
				glDrawPixels(array[text[i]].nX, array[text[i]].nY, GL_RGBA, GL_UNSIGNED_BYTE, array[text[i]].data);
				i++;
		}
	}
	void buffer(unsigned int x, unsigned int y, byte * text, ArrayS3 <byte> & a)
	{
		unsigned int i=0;
		while(text[i]!=0x24)
		{
			//a->mergeGraphics(x,y,&array[text[i]]);
			a.mergeGraphics(x,y,&array[text[i]]);

			++i;
			x+=array[text[i]].nX+2;
		}
	}

	void buffer(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, String text, ArrayS3 <byte> & a)
	{
		unsigned int i=0;
		while(text.string[i]!=0x24)
		{
			if(x1+array[text.string[i]].nX<x2)
			{
				a.mergeGraphics(x1,y1,&array[text.string[i]]);
				++i;
				x1+=array[text.string[i]].nX+2;
			}
			else { return; }
		}
	}

	void buffer(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, std::string text, ArrayS3 <byte> & a)
	{
		unsigned int i=0;
		while(text[i]!=0x00)
		{
			if(x1+array[text[i]].nX<x2)
			{
				a.mergeGraphics(x1,y1,&array[text[i]]);
				++i;
				x1+=array[text[i]].nX+2;
			}
			else { return; }
		}
	}
	

	inline void bufferCharacter(const unsigned int x, const unsigned int y, const unsigned char character, ArrayS3 <unsigned char> * a)
	{
		if(x+nX < a->nX && y+nY < a->nY)
		{
			a->mergeGraphics(x,y,&array[character]);
		}
	}

	void buffer(unsigned int x, unsigned int y, String text, ArrayS3 <unsigned char> * a)
	{
		unsigned int i=0;
		while(text.string[i]!=0x24)
		{
			//merge in

			a->mergeGraphics(x,y,&array[text.string[i]]);
			++i;
			x+=array[text.string[i]].nX+2;
		}
	}
	*/
};
//const unsigned char Font::header[] ={0x3E,0x2B};


#endif
