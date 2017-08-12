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
	
	Png()
	{
		data=0;
		useCompression=false;
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
	//unsigned char getPixel2D(const int _x, const int _y)
	//{
	//	return data[nX*_y+_x]; 
	//}
	
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
			std::cout<<"PNG dimensions are: "<<nX<<", "<<nY<<".\n";
			
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
		std::cout<<"Savetofile: "<<strFilePath<<"\n";
	
//unsigned LodePNG_encode(unsigned char** out, size_t* outsize, const unsigned char* image, unsigned w, unsigned h, unsigned colorType, unsigned bitDepth); /*return value is error*/
//unsigned LodePNG_encode_file(const char* filename, const unsigned char* image, unsigned w, unsigned h, unsigned colorType, unsigned bitDepth);
		LodePNG_encode_file(strFilePath.c_str(),data,nX,nY,6,8);
		std::cout<<"done\n";
		return false;
	}
	
	static bool encodeS2 (std::string fileName, ArrayS2 <unsigned char> *mono)
	{
		LodePNG_encode_file(fileName.c_str(),mono->data,mono->nX,mono->nY,0,8);
		return false;
	}
	static bool encodeS3 (std::string fileName, ArrayS3 <unsigned char> *mono)
	{
		//I'M USING THE BASIC FUNCTION WHICH DIRECTLY ENCODES THE DATA INTO A FILE. I HAVEN'T BEEN ABLE TO FIGURE OUT HOW TO GET THE ENCODER OBJECT WORKING, BUT I ONLY NEED IT TO TWEAK COMPRESSION SETTINGS ANYWAY. I JUST DISABLED COMPRESSION AND USE THE BASIC FUNCTION. I USE OPTIPNG TO COMPRESS PNGS WHEN I NEED TO. DISABLING THE COMPRESSION MAKES THE ENCODING MUCH FASTER, AND IT STILL USES THE PNG FORMAT SO IT CAN BE COMPRESSED EXTERNALLY AND STILL WORK IN MY GAMES.
	
 // unsigned useLZ77; /*whether or not to use LZ77. Should be 1 for proper compression.*/
  //unsigned windowSize; /*the maximum is 32768, higher gives more compression but is slower. Typical value: 2048.*/
	//LodePNG::LodeZlib_CompressSettings.useLZ77=0;
	//LodeZlib_CompressSettings settings;
	//settings.useLZ77=0;
	//LodeZlib_CompressSettings_init(&settings);
	
		//LodeZlib_CompressSettings_init(0,0,2048);
		//LodeZlib_CompressSettings pngsettings(0,0,2048);
		// LodeZlib_CompressSettings pngSettings;
		// pngSettings.btype = 2;
		// pngSettings.useLZ77 = 1;
		// pngSettings.windowSize = 2048;
		
		// LodeZlib_CompressSettings_init(&pngSettings);
		
		// LodePNG::Encoder encoder;
		// //LodePNG_Encoder_init(&encoder);
		// //LodePNG_EncodeSettings encodeSettings;
		// //encodeSettings.zlibsettings.btype=2;
		// ///encodeSettings.zlibsettings.useLZ77=1;
		// //encodeSettings.zlibsettings.windowSize=2048;
		// //encoder.setSettings(encodeSettings);
		
		// std::vector <unsigned char> buffer;
		// size_t buffersize;
		
		// //encoder.encode(buffer, mono->data,mono->nX,mono->nY);
		// encoder.encode(buffer, mono->data, mono->nX,mono->nY);
		// LodePNG::saveFile(buffer, "test.png");
		
		//unsigned error = LodePNG_encode(&buffer, &buffersize, mono->data,mono->nX,mono->nY,2,8);
		//if(!error) error = LodePNG_saveFile(buffer, buffersize, fileName.c_str());
 // return error;
		
		//
// unsigned LodePNG_encode(unsigned char** out, size_t* outsize, const unsigned char* image, unsigned w, unsigned h, unsigned colorType, unsigned bitDepth)
// {
  // unsigned error;
  // LodePNG_Encoder encoder;
  // LodePNG_Encoder_init(&encoder);
  // encoder.infoRaw.color.colorType = colorType;
  // encoder.infoRaw.color.bitDepth = bitDepth;
  // encoder.infoPng.color.colorType = colorType;
  // encoder.infoPng.color.bitDepth = bitDepth;
  // LodePNG_Encoder_encode(&encoder, out, outsize, image, w, h);
  // error = encoder.error;
  // LodePNG_Encoder_cleanup(&encoder);
  // return error;
// }
	
		//LodePNG_encode_file(fileName.c_str(),mono->data,mono->nX,mono->nY,2,8);
		std::cout<<"Encoding file: "<<fileName<<".\n";
		
		//std::vector<unsigned char> buffer;
		//encoder.encode(buffer, 0, mono->nX,mono->nY);
		//LodePNG::saveFile(buffer, "test");
		
		LodePNG_encode_file(fileName.c_str(),mono->data,mono->nX,mono->nY,2,8);
		return false;
	}
	
	unsigned char *save()
	{ return 0; }
	unsigned int getObjectSize()
	{ return 0; }
};
