#pragma once
#ifndef WILDCAT_DATA_DATA_TOOLS_HPP
#define WILDCAT_DATA_DATA_TOOLS_HPP

/* Wildcat: Data/DataTools.hpp
   #include <Data/DataTools.hpp>

	Library for miscellaneous common functions involving datatypes.
   
   For now it also includes string and vector operations, although
   in future I may consider a String wrapper class.
*/

#include <Container/Vector/Vector.hpp>

#include <sstream>
#include <string>     // std::string, memcpy (need a better way of doing this.)
#include <iostream>   // std::cout
#include <iomanip>    // setprecision
#include <locale>     // std::isalnum
#include <stdlib.h>   // atoi

#include <string.h> // contains old functions like memcpy


namespace DataTools
{
  // Break string up into a vector of strings split by the passed delimiters.
  // Don't forget to include \n and \r.
   //  tokenize("test,1,2,,,3,", ",") will return "test", "1", "2", and "3".
   Vector <std::string> * tokenize(const std::string _str, const std::string delim)
   {
      // Print a warning that I've tokenised only one of the two newline codes,
      // because I do that too often.
      #define WILDCAT_WARN_CARRIAGE_RETURN
      #ifdef WILDCAT_WARN_CARRIAGE_RETURN
      
      const bool hasNewLine = delim.find('\n') != std::string::npos;
      const bool hasCarriage = delim.find('\r') != std::string::npos;
      
      if (hasNewLine != hasCarriage)
      {
         std::cout<<"WARNING: Tokenising one of two potential carriage returns.\n";
      }
      #endif

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
         {
            temp+=_str[i];
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
  
   // Remove all chars in strTokens from the passed string.
   std::string strip(std::string _strInput, std::string _strTokens)
   {
      for (unsigned int i=0;i<_strTokens.size();++i)
      {
         _strInput.erase(std::remove(_strInput.begin(), _strInput.end(), _strTokens[i]), _strInput.end());
      }
      return _strInput;
   }
  
	
   bool isAlphaNumeric (const std::string _s, const bool allowSpaces=false )
	{ // basically just wraps the stl function.
		for ( const auto c : _s )
		{
         if (std::isalnum(c) == false || (allowSpaces==false && c==' ') )
         {
            return false;
         }
		}
		return true;
	}

		// MODIFIES X AND Y COORDINATES BASED ON NUMERICAL INPUT, IE THE NUMPAD.
	void numpadToCoords(int* const x, int* const y, const int numpadDirection)
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
	inline bool isNumeric ( const char c)
	{
      return std::isdigit(c);
	}
   // removed: isAsciiNumber
		
	inline bool isNumeric (const std::string _s)
	{ // number may start with any number of - or + signs.
      
      bool canSign = true;
      
		for ( const auto c : _s )
		{
			if ( std::isdigit(c) )
			{
            // from this point no more signs are allowed
            canSign=false;
			}
         else if (canSign && (c=='+' || c=='-'))
         {
            // number may start with any number of + or - signs.
         }
			else
			{
				return false;
			}
		}
		return true;
	}
   inline bool isNumber(const std::string _s)
   {  return isNumeric(_s); }
      
	inline bool isAlphaNumeric(const int key)
	{ return std::isalnum(key); }
   
   inline bool isAlpha (const unsigned char _c)
   { return std::isalpha(_c); }
   
   inline bool isAlpha(const std::string _str)
   {
      if ( _str.size() == 0 )
      { return false; }
   
      for (const auto & c: _str)
      {
         if (DataTools::isAlpha(c) == false)
         {
            return false;
         }
      }
      return true;
   }
   
   inline std::string toUpper(std::string _str)
   {
      for (auto & c: _str) c = toupper(c);
      return _str;
   }

   // Returns true if the passed char contains any char in the string.
   inline bool containsAny(char _c, const std::string _charList)
   {
      if (_charList.find(_c) != std::string::npos)
      { return true; }
      return false;
   }

	// Swap 2 variables around
   // There's probably some cool way to do this tbh
	template <class T>
	inline void swap(T * const v1, T * const v2)
	{
		const T temp = *v1;
		*v1 = *v2;
		*v2 = temp;
	}
   
	std::string reverseString(const std::string _inStr)
	{ return std::string (_inStr.rbegin(),_inStr.rend()); }

   // Sanity check to see if the computer has any unusual datatype sizes. Maybe useful for error reports.
	inline bool datatypeSizesOkay()
	{
		if(sizeof(char)>=1&&sizeof(int)>=4&&sizeof(double)>=8&&sizeof(float)>=4)
		{ return true; }
		return false;
	}
   
	/* Turns a variable into a string, and also pads with zeroes. */
	template <class T>
	inline static std::string toString (const T n, unsigned int nZeroes=1)
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
	// template <class T>
	// inline static std::string toString(const T n)
	// {
      // std::ostringstream oss;
		// oss<<n;
		// return oss.str();
	// }

      // Delimit thousands with the given delimiter. Also works on negative values
	std::string delimitThousands(const std::string _str, const char _delimiter)
	{

		if ( _str.size() == 0 )
		{ return ""; }

		std::string dString="";
			// Special case: negative values
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
		return reverseString(dString);
	}

   inline int toInt(const std::string s)
   {
      // If the string is not a valid integer, zero will be returned.
      return stoi(s);
   }
   inline short int toShort(const std::string s)
   {
      // If the string is not a valid integer, zero will be returned.
      return stoi(s);
   }
   inline long int toLong(const std::string s)
   {
      return stol(s);
   }
	inline int toUnsignedInt(const std::string s)
   {
      // If the string is not a valid integer, zero will be returned.
      return stoul(s);
   }
	inline short int toUnsignedShort(const std::string s)
	{
		/* NOTE: If the string is not a valid integer, zero will be returned. */
    return stoul(s);
	}
   inline long int toUnsignedLong(const std::string s)
   {
      return stoul(s);
   }
   
   // NUMBER OPERATIONS ////////////////////////////////////////////////////////////////////////////
   
   unsigned char moveTo(unsigned char v1, unsigned char v2, unsigned char maxMove)
   { // move v1 to v2 by maxMove amount. maxMove must be positive.
   
      //return v2;
   
      if(v1==v2) { return v1; }
      
      short int finalValue = v1;
      
      if ( v1 < v2 )
      {
         finalValue+=maxMove;
         if ( finalValue > v2 )
         {
            return v2;
         }
         return (unsigned char)finalValue;
      }
      else
      {
         finalValue-=maxMove;
         if ( finalValue < v2 )
         {
            return v2;
         }
         return (unsigned char)finalValue;
      }
   
   }
   
   // DISTANCES ////////////////////////////////////////////////////////////////////////////////////
   
   inline unsigned long int absoluteDifference
   ( const unsigned long int val1, const unsigned long int val2)
   {
      if ( val1 == val2 ) { return 0; }

      if (val1 > val2)
      {
         return val1-val2;
      }
      return val2-val1;
   }

   inline unsigned long int manhattanDistance
   (const unsigned long int x1, const unsigned long int y1, const unsigned long int x2, const unsigned long int y2)
   {
      return DataTools::absoluteDifference(x1,x2)+DataTools::absoluteDifference(y1,y2);
   }
  
   /* The distance from one tile to another in tiles, including diagonal movement (otherwise it's
   manhattan distance). Known as Chebyshev distance. This works basically by just returning the
   longest x or y difference. */
   unsigned long int chebyshevDistance
   (const unsigned long int x1, const unsigned long int y1, const unsigned long int x2, const unsigned long int y2)
   {
      const unsigned long int dist1 = DataTools::absoluteDifference(x1,x2);
      const unsigned long int dist2 = DataTools::absoluteDifference(y1,y2);

      if (dist1 > dist2 ) { return dist1; }
      return dist2;
   }
   
   
   // Less common stuff goes here. /////////////////////////////////////////////////////////////////


	template <class T>
	char * toCharArray(T* var)
	{
		/* Convert the data into a stream of bytes for saving. This is a simple implementation that
      will probably break on some systems. May have issues with endian-ness, sizeof()s, etc... */
		return (char*)var;
	}

	/* Get X number of bytes from a char (byte) array and read them into the datatype. If the
   datatype has more space, the empty space is filled with zeros. If the datatype doesn't have
   enough space, then the last bits will be left out. */
	template <class T>
	T getBytesValue (char* data, unsigned int numBytes)
	{
		T returnValue=0;
		char* currentBit = (char*)&returnValue;

		for(unsigned int i=0;i<numBytes && i<sizeof(T);++i)
		{
			*currentBit&=0x00;
			*currentBit|=data[i];
			++currentBit;
      }
		return returnValue;
	}

	/* Outputs the actual binary representation of the given value. Doesn't work on floating point
   values (ie float, double). */
	template <class T>
	std::string getBinaryString (T value, const std::string delimiter="")
	{
		std::string r = "";
		int nBytes = sizeof(T);

		// C++ doesn't support binary values apparently, so I'm using hex here.
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

	template <class T2>
	void fromCharArray(T2** var, char* firstByte)
	{
		/* Convert a stream of bytes into a datatype. This is a simple implementation that will
      probably break on some systems. May have issues with endian-ness, sizeof()s, etc... */
		*var = (T2*)firstByte;
	}
	template <class T3>
	T3 valFromCharArray(char* firstByte)
	{
		/* Similar to above, but returns value rather than pointer. Usage: double d = fromCharArray2
      <double> (bytes) */
		return *(T3*)firstByte;
	}

	template <class T4>
	T4 valFromVector(Vector <char>* vByte, const int startIndex)
	{
		char* arrayByte = new char [sizeof(T4)];

		// Copy bytes from memory to array.
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
	T fromArray(char* address, int numBytes)
	{
		T r;
		return r;
	}

	template <class T>
	void mergeIntoArray(T* number, unsigned char** startAddress)
	/* Will convert the T into a char array, and then merge it into the given array address
	Also, it will position the supplied address pointer to the end of the merge
	Update: Changed from char to unsigned char... Should fix things a little
	Update: Now I'm using memcpy, and things seem to be working better. */
	{
//#ifdef WILDCAT_WINDOWS
		memcpy(*startAddress,number,sizeof(T));
		*startAddress+=sizeof(T);
//#endif
	}
	template <class T>
	void mergeOutOfArray(T* number, unsigned char** startAddress)
	{
		memcpy(number,*startAddress,sizeof(T));
		*startAddress+=sizeof(T);
	}
}

#endif
