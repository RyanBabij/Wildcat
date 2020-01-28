#ifndef ARRAYSTATIC3D_HPP
#define ARRAYSTATIC3D_HPP

/* ArrayS3.hpp
	#include <Container/ArrayS3/ArrayS3.hpp>

	Static 3-dimensional array. Component of Wildcat engine.
	
	
	0271093718: Adding support for tuples.
	
	TODO: Add tuple support.
*/

#include <iostream>
#include <Data/DataTools.hpp>
#include <File/CanLoadSave.hpp>
#include <Math/BasicMath/BasicMath.hpp>
#include <limits> // TO DETERMINE IF A VALUE IS EQUAL TO INFINITY.

#include <Interface/HasXY.hpp>


template <class T>
class ArrayS3: public CanLoadSave
{
	private:
	public:
	T * data;
	T nullValue;
	T * nullAddress;
	T * currentElement;
	T outOfBounds;
	bool overFlow;
	unsigned int nX,nY,nZ;

		// FOR EACH MACRO HACK STUFF.
	#include <Container/ArrayS3/ArrayS3_ForEach.hpp>

	#include "ArrayS3_Geometry.hpp"

	ArrayS3 ()
	: CanLoadSave()
	{
		nX=0;
		nY=0;
		nZ=0;
		nullAddress=0;
		currentElement=0;
		overFlow=false;
		data=0;
	}
	ArrayS3 ( const int _nX, const int _nY, const int _nZ, const T _nullValue)
	: CanLoadSave ()
	{

		nX=0; nY=0; nZ=0;
		nullAddress=0;
		currentElement=0;
		overFlow=false;
		data=0;
		init(_nX,_nY,_nZ,_nullValue);

	}

		// NEW
		// ATTEMPT TO PORT RESIZE FUNCTION FROM ARRAYS2 TO ARRAYS3.
		// SEEMS TO WORK, BUT MAKES PRETTY MESSY THUMBNAILS.

	ArrayS3<T>* resizeTo(const int _x, const int _y, const int _z)
	/* can upsize and downsize. can squash aspect ratio */
	/* works by discarding pixels */
	{
		// number of old pixels for each new pixel: oldX/newX
		ArrayS3 <T> * resizedArray = new ArrayS3 <T>;

		const double pixelRatioX = (double)nX/_x;
		const double pixelRatioY = (double)nY/_y;
		const double pixelRatioZ = (double)nZ/_z;

		resizedArray->init(_x,_y,_z,0);

		for (unsigned int z=0;z<resizedArray->nZ;++z)
		{
		for(unsigned int y=0;y<resizedArray->nY;++y)
		{
			for(unsigned int x=0;x<resizedArray->nX;++x)
			{
				int targetX = x*pixelRatioX;
				int targetY = y*pixelRatioY;
				int targetZ = z*pixelRatioZ;

				(*resizedArray)(x,y,z)=(*this)(targetX,targetY,targetZ);
				//get all the required neighbor pixels.
				//Vector <T> neighbours;

				//for(int y2=y;y2<y+pixelRatioY;++y2)
				//{
				//	for(int x2=x;x2<x+pixelRatioX;++x2)
				//	{
						//neighbours.push(vIndex(x2,y2));

				//	}
				//}

			}
		}
		}

		return resizedArray;
	}



	bool isSafe(const unsigned int _x, const unsigned int _y, const unsigned int _z)
	{
		if(_x<nX && _y<nY && _z<nZ )
		{ return true; }
		return false;
	}

	void changeData(T* newData)
	{
		delete [] data;
		data=newData;
	}

