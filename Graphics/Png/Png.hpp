#pragma once

/*
	Png.hpp
	#include <Graphics/Png/Png.hpp>

	Handling of PNG format.
	
	Does the following:
		* Accept raw PNG file data and turns it into Array (2D/3D) format.
		* Convert Array (2D/3D) format to file data.

	Additional planned features:
		* Compression level.

	Ideally, the rest is handled by other libraries. For example, loading a PNG file is done by loading the FileManager, and then feeding the data to the Png class.
	Likewise, saving a PNG is done by feeding the Png data to the FileManager.
	
	0253586805 - The new approach is the make things convenient by allowing strings and file operations in all libraries.
	
	For some reason saving with an ArrayS3 works, but loading and then using the PNG data to save to file is screwed up. (I think because alpha channel.)
	
	Regarding upside-down textures: OpenGL and LodePNG use different coordinate systems.
	
	LodePNG colourtypes
  LCT_GREY = 0, greyscale: 1,2,4,8,16 bit
  LCT_RGB = 2, RGB: 8,16 bit
  LCT_PALETTE = 3, palette: 1,2,4,8 bit
  LCT_GREY_ALPHA = 4, greyscale with alpha: 8,16 bit
  LCT_RGBA = 6 RGB with alpha: 8,16 bit
  
  Compression is currently disabled directly in the lodepng library.
  That's not the proper way to do it but I haven't yet figured out
  how to change encoding settings properly. Disabling compression
  significantly increases performance. It's probably not necessary
  to ever use compression anyway. Better to use a third party tool
  like optipng.
*/


#include <iostream>

#include <Container/ArrayS2/ArrayS2.hpp>
#include <Container/ArrayS3/ArrayS3.hpp>

#include <string>
#include <algorithm>
#include <tuple>
//#include <SFML/Graphics.hpp>

//#include <File/FileManager.hpp>

//#include <Data/DataTools.hpp>

//#include <File/CanLoadSave.hpp>

#include "lodepng.cpp"

class Png
{
	public:
	int nX;
	int nY;
	int nPixels;
		// Stored as RGBA (I think). Note that ArrayS3 is only RGB right now.
	unsigned char* data;
	int nData;
	bool useCompression;
	
		// Averages for faster rendering for small textures.
	unsigned char averageRed, averageGreen, averageBlue;
  
    //Flag to avoid deletion of PNG data array.
    // Useful because PNG will often pass off the data to other objects.
    // For example, loading textures.
    // This is a new feature and likely to break many things.
  bool preserveData;
	
	Png(bool _preserveData=false)
	{
		data=0;
		useCompression=false;
		
		averageRed = 0;
		averageGreen = 0;
		averageBlue = 0;
    
    preserveData = _preserveData;
	}
	
	~Png()
	{
    // Use a flag to prevent destructor deleting data.
    if ( preserveData == false )
    {
      delete [] data;
    }

	}
	
	void getAverageColour()
	{
		const double nValues = nX*nY;
		double _averageRed = 0;
		double _averageGreen = 0;
		double _averageBlue = 0;

		for (int _y=0;_y<nY;++_y)
		{
			for (int _x=0;_x<nX;++_x)
			{
				_averageRed += (getPixel3D(_x,_y,0)/nValues);
				_averageGreen += (getPixel3D(_x,_y,1)/nValues);
				_averageBlue += (getPixel3D(_x,_y,2)/nValues);
			}
		}
		
		averageRed = _averageRed;
		averageGreen = _averageGreen;
		averageBlue = _averageBlue;
	}
	
	void copy ( Png * png )
	{
		delete [] data;
		nX = png->nX;
		nY = png->nY;
		nPixels = png->nPixels;
		nData = png->nData;
		useCompression = png->useCompression;
		averageRed = png->averageRed;
		averageGreen = png->averageGreen;
		averageBlue = png->averageBlue;
		
		data = new unsigned char [nX*nY*4];
		
		for(int _y=0;_y<nY;++_y)
		{
			for(int _x=0;_x<nX;++_x)
			{
				setPixel3D(_x,_y,0, png->getPixel3D(_x,_y,0) );
				setPixel3D(_x,_y,1, png->getPixel3D(_x,_y,1) );
				setPixel3D(_x,_y,2, png->getPixel3D(_x,_y,2) );
				setPixel3D(_x,_y,3, png->getPixel3D(_x,_y,3) );

			}
		}
	}
	
