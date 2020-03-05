#pragma once
#ifndef WILDCAT_BITFIELD_BITFIELD_HPP
#define WILDCAT_BITFIELD_BITFIELD_HPP

/* Wildcat: Bitfield.hpp
	#include <Container/Bitfield/Bitfield.hpp>
   
   Special array which stores bit values on a 2D array.
   Useful if storing large amounts of bits to save RAM
   or disk space.
   
   It saves space but requires a few extra calculations to
   store and retrieve a bit.
   
   This could probably use std::bitset although at this point
   I don't know if it would be faster.
   
   A bitqueue might also be handy. Basically a vector but
   allows pushing bits instead of bool.
   
   Todo: Return a string representation of the bitfield
   for loading/saving.

*/

#include <bitset>

class Bitfield
{
   public:
   //Vector <char> vBit; //  vector of bits already created.
   unsigned char currentBit; // current bit to store data in
   //unsigned char mask;
   
      // bit values. They do not need to be a multiple of 8.
   unsigned long int nX, nY;
      // byte values (always rounded up to nearest 8 bits).
   unsigned long int nXBytes, nYBytes;
   unsigned long int nBytes;
   
   unsigned char * data;
   
   Bitfield()
   {
      //currentBit=0; //
      //mask=1;
      data=0;
      
      nX=0;
      nY=0;
      
      nXBytes=0;
      nYBytes=0;
      
      nBytes=0;
   }
   ~Bitfield()
   {
      if ( data )
      {
         delete [] data;
      }
   }
   
   // return a single bit from the bitfield
   bool operator () (const unsigned int bitX, const unsigned int bitY)
	{
      const unsigned long int targetBit =  nX*bitY+bitX;
      const unsigned long int targetByte = targetBit/8;
      const unsigned long int byteOffset = targetBit%8;

      const unsigned char mask = 1<<byteOffset;

      if ( (mask & data[targetByte]) == 0 )
      {
         return false;
      }

      return true;
   }
   
   void set(const unsigned int bitX, const unsigned int bitY, const bool value)
   {
      const unsigned long int targetBit =  nX*bitY+bitX;
      const unsigned long int targetByte = targetBit/8;
      const unsigned long int byteOffset = targetBit%8;

      const unsigned char mask = 1<<byteOffset;

      if ( value==true )
      {
         data[targetByte] = data[targetByte] | mask;
      }
      else // set byte to 0
      {
         data[targetByte] = data[targetByte] & (~mask);
      }
   }
   // set binary representation of given value from given position, with a maximum of length bits set.
   // overflow not recommended
   // currently only up to 8 bits supported
   void setValue(unsigned int bitX, unsigned int bitY, unsigned char value, unsigned char length)
   {
      std::bitset<8> hello(value);
      for (int i=0;i<length;++i)
      {
         set(bitX,bitY,value);
         ++bitX;
         if (bitX>=nX)
         {
            bitX=0;
            ++bitY;
         }
         if ( bitY>=nY )
         {
            return;
         }
      }
   }
   
   void init (const unsigned int _nX, const unsigned int _nY)
   // this should specify bits not bytes
   // user should be able to specify any number of bits and container will allocate enough space.
   { // will init an array of [NX][NY] filled with 0 bits.
   
      nX = _nX;
      nY = _nY;
      
      unsigned long int nBits = _nX*_nY;
      
      nBytes = nBits/8;
      if ( nBits%8 != 0 )
      {
         ++nBytes;
      }
      data = new unsigned char [nBytes] (); // initialised array to 0 values
      
   }
   
   std::string toString()
   {
      std::string strRet((char*)data,nBytes);
      return strRet;
   }
   
};


#endif