	inline T *index(const unsigned int x, const unsigned int y, const unsigned int z)
	{ return &data[(y*nX+x)*nZ+z]; }
	inline T vIndex(const unsigned int x, const unsigned int y, const unsigned int z)
	{ return data[(y*nX+x)*nZ+z]; }
	ArrayS3 sub (unsigned int x1, unsigned int y1, unsigned int z1, unsigned int x2, unsigned int y2, unsigned int z2)
	{
		ArrayS3 <T> subArray;
		subArray.init(x2-x1+1,y2-y1+1,z2-z1+1,nullValue);

		for(unsigned int z=0;z<subArray.nZ;++z)
		{
			for(unsigned int y=0;y<subArray.nY;++y)
			{
				for(unsigned int x=0;x<subArray.nX;++x)
				{
					subArray(x,y,z) = *index(x1+x,y1+y,z1+z);
				}
			}
		}
		return subArray;
	}
	inline T& operator() (const unsigned int x, const unsigned int y, const unsigned int z)
	{
		if(x<nX && y<nY && z<nZ)
		{
			return data[(y*nX+x)*nZ+z];
		}
		else
		{
			outOfBounds=nullValue;
			overFlow=true;
			return outOfBounds;
		}
	}
	
		//Accepts a Vector and will fill each z with the corresponding tuple value. Tuple must have same size as nZ. Otherwise nothing will happen...
	inline void operator() (const unsigned int x, const unsigned int y, Vector <T> vValues)
	{
		if ((unsigned int) vValues.size() == nZ)
		{
			for (unsigned int _z=0;_z<nZ;++_z)
			{
				//(*this)(x,y,_z)=vValues(_z);
				ArrayS3::operator () (x,y,_z)=vValues(_z);
			}
		}
	}
		//Accepts an ArrayS2 and will fill each z with the single value at the ArrayS2 coordinate.
	// inline void operator() (const unsigned int x, const unsigned int y, ArrayS2 <T> *aValue)
	// {
		// if ( aValue.isSafe(x,y) )
		// {
			// for (unsigned int _z=0;_z<nZ;++_z)
			// {
				// //(*this)(x,y,_z)=vValues(_z);
				// ArrayS3::operator () (x,y,_z)=aValue(x,y);
			// }
		// }
		// // if ((unsigned int) vValues.size() == nZ)
		// // {
			// // for (unsigned int _z=0;_z<nZ;++_z)
			// // {
				// // //(*this)(x,y,_z)=vValues(_z);
				// // ArrayS3::operator () (x,y,_z)=vValues(_z);
			// // }
		// // }
	// }
	// {
		// // if(x<nX && y<nY && z<nZ)
		// // {
			// // return data[(y*nX+x)*nZ+z];
		// //}
		// // else
		// // {
			// // outOfBounds=nullValue;
			// // overFlow=true;
			// // return outOfBounds;
		// // }
	// }
	
	inline T& operator[] (unsigned int i)
	{ return data[i]; }
	void fill(T value)
	{
		currentElement=&data[0];
		while(currentElement!=nullAddress)
		{
			*currentElement=value;
			++currentElement;
		}
		currentElement=&data[0];
	}
	
		// Fill all z values at a particular x,y coordinate with a single value.
	void fillZ (const unsigned int _x, const unsigned int _y, T value)
	{
		for (unsigned int _z=0;_z<nZ;++_z)
		{
			ArrayS3::operator () (_x,_y,_z)=value;
		}
	}
   
   void fillLayer(const unsigned int _z, const T _value)
   {
      for (unsigned int _y=0;_y<nY;++_y)
      {
         for (unsigned int _x=0;_x<nX;++_x)
         {
            ArrayS3::operator () (_x,_y,_z)=_value;
         }
      }
   }
	
	
	void init(unsigned int x, unsigned int y, unsigned int z, T _nullValue)
	{
		delete[] data;
		nullValue=_nullValue;
		nX=x;
		nY=y;
		nZ=z;
		data = new T [x*y*z];
		nullAddress=&data[x*y*z];
		currentElement=&data[0];
		fill(nullValue);
		outOfBounds=nullValue;
		overFlow=false;
	}
	void initCopyFill(unsigned int _x, unsigned int _y, unsigned int _z, T _nullValue)
	{
		delete[] data;
		nullValue=_nullValue;
		nX=_x;
		nY=_y;
		nZ=_z;
		data= new T [nX*nY*nZ];
		for(unsigned int i=0;i<nX*nY*nZ;++i)
		{
			T* someT = new T;
			data[i]= *someT;
		}
		nullAddress=&data[nX*nY*nZ];
		currentElement=&data[0];
		outOfBounds=nullValue;
		overFlow=false;
	}
	
