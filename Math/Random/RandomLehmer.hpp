#pragma once
#ifndef WILDCAT_MATH_RANDOM_RANDOMLEHMER_HPP
#define WILDCAT_MATH_RANDOM_RANDOMLEHMER_HPP

/*
#include <Math/Random/RandomLehmer.hpp>

Basic implementation of Lehmer random number generator, using code from
https://www.youtube.com/watch?v=ZZY9YE7rZJw (16:09).

This RNG is very compact and simple, and good enough for games. It is about
5x faster than Mersenne Twister at generating 32-bit values.

rand8 just generates a 32 bit value and then splits it into 4 8-bit values. As one
would hope, this runs about 4x faster than rand32. It actually runs about 5x
faster if you don't modulus the result.

Theoretically using 64-bit would double the speed but I currently compile
only for 32 bit.

I'll probably shift all of my RNG stuff to using this class, but it'll take a while.

*/

class RandomLehmer
{
   uint32_t nLehmer;

   // 8 bit
   uint32_t currentVal;
   unsigned char currentByte;
   unsigned char aByte[4];

   public:

   RandomLehmer()
   {
      nLehmer=0;
      currentByte=3;
      currentVal=0;
   }

   // generate a psuedorandom number from 0-4294967295
   uint32_t rand32()
   {
      nLehmer += 0xe120fc15;
      uint64_t tmp;
      tmp = (uint64_t)nLehmer * 0x4a39b70d;
      uint32_t m1 = (tmp >>32)^tmp;
      tmp = (uint64_t)m1*0x12fad5c9;
      uint32_t m2=(tmp>>32)^tmp;
      return m2;
   }

   // return random number from 0-255. Generates 4 values at a time
   // pulling from a 32 bit value.
   unsigned char rand8()
   {
      ++currentByte;
      if ( currentByte==4)
      {
         currentVal=rand32();
         currentByte=0;
         // stolen from stackoverflow.com/questions/30777450/bitwise-bitshift-operations-on-64-bit-integers-in-c
         aByte[0] = (currentVal & 0xff000000UL) >> 24;
         aByte[1] = (currentVal & 0x00ff0000UL) >> 16;
         aByte[2] = (currentVal & 0x0000ff00UL) >>  8;
         aByte[3] = (currentVal & 0x000000ffUL);
      }
      return aByte[currentByte];
   }
   
};

#endif
