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

*/

class Bitfield
{
   public:
   Vector <char> vBit; //  vector of bits already created.
   unsigned char currentBit; // current bit to store data in
   //unsigned char mask;
   
      // bit values. They do not need to be a multiple of 8.
   unsigned long int nX, nY;
      // byte values (always rounded up to nearest 8 bits).
   unsigned long int nXBytes, nYBytes;
   
   unsigned char * data;
   
   Bitfield()
   {
      currentBit=0; //
      //mask=1;
      data=0;
      
      nX=0;
      nY=0;
      
      nXBytes=0;
      nYBytes=0;
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
   
   void init (const unsigned int _nX, const unsigned int _nY)
   // this should specify bits not bytes
   // user should be able to specify any number of bits and container will allocate enough space.
   { // will init an array of [NX][NY] filled with 0 bits.
   
      nX = _nX;
      nY = _nY;
      
      unsigned long int totalBytes = _nX*_nY;
      if ( totalBytes%8 != 0 )
      {
         ++totalBytes;
      }
      std::cout<<"Initializing array of "<<totalBytes<<" bytes.\n";
      data = new unsigned char [totalBytes] (); // initialised array to 0 values
      
   }
   
   
   
   void addBit(const bool bit) // move to bitqueue
   {
      // if ( bit )
      // {
         // currentBit = currentBit | (mask & 255);
      // }
      // mask=mask<<1;
      
      // if (mask==0)
      // {
         // vBit.push(currentBit);
         // currentBit=0;
         // mask=1;
      // }
   }

   void save()
   {
      // return binary data which can be written to a file.
   }
   void load()
   {
   }
   
   
};


#endif