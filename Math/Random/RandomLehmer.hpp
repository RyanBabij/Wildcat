#pragma once
#ifndef WILDCAT_MATH_RANDOM_RANDOMLEHMER_HPP
#define WILDCAT_MATH_RANDOM_RANDOMLEHMER_HPP

/* Wildcat: RandomLehmer
	#include <Math/Random/RandomLehmer.hpp>

Basic implementation of Lehmer random number generator, using code from
https://www.youtube.com/watch?v=ZZY9YE7rZJw (16:09).

This RNG is very compact and simple, and good enough for games. It is about
5x faster than Mersenne Twister at generating 32-bit values.

rand8 just generates a 32 bit value and then splits it into 4 8-bit values. As one
would hope, this runs about 4x faster than rand32. It actually runs about 5x
faster if you don't modulus the result.

I haven't benchmarked flip() but I would guess it's even faster than rand8() because it can
pull 32 velues before running the rng again.

Theoretically using 64-bit would double the speed but I currently compile
only for 32 bit.

I'll probably shift all of my RNG stuff to using this class, but it'll take a while.

Note: For good quality seeding you should pass an rng instead of something like time(NULL), to prevent
accidentally seeding multiple RNGs with the same value.

*/

#include <Math/Random/RandomInterface.hpp>

class RandomLehmer: public RandomInterface
{
	private:
	
	uint32_t nLehmer;

	// 8 bit
	uint32_t currentVal;
	unsigned char currentByte;
	unsigned char aByte[4];
	unsigned char flipMask; // 8 flips per byte.

	public:

	RandomLehmer(const uint32_t _seed=0)
	{
		nLehmer=_seed;
		currentByte=3;
		currentVal=0;
		flipMask=0;
	}

	void seed (const uint32_t _seed=0)
	{
		nLehmer=_seed;
	}

	// you can seed the rng with another rng to prevent all your rngs being accidentally seeded with the same
	// time(NULL) value.
	void seed (RandomLehmer &seeder)
	{
		nLehmer = seeder.rand32();
	}

	// generate a psuedorandom number from 0-4294967295
	uint32_t rand32() override
	{
		nLehmer += 0xe120fc15;
		uint64_t tmp;
		tmp = (uint64_t)nLehmer * 0x4a39b70d;
		uint32_t m1 = (tmp >>32)^tmp;
		tmp = (uint64_t)m1*0x12fad5c9;
		uint32_t m2=(tmp>>32)^tmp;
		return m2;
	}
	inline uint32_t rand32(const uint32_t _max) override
	{
		if ( _max==0 ) { return 0; }
		return rand32()%_max;
	}
	inline uint32_t rand(const uint32_t _max)
	{ return rand32(_max); } 

	// return random number from 0-255. Generates 4 values at a time
	// pulling from a 32 bit value.
	unsigned char rand8() override
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
	inline unsigned char rand8(const unsigned char max) override
	{
		return rand8()%max;
	}
	
	// basically returns true 50% of the time. Uses bitmasks for performance.
	inline bool flip() override
	{
		flipMask = flipMask << 1;
		if (flipMask == 0)
		{
			rand8();
			flipMask=1;
		}
		return aByte[currentByte] & flipMask;
	}
	
	inline uint32_t multiRoll8(const uint16_t nDices, const unsigned char diceMax, const bool allowZeroRoll=false)
	{
		uint32_t total=0;

		for ( uint16_t i=0;i<nDices;++i)
		{
			if ( allowZeroRoll == false )
			{
				total+=rand8(diceMax-1)+1;
			}
			else
			{
				total+=rand8(diceMax);
			}
		}
		return total;
	}
	inline uint32_t multiRoll32(const uint16_t nDices, const uint16_t diceMax, const bool allowZeroRoll=false)
	{
		uint32_t total=0;
		for ( uint16_t i=0;i<nDices;++i)
		{
			if ( allowZeroRoll == false )
			{
				total+=rand8(diceMax-1)+1;
			}
			else
			{
				total+=rand8(diceMax);
			}
		}
		return total;
	}

	/* Will return true one in 'prob' times. Ie, with prob at 100, will return true roughly 1 in 100 times. */
	inline bool oneIn(const uint16_t prob) override
	{ return (rand32(prob)==0); }

	//Range is inclusive. Seems to work with negative values.
	inline int32_t range32(const int32_t _min, const int32_t _max) override
	{
		if (_min==_max) { return _min; }
		return (rand32(_max-_min)) + _min;
	}

};

#endif