		// Save some cycles for important code. This should be initialised manually.
	void initNoFill(const unsigned int _x, const unsigned int _y, const unsigned int _z, T _nullValue)
	{
		delete[] data;
		nullValue=_nullValue;
		nX=_x;
		nY=_y;
		nZ=_z;
		data = new T [_x*_y*_z];
		nullAddress=&data[_x*_y*_z];
		currentElement=&data[0];
		//fill(nullValue);
		outOfBounds=_nullValue;
		overFlow=false;
	}


	bool load(unsigned char __attribute__ ((unused)) * pointData)
	{
	/*
		delete [] saveData;
		saveData=pointData;

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
				unpack(&nX);
				unpack(&nY);
				unpack(&nZ);
				unpack(&nullValue);
				data=new T[nX*nY*nZ];
				unpack(data,nX*nY*nZ);

				nullAddress=&data[nX*nY*nZ];
				currentElement=&data[0];
				outOfBounds=nullValue;
				overFlow=false;
				currentPos=0;
			}
			else { std::cout<<"Headers do not match\n"; }
			delete [] header2;
		}
		else { std::cout<<"Header lengths do not match\n"; }
	*/
		return false;
	}

	unsigned char* save()
	{
	/*
		//Need to know how much memory to allocate (in bytes) (for now).
		currentPos=0;
		setObjectSize();
		pack(&headerLength);
		pack(header.c_str(),headerLength);
		pack(&nX);
		pack(&nY);
		pack(&nZ);
		pack(&nullValue);
		pack(data,nX*nY*nZ);
		currentPos=0;
		return saveData;
		*/
		return 0;
	}

	unsigned int getObjectSize()
	{
		return 0;
		//return((nX*nY*nZ)*sizeof(T)+headerLength+sizeof(unsigned int)*3+1+sizeof(T));
	}

	template <class T2>
	void print(std::string delim="")
	{
		for(unsigned int z=0;z<nZ;z++) {
			for(unsigned int y=0;y<nY;y++) {
				for(unsigned int x=0;x<nX;x++)
				{
					std::cout<<(T2)vIndex(x,y,z)<<delim;
				}
				std::cout<<"\n";
			}
			std::cout<<"\n";
		}
	}