		// Rotate the image 90 degrees clockwise.
	void rotate90Clockwise()
	{
		Png temp;
		temp.copy(this);
		
		for(int _y=0;_y<nY;++_y)
		{
			for(int _x=0;_x<nX;++_x)
			{
				setPixel3D(nY-_y-1,_x,0,temp.getPixel3D(_x,_y,0));
				setPixel3D(nY-_y-1,_x,1,temp.getPixel3D(_x,_y,1));
				setPixel3D(nY-_y-1,_x,2,temp.getPixel3D(_x,_y,2));
				setPixel3D(nY-_y-1,_x,3,temp.getPixel3D(_x,_y,3));
			}
		}
	}
	
	inline unsigned char getPixel3D(const int _x, const int _y, const int _z)
	{ //ALPHA
		return data[(_y*nX+_x)*4+_z]; 
	}
	
	inline void setPixel3D (const int _x, const int _y, const int _z, const unsigned char value)
	{	data[(_y*nX+_x)*4+_z] = value;
	}
	void setPixel ( const int _x, const int _y, std::tuple <unsigned char, unsigned char, unsigned char> tColour )
	{
		setPixel3D(_x,_y,0, std::get<0> (tColour));
		setPixel3D(_x,_y,1, std::get<1> (tColour));
		setPixel3D(_x,_y,2, std::get<2> (tColour));
	}
	
	std::tuple <unsigned char, unsigned char, unsigned char> getTuple (const int _x, const int _y)
	{
		return std::tuple <unsigned char, unsigned char, unsigned char> (getPixel3D(_x,_y,0),getPixel3D(_x,_y,1),getPixel3D(_x,_y,2));
	}
	
	bool load (unsigned char* data2, int nData2)
	{
		if(data2!=0)
		{
			LodePNG_Decoder decoder;
			LodePNG_Decoder_init(&decoder);
			//LodePNG_decode(&decoder, (unsigned char**)&data, (size_t*)&nPixels, (unsigned char*)data2, nData2);
			LodePNG_Decoder_decode(&decoder, (unsigned char**)&data, (size_t*)&nPixels, (unsigned char*)data2, nData2);
			
			nX=decoder.infoPng.width;
			nY=decoder.infoPng.height;
			
			if(data==0)
			{
				std::cout<<"Graphics/Png/Png.hpp Png::load(), unsuccessful load. LodePNG_Decoder_decode fail.\n";
				return false;
			}
			
			return true;
		}
		std::cout<<"Graphics/Png/Png.hpp Png::load(), data* was 0.\n";
		return false;
		
	}
	
	bool load (std::string data2)
	{
		unsigned char* bytes=new unsigned char[data2.size()+1]();
		std::copy(data2.begin(),data2.end(),bytes);
		load(bytes,data2.size());
		return true;
	}
	
		// Save it internally. Note that this one expects 8 bit RGBA (colourType 6, bitDepth 8), which is how PNG loads the data internally.
		// Doesn't seem to work in some cases, perhaps due to above condition. EncodeS3 is probably what should be used.
	bool saveToFile (std::string strFilePath)
	{
	
//unsigned LodePNG_encode(unsigned char** out, size_t* outsize, const unsigned char* image, unsigned w, unsigned h, unsigned colorType, unsigned bitDepth); /*return value is error*/
//unsigned LodePNG_encode_file(const char* filename, const unsigned char* image, unsigned w, unsigned h, unsigned colorType, unsigned bitDepth);
		LodePNG_encode_file(strFilePath.c_str(),data,nX,nY,6,8);
		return false;
	}
	
	static bool encodeS2 (std::string fileName, ArrayS2 <unsigned char> *mono)
	{
		LodePNG_encode_file(fileName.c_str(),mono->data,mono->nX,mono->nY,0,8);
		return false;
	}
	static bool encodeS3 (std::string fileName, ArrayS3 <unsigned char> *mono)
  {
		LodePNG_encode_file(fileName.c_str(),mono->data,mono->nX,mono->nY,2,8);
		return false;
	}
	
	unsigned char *save()
	{ return 0; }
	unsigned int getObjectSize()
	{ return 0; }
};
