#pragma once

/*
	DataTools
	#include <Data/DataTools.hpp>

	Library for miscellaneous functions involving datatypes.

	Library for dealing with the main datatypes: int, char, double, etc.
	String will be a seperate library (probably should consider a wrapper).
*/

#include <sstream>
#include <string>
#include <iostream>
#include <iomanip> // setprecision

#include <stdlib.h> /* atoi */

//#include <stdio.h>

#include <Container/Vector/Vector.hpp>

#include <string.h> // memcpy (need a better way of doing this.)


namespace DataTools
{
  // Break string up into a vector of strings split by the passed delimiters.
  // Don't forget to include \n and \r.
  //  tokenize("test,1,2,,,3,", ",") will return "test", "1", "2", and "3".
  Vector <std::string> * tokenize(const std::string _str, const std::string delim)
  {
    if ( _str.length() == 0 ) { return 0; }
    auto result = new Vector <std::string>;
    std::string temp = "";
    
    for (unsigned int i=0;i<_str.size();++i)
    {
      
      if (delim.find(_str[i]) != std::string::npos)
      {
        if ( temp.size() >  0 )
        {
          result->push(temp);
        }
        temp="";
      }
      else
      { temp+=_str[i];
      }
    }
    if (temp.size() > 0 )
    { result->push(temp); }
    
    if ( result->size() == 0 )
    {
      delete result;
      return 0;
    }
    
    return result;
  }
  
	
	bool isAlphaNumeric(std::string _s, const bool allowSpaces=false )
	{
		for ( auto c : _s )
		{
			if ( c=='0'||c=='1'||c=='2'||c=='3'||c=='4'||c=='5'||c=='6'||c=='7'||c=='8'||c=='9'
			|| c=='a' || c=='b' || c=='c' || c=='d' || c=='e' || c=='f' || c=='g' || c=='h' || c=='i'
			|| c=='j' || c=='k' || c=='l' || c=='m' || c=='n' || c=='o' || c=='p' || c=='q' || c=='r'
			|| c=='s' || c=='t' || c=='u' || c=='v' || c=='w' || c=='x' || c=='y' || c=='z'
			|| c=='A' || c=='B' || c=='C' || c=='D' || c=='E' || c=='F' || c=='G' || c=='H' || c=='I'
			|| c=='J' || c=='K' || c=='L' || c=='M' || c=='N' || c=='O' || c=='P' || c=='Q' || c=='R'
			|| c=='S' || c=='T' || c=='U' || c=='V' || c=='W' || c=='X' || c=='Y' || c=='Z'
			|| (allowSpaces && c==' ')
			)
			{
			}
			else
			{
				return false;
			}
		}
		return true;
	}

		// MODIFIES X AND Y COORDINATES BASED ON NUMERICAL INPUT, IE THE NUMPAD.
	void numpadToCoords(int* x, int* y, const int numpadDirection)
	{
		switch (numpadDirection)
		{
			case 1:
				--(*x);
				--(*y);
				break;
			case 2:
				--(*y);
				break;
			case 3:
				++(*x);
				--(*y);
				break;
			case 4:
				--(*x);
				break;
			case 5:
				++(*x);
				--(*y);
				break;
			case 6:
				++(*x);
				break;
			case 7:
				--(*x);
				++(*y);
				break;
			case 8:
				++(*y);
				break;
			case 9:
				++(*x);
				++(*y);
				break;
		}
	}
		// RETURNS TRUE IS THE CHAR IS A NUMBER.
	bool isNumeric ( const char c)
	{
		return (c=='0'||c=='1'||c=='2'||c=='3'||c=='4'||c=='5'||c=='6'||c=='7'||c=='8'||c=='9'); 
	}
		bool isAsciiNumber (const char c)
		{ return isNumeric(c); }
		
	bool isNumeric (std::string _s)
	{
		for ( auto c : _s )
		{
			if ( c=='0'||c=='1'||c=='2'||c=='3'||c=='4'||c=='5'||c=='6'||c=='7'||c=='8'||c=='9' )
			{
			}
			else
			{
				return false;
			}
		}
		
	// std::string::const_iterator it = s.begin();
		// while (it != s.end() && std::isdigit(*it)) ++it;
		// return !s.empty() && it == s.end();
		return true;
	}


	// Swap 2 variables around. 0221152584
	template <class T>
	void swap(T * v1, T * v2)
	{
		const T temp = *v1;
		*v1 = *v2;
		*v2 = temp;
	}