	inline void merge(unsigned int x1, unsigned int y1, unsigned int z1, unsigned int x2, unsigned int y2, unsigned int z2, unsigned int x3, unsigned int y3, unsigned int z3, ArrayS3 <T> * array)
	{
		//Merge includes all given values.
		// The first 2 sets of coords are the source coords. The final set of coords is the target point of entry.
		unsigned int x4=x3;
		unsigned int y4=y3;
		unsigned int z4=z3;
		for(unsigned int z=z1;z<=z2;++z) {
			for(unsigned int y=y1;y<=y2;++y) {
				for(unsigned int x=x1;x<=x2;++x)
				{
					data[(y4*nX+x4)*nZ+z4]=array->data[(y*array->nX+x)*array->nZ+z];
					++x4;
				}
				++y4;
				x4=x3;
			}
			++z4;
			y4=y3;
		}
	}
	//Oops... we have alpha channel problems... Must fix this.
	inline void mergeGraphics(unsigned int x3, unsigned int y3, ArrayS3 <T> * array)
	{
		unsigned int x2=x3;
		unsigned int y2=y3;
		for(unsigned int y=0;y<array->nY;++y)
		{
			for(unsigned int x=0;x<array->nX;++x)
			{
				if(array->data[(y*array->nX+x)*array->nZ+3]!=0)
				{
					data[(y2*nX+x2)*nZ+0]=array->data[(y*array->nX+x)*array->nZ+0];
					data[(y2*nX+x2)*nZ+1]=array->data[(y*array->nX+x)*array->nZ+1];
					data[(y2*nX+x2)*nZ+2]=array->data[(y*array->nX+x)*array->nZ+2];
					//This was probably supposed to be an alpha channel.
					//data[(y2*nX+x2)*nZ+3]=array->data[(y*array->nX+x)*array->nZ+3];
				}
				++x2;
			}
			++y2;
			x2=x3;
		}
	}
	inline void mergeGraphics3(unsigned int x3, unsigned int y3, ArrayS3 <T> * array)
	{
		unsigned int x2=x3;
		unsigned int y2=y3;
		for(unsigned int y=0;y<array->nY;++y)
		{
			for(unsigned int x=0;x<array->nX;++x)
			{
				data[(y2*nX+x2)*nZ+0]=array->data[(y*array->nX+x)*array->nZ+0];
				data[(y2*nX+x2)*nZ+1]=array->data[(y*array->nX+x)*array->nZ+1];
				data[(y2*nX+x2)*nZ+2]=array->data[(y*array->nX+x)*array->nZ+2];
				++x2;
			}
			++y2;
			x2=x3;
		}
	}
	inline void mergeGraphics4(unsigned int x3, unsigned int y3, ArrayS3 <T> * array)
	{
		unsigned int x2=x3;
		unsigned int y2=y3;
		for(unsigned int y=0;y<array->nY;++y)
		{
			for(unsigned int x=0;x<array->nX;++x)
			{
				if(array->data[(y*array->nX+x)*array->nZ+3]==0)
				{
					data[(y2*nX+x2)*nZ+0]=array->data[(y*array->nX+x)*array->nZ+0];
					data[(y2*nX+x2)*nZ+1]=array->data[(y*array->nX+x)*array->nZ+1];
					data[(y2*nX+x2)*nZ+2]=array->data[(y*array->nX+x)*array->nZ+2];
				}
				++x2;
			}
			++y2;
			x2=x3;
		}
	}
// Deep copy
	~ArrayS3()
	{
		delete[] data;
	}
	ArrayS3 (const ArrayS3 <T> &array)
	{
		nX=array.nX;
		nY=array.nY;
		nZ=array.nZ;

		int memSize=array.nullAddress-array.data;
		data=new T[memSize];
		nullAddress=data+memSize;
		nullValue=array.nullValue;

		T * i = data;
		T * j = array.data;
		while(i!=nullAddress)
		{
			*i=*j;
			i++;
			j++;
		}
	}
	void operator = (const ArrayS3 &array)
	{
		nX=array.nX;
		nY=array.nY;
		nZ=array.nZ;

		int memSize=array.nullAddress-array.data;
		data=new T[memSize];
		nullAddress=data+memSize;
		nullValue=array.nullValue;

		T * i = data;
		T * j = array.data;
		while(i!=nullAddress)
		{
			*i=*j;
			i++;
			j++;
		}
	}
	/*
	 * Deprecated
	 */
	void fill(unsigned int x1, unsigned int y1, unsigned int z1, unsigned int x2, unsigned int y2, unsigned int z2, T value)
	{
		//TODO: Optimise fill routine.
		for(unsigned int z=z1;z<=z2;z++){
			for(unsigned int y=y1;y<=y2;y++){
				for(unsigned int x=x1;x<=x2;x++)
				{
					if(x<nX&&y<nY&&z<nZ)
					{
						*index(x,y,z)=value;
					}
		}	}	}
	}

	void border2D(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int z1, T fillValue)
	{
		for(unsigned int xx=x1;xx<x2;++xx)
		{
			*index(xx,y1,z1)=fillValue;
			*index(xx,y2,z1)=fillValue;
		}
		for(unsigned int yy=y1;yy<y2;++yy)
		{
			*index(x1,yy,z1)=fillValue;
			*index(x2,yy,z1)=fillValue;
		}
	}

	void loadStream(T * _data, T * _nullAddress)
	{
	/*
		data=DataTools::copy(data,nullAddress);
		nullAddress=&data[nullAddress-data];
	*/
	}

	void loadXMajor(T * _data)
	{
		for(unsigned int z=0;z<nZ;z++)
		{
			for(unsigned int y=0;y<nY;y++)
			{
				for(unsigned int x=0;x<nX;x++)
				{
					*index(x,y,z)=_data[(z*nY+y)*nX+x];
				}
			}
		}
	}
	void flipHorizontal()
	{
		for(unsigned int z=0;z<nZ;z++)
		{
			for(unsigned int y=nY/2;y<nY;y++)
			{
				for(unsigned int x=0;x<nX;x++)
				{
					T temp = *index(x,y,z);
					*index(x,y,z)=*index(x,nY-1-y,z);
					*index(x,nY-1-y,z)=temp;
				}
			}
		}
	}

