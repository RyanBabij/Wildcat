//DO NOT USE ANYMORE
#define LIF_HPP

#ifndef LIF_HPP
#define LIF_HPP



#include <Graphics/Buffer.hpp>
#include <File/CanLoadSave.hpp>

class Lif: public Buffer, public CanLoadSave
{
	public:
	ArrayS3 <unsigned char> image;

	Lif()
	: Buffer(), CanLoadSave()
	{
		headerLength=27;
		header= new char [headerLength];
		header="ryan.graphics.2d.lif.00000\0";
	}
	void init(int x, int y, int x2, int y2)
	{
		Buffer::init(x,y,x2,y2);
	}
	void buffer()
	{
		pixel->fill(panelX,panelY,0,panelEndX,panelEndY,2,0);
		if(image.nZ==3)
		{
			pixel->mergeGraphics3(panelX,panelY,&image);
		}
		else if (image.nZ==4)
		{
			pixel->mergeGraphics4(panelX,panelY,&image);
		}
		
		bufferAll();
	}
	void directBuffer(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, ArrayS3 <unsigned char>* pixel)
	{
		//pixel->fill(x1,y1,0,x2,y2,2,0);
		if(image.nZ==3)
		{
			pixel->mergeGraphics3(x1,y1,&image);
		}
		else if (image.nZ==4)
		{
			pixel->mergeGraphics4(x1,y1,&image);
		}
	}
	void load(char * pointData)
	{
		std::cout<<"Loading data\n";
		delete [] saveData;
		saveData=pointData;
		std::cout<<"Data ops\n";
		unsigned char headerLength2;
		unpack(&headerLength2);
		if(headerLength2==headerLength)
		{
			char* header2 = new char [headerLength];
			unpack(header2,headerLength);
			bool headersMatch=true;
			
			for(unsigned int i=0;i<headerLength;++i)
			{
				if(header[i]!=header2[i]) { headersMatch=false; }
			}
			if(headersMatch)
			{
				char* subData=&pointData[currentPos];
				image.load(&pointData[currentPos]);
				currentPos=0;
			}
			else
			{
				std::cout<<"Headers do not match\n";
			}
			delete [] header2;
		}
		else
		{
			std::cout<<"Headers do not match\n";
		}
		std::cout<<"End of load\n";
	}
	char* save()
	{
		//Need to know how much memory to allocate (in bytes) (for now).
		delete[] saveData;
		currentPos=0;
		setObjectSize();
		pack(&headerLength);
		pack(header.c_str(),headerLength);
		pack(image.save(),image.getObjectSize());
		currentPos=0;
		return saveData;
	}
	unsigned int getObjectSize()
	{
		return(sizeof(char)+headerLength+image.getObjectSize());
	}
	void print()
	{
		image.print <int> ();
	}
};

#endif
