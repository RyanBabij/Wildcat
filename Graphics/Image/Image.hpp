#ifndef IMAGE_H
#define IMAGE_H

#include <Graphics/Image/BmpLoader.hpp>
#include <Data/Byte.hpp>

class Image
{
	//Ryan raw image format.
	public:
	//x*y*4
	//RGB and transparency
	ArrayS3 <byte> rgb;
	
	unsigned int height;
	unsigned int width;
	
	unsigned int x;
	unsigned int y;
	
	Image()
	{
		x=0;
		y=0;
		height=0;
		width=0;
	}
	
	bool liesOn(unsigned int _x, unsigned int _y)
	{
		if(_x>width+x||_x<x||_y>height+y||_y<y)
		{
			return false;
		}
		return true;
	}
	
	byte * getStream()
	{
		return rgb.data;
	}
	ArrayS3 <byte> getArray()
	{
		return rgb;
	}
	Image(std::string filename)
	{
		loadBmp(filename);
	}
	void loadArray(ArrayS3 <byte> array)
	{
		rgb=array;
		width=array.nX;
		height=array.nY;
	}
	void loadBmp(std::string filename)
	{
		BMPClass bmp;
		BMPLoad(filename,bmp);
		
		rgb.init(bmp.width, bmp.height, 3, 0);
		//rgb.loadStream(bmp.bytes);
		//rgb.write(0,0,0,0);
		width=bmp.width;
		height=bmp.height;
	}
	//void rip(std::string fileName, unsigned int x, unsigned int 
	void saveBmp(std::string filename)
	{
	}
	void scale()
	{
	}
	void rotate()
	{
	}
	//crop invert etc.
	
/**********************************************************************/
	void draw(unsigned int x, unsigned int y)
	{
	//Doesn't use transparency cos we have nothing to compare to anyway.
		glRasterPos2i(x, y);
		glDrawPixels(rgb.nX, rgb.nY, GL_RGB, GL_UNSIGNED_BYTE, rgb.data);
		glFlush();
	}
/**********************************************************************/
};

#endif