	/* Returns array with the same size as the input array, with a moving average. */
	template <class T>
	T *  movingAverage(T* inArray, const int arraySize, const int radius)
	{


		return 0;
	}

		// SANITY CHECK TO SEE IF DATATYPE SIZES ARE STANDARD. THERE ISN'T MUCH PURPOSE TO THIS, SINCE WE SHOULD EXPECT OUR PROGRAM TO RUN ANYWAY. HOWEVER, IT COULD BE A USEFUL SANITY CHECK WHEN PROVIDING TECH SUPPORT OVER MANY SYSTEMS.
	bool datatypeSizesOkay()
	{
		if(sizeof(char)>=1&&sizeof(int)>=4&&sizeof(double)>=8&&sizeof(float)>=4)
		{ return true; }
		return false;
	}

	template <class T>
	char * toCharArray(T* var)
	{
		/* Convert the data into a stream of bytes for saving. This is a simple implementation that will probably break on some systems. May have issues with endian-ness, different sizeof()s, etc... */
		return (char*)var;
	}


	/* Get X number of bytes from a char (byte) array and read them into the datatype. If the datatype has more space, the empty space is filled with zeros. If the datatype doesn't have enough space, then the last bits will be left out. */
	template <class T>
	T getBytesValue (char* data, unsigned int numBytes)
	{
		// if((int)sizeof(T)<numBytes)
		// {
			// std::cout<<"Data/DataTools.hpp::T getBytesValue(char* data, int numBytes): Warning: Target datatype is too small.\n";
		// }
		T returnValue=0;
		//returnValue&=0x0000;
		char* currentBit = (char*)&returnValue;

		for(unsigned int i=0;i<numBytes && i<sizeof(T);++i)
		{
			//std::cout<<"bit written.\n";
			*currentBit&=0x00;
			*currentBit|=data[i];
			++currentBit;
		}


		//*currentBit|=0x00;
		//*currentBit|=data[0];
		//++currentBit;
		//*currentBit|=data[1];

		//short int vv = 9899;
		//returnValue&=0;
		//returnValue|=0x0f0f;
		//returnValue|=data[0];
		//returnValue>>=8;
		//returnValue&=0;
		//returnValue|=data[1];
		//returnValue>>=8;


		//returnValue<<=20;

		//char* tArray = &returnValue;

		//tArray[0]|=data[0];
		//tArray[1]|=data[1];

		//--numBytes;
		//char data0 = data[0];
		//returnValue=returnValue|data0;
		//returnValue|=data[1];
		//returnValue|=data[0];

		//for (int i=0;i<numBytes;++i)
		// for(;numBytes>0;--numBytes)
		// {
			// returnValue<<=8;
			// if(numBytes!=0)
			// {
				// char v = data[numBytes-1];
				// //char v = 8;
				// returnValue|=0;
				// returnValue|=v;
				// //returnValue|=8;
				// std::cout<<"Char val was: "<<(int)v<<".\n";
			// //return returnValue;
			// }
			// else
			// {

// NOTE FOR LATER PLEASE READ THIS DUMB SHIT. WHY THE FUCK DID YOU | WITH A ZERO HERE? WHY? IT DOESNT WIPE THE GODDAMN VARIABLE FUCKTARD. YOU NEED TO & IT.

				// //char n = 0;
				// returnValue|=0;
			// }

			// //returnValue>>=8;


			// //--numBytes;

		// }


		return returnValue;
	}

	/* Outputs the actual binary representation of the given value. Doesn't work on floating point values (ie float, double). */
	template <class T>
	std::string getBinaryString (T value, const std::string delimiter="")
	{
		std::string r = "";

		int nBytes = sizeof(T);

		/* C++ doesn't support binary values apparently, so I'm using hex here. */
		while(nBytes>0)
		{
				if((value&0x80) == 0x80)
				{ r+="1"; }
				else
				{ r+="0"; }
				if((value&0x40) == 0x40)
				{ r+="1"; }
				else
				{ r+="0"; }
				if((value&0x20) == 0x20)
				{ r+="1"; }
				else
				{ r+="0"; }
				if((value&0x10) == 0x10)
				{ r+="1"; }
				else
				{ r+="0"; }
				if((value&0x8) == 0x8)
				{ r+="1"; }
				else
				{ r+="0"; }
				if((value&0x4) == 0x4)
				{ r+="1"; }
				else
				{ r+="0"; }
				if((value&0x2) == 0x2)
				{ r+="1"; }
				else
				{ r+="0"; }
				if((value&0x1) == 0x1)
				{ r+="1"; }
				else
				{ r+="0"; }
				r+=delimiter;
			value>>=8;
			--nBytes;
		}

		return r;
	}