	void resize( unsigned int x, unsigned int y, unsigned int z)
	{
		T* newData = new T [x*y*z];

		for(unsigned int i=0;i<x*y*z;++i)
		{
			newData[i]=0;
		}

		for(unsigned int z2=0;z2<z;++z2) {
			for(unsigned int y2=0;y2<y;++y2) {
				for(unsigned int x2=0;x2<x;++x2) {
					if(z2<nZ && y2<nY && x2<nX)
					{
						newData[(y2*x+x2)*z+z2]=*index(x2,y2,z2);
					}
					else
					{
						newData[(y2*x+x2)*z+z2]=nullValue;
					}
				}
			}
		}

		nX=x;
		nY=y;
		nZ=z;

		nullAddress=&newData[x*y*z];
		currentElement=&newData[0];

		delete [] data;
		data = newData;
	}
/*
	void loadStream(T *data)
	{
		for(unsigned int i=0;i<size;i++)
		{
			array[i]=data[i];
		}
	}
	void merge(unsigned int x, unsigned int y, unsigned int z, DynamicArray <T> d)
	{
		for(unsigned int z2=0;z2<d.numZ;z2++)
		{
			for(unsigned int y2=0;y2<d.numY;y2++)
			{
				for(unsigned int x2=0;x2<d.numX;x2++)
				{
					this->write(x+x2,y+y2,z+z2,d.read(x2,y2,z2));
				}
			}
		}
	}

	void insertStream(unsigned int from, unsigned int to, T* stream)
	{
		for(unsigned int i=from;i<size;i++)
		{
			if(i>from)
			{
				break;
			}
			//if stream is done
			//break;
			array[i]=stream[i];
		}
	}

	//Fill a line of data between two points. (More complicated than it sounds).
	void fillLine2D(unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int z, T value)
	{
		double xStep=(double)x2-x1;
		double yStep=(double)y2-y1;
		double posXStep=xStep;
		double posYStep=yStep;
		if(posXStep<0)
		{
			posXStep=-posXStep;
		}
		if(posYStep<0)
		{
			posYStep=-posYStep;
		}

		if(posXStep>=posYStep)
		{
			//X focused.
			if(xStep!=0)
			{
				double slope= yStep/xStep;
				if(yStep==0&&xStep>0)
				{
					for(double x=x1;x<x2;x++)
					{
						write(x,y2,z,value);
					}
				}
				else if(yStep==0&&xStep<0)
				{
					for(double x=x2;x<x1;x++)
					{
						write(x,y2,z,value);
					}
				}
				else if(xStep>0)
				{
						double y=y1;
						for(double x=x1;x<x2;x++)
						{
							write((unsigned int)x,(unsigned int)y,z,value);
							y+=slope;
						}
				}
				else if(xStep<0)
				{
						double y=y2;
						for(double x=x2;x<x1;x++)
						{
							write((unsigned int)x,(unsigned int)y,z,value);
							y+=slope;
						}
				}
			}
			else if(xStep==0&&yStep>0)
			{
				for(double y=y1;y<y2;y++)
				{
					write(x2,y,z,value);
				}
			}
			else if(xStep==0&&yStep<0)
			{
				for(double y=y2;y<y1;y++)
				{
					write(x2,y,z,value);
				}
			}
		}
		else
		{
			//Y focused.
			if(yStep!=0)
			{
				double slope= xStep/yStep;
				if(xStep==0&&yStep>0)
				{
					for(double y=y1;y<y2;y++)
					{
						write(x2,y,z,value);
					}
				}
				else if(xStep==0&&yStep<0)
				{
					for(double y=y2;y<y1;y++)
					{
						write(x2,y,z,value);
					}
				}
				else if(yStep>0)
				{
						double x=x1;
						for(double y=y1;y<y2;y++)
						{
							write((unsigned int)x,(unsigned int)y,z,value);
							x+=slope;
						}
				}
				else if(yStep<0)
				{
						double x=x2;
						for(double y=y2;y<y1;y++)
						{
							write((unsigned int)x,(unsigned int)y,z,value);
							x+=slope;
						}
				}
			}
		}
	}
	*/
};
#endif