	// template <class T>
	// char * toCharArray(T* var)
	// {
	// /* Convert the data into a stream of bytes for saving */

		// char * data = new char [sizeof(T)];

		// for (unsigned int i=0;i<sizeof(T);++i)
		// {
			// data[i] = *var >> (8*i);
		// }

		// //char *data = (char*)number;
		// return data;
	// }

	template <class T2>
	void fromCharArray(T2** var, char* firstByte)
	{
		/* Convert a stream of bytes into a datatype. This is a simple implementation that will probably break on some systems. May have issues with endian-ness, different sizeof()s, etc... */
		*var = (T2*)firstByte;
	}
	template <class T3>
	T3 valFromCharArray(char* firstByte)
	{
		/* Similar to above, but returns value rather than pointer. Usage: double d = fromCharArray2 <double> (bytes) */
		return *(T3*)firstByte;
	}
	// template <class T3>
	// T3 valFromCharArray(char* firstByte)
	// {
		// /* Similar to above, but returns value rather than pointer. Usage: double d = fromCharArray2 <double> (bytes) */
		// return *(T3*)firstByte;
	// }
	template <class T4>
	T4 valFromVector(Vector <char>* vByte, const int startIndex)
	{
		char* arrayByte = new char [sizeof(T4)];

		/* Copy bytes from memory to array. */
		const int dataSize = sizeof(T4);
		for (int i=0;i<dataSize;++i)
		{
			if(vByte->size()>i+startIndex)
			{
				arrayByte[i]=(*vByte)(i+startIndex);
			}
			else { std::cout<<"Error: DataTools::valFromVector is out of vector bounds.\n"; arrayByte[i]=0; }
		}

		return *(T4*)arrayByte;
	}


	#ifdef DATATOOLS_OLD

	template <class T>
	char * toCharArray(T* number)
	{
	/* Convert the data into a stream of bytes for saving */
		char *data = (char*)number;
		return data;
	}

	template <class T>
	void fromCharArray(T* var, char* firstByte)
	{
		/* Convert stream of bytes into a datatype. This may have issues with endian-ness, bit-shifting, different sizeof()s... */


	//unsigned char tempArr[4]={0x00, 0x00, 0x02, 0x07}; //00000000, 00000000, 00000010, 00000111};
	//int i=0, arrToInt=0;
		const unsigned int dataTypeSize=sizeof(T);
		//for(unsigned int i=0;i<dataTypeSize;++i)
		for(int i=dataTypeSize-1;i>=0;--i)
		{
			//std::cout<<"Int: "<<(int)firstByte[i]<<".\n";
			//*var =(*var<<8) | *firstByte;
			//*var=(*var<<8) | firstByte[i];
			*var = (*var<<8);
			*var = (*var | firstByte[i]);
			//*var=((*var<<8) | firstByte[i]);
			//++firstByte;
		}

		//*var = firstByte;
	}

	#endif


	template <class T>
	static unsigned int find(T * data, T value, unsigned int startPos=0)
	{
		while(data[startPos++]!=value){}
		return startPos;
	}
	template <class T>
	T * deepCopy(T * from, T * to)
	{
		int size=(to-from);
		T * final = new T [size];
		for(int i=0;i<size;i++)
		{
			final[i]=*from;
			from++;
		}
		return final;
	}
	template <class T>
	void print(T * from, T * to, std::string delimiter=", ")
	{
		while(from!=to)
		{
			std::cout<<*from<<delimiter;
			from++;
		}
		std::cout<<"\n";
	}
	template <class T>
	static std::string toString(const T n)
	{
		std::ostringstream oss;
		oss<<n;
		return oss.str();
	}
	/* Turns a variable into a string, and also pads with zeroes. */
	template <class T>
	static std::string toString (const T n, unsigned int nZeroes)
	{
		std::ostringstream oss;
		oss<<n;
		std::string val = oss.str();

		std::string retVal = "";

		if(val.length()<nZeroes)
		{
			int requiredZeroes = nZeroes - val.length();
			while(requiredZeroes>0)
			{
				retVal+="0";
				--requiredZeroes;
			}
		}
		retVal+=val;
		return retVal;
	}

	std::string reverseString(const std::string _inStr)
	{ return std::string (_inStr.rbegin(),_inStr.rend()); }

		//0231710017: NOW DELIMITS NEGATIVE VALUES PROPERLY.
	std::string delimitThousands(const std::string _str, const char _delimiter)
	{

		//std::cout<<"Delimitthousands.\n";

		if ( _str.size() == 0 )
		{ return ""; }



		std::string dString="";
			// NEGATIVE VALUES MUST BE HANDLED DIFFERENTLY BECAUSE OF THE '-' IN THEM.
		if ( _str.at(0) == '-' )
		{
			std::string rString = reverseString(_str);
			int delimPos=1;
			for (unsigned int i=0;i<rString.size();++i)
			{
				dString+=rString[i];
				if(delimPos==3 && i<rString.size()-2) { dString+=_delimiter; delimPos=0; }
				++delimPos;
			}
		}
		else
		{
			std::string rString = reverseString(_str);
			int delimPos=1;
			for (unsigned int i=0;i<rString.size();++i)
			{
				dString+=rString[i];
				if(delimPos==3 && i<rString.size()-1) { dString+=_delimiter; delimPos=0; }
				++delimPos;
			}
		}


		//dString=reverseString(dString);


		return reverseString(dString);

		// std::string outString="";
		// for (unsigned int i=0; i<_str.size();++i)
		// {
			// char c = _str[i];
			// std::cout<<c<<",";
			// outString+=c;
			// //outString.append((const char*)c);
			// //outString.append(_str[i]);
			// //_str(i)="l";
		// } std::cout<<"\n";
		// return outString;
	}

	int toInt(std::string s)
	{
		/* NOTE: If the string is not a valid integer, zero will be returned. */
    return stoi(s);
	}
	short int toShort(std::string s)
	{
		/* NOTE: If the string is not a valid integer, zero will be returned. */
    return stoi(s);
	}
  long int toLong(std::string s)
  {
    return stol(s);
  }
  
	int toUnsignedInt(std::string s)
	{
		/* NOTE: If the string is not a valid integer, zero will be returned. */
    return stoul(s);
	}
	short int toUnsignedShort(std::string s)
	{
		/* NOTE: If the string is not a valid integer, zero will be returned. */
    return stoul(s);
	}
  long int toUnsignedLong(std::string s)
  {
    return stoul(s);
  }
  
  
  
	template <class T>
	T fromArray(char* address, int numBytes)
	{
		T r;
		return r;
	}


	template <class T>
	void mergeIntoArray(T* number, unsigned char** startAddress)
	/* Will convert the T into a char array, and then merge it into the given array address */
	/* Also, it will position the supplied address pointer to the end of the merge */
	/* Update: Changed from char to unsigned char... Should fix things a little */
	/* Update: Now I'm using memcpy, and things seem to be working better. */
	{
		//unsigned char *data = (unsigned char*)number;
		//for(unsigned int i=0;i<sizeof(T);++i)
		//{
		//	**startAddress=data[i];
		//	++*startAddress;
		//}
		memcpy(*startAddress,number,sizeof(T));
		*startAddress+=sizeof(T);
	}
	template <class T>
	void mergeOutOfArray(T* number, unsigned char** startAddress)
	{
		//*number=(T)**startAddress;
		//*startAddress+=(sizeof(T));

		memcpy(number,*startAddress,sizeof(T));
		*startAddress+=sizeof(T);
	}
  
  unsigned long int absoluteDifference( unsigned long int val1, unsigned long int val2)
  {
    if ( val1 == val2 ) { return 0; }
    
    if (val1 > val2)
    {
      return val1-val2;
    }
    return val2-val1;
  }


  unsigned long int manhattanDistance (unsigned long int x1, unsigned long int y1, unsigned long int x2, unsigned long int y2)
  {
    return DataTools::absoluteDifference(x1,x2)+DataTools::absoluteDifference(y1,y2);
  }
  
  // The distance from one tile to another in tiles, including diagonal movement (otherwise it's just manhattan distance)
  // Known as Chebyshev distance.
  // This works basically by just returning the longest x or y difference.
  unsigned long int chebyshevDistance (unsigned long int x1, unsigned long int y1, unsigned long int x2, unsigned long int y2)
  {
    unsigned long int dist1 = DataTools::absoluteDifference(x1,x2);
    unsigned long int dist2 = DataTools::absoluteDifference(y1,y2);
    
    if (dist1 > dist2 ) { return dist1; }
    return dist2;
  }
}

