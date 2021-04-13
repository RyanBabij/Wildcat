#pragma once
#ifndef WILDCAT_MATH_RANDOM_RANDOM_INTERFACE_HPP
#define WILDCAT_MATH_RANDOM_RANDOM_INTERFACE_HPP

/* Wildcat: RandomInterface
#include <Math/Random/RandomInterface.hpp>

Interface for passing randomness to a class or function. A lot of my classes have functionality to perform randomised
functions, for example selecting a random element out of a container. To simplify this I have this RNG interface
which can be passed instead of having to generate sub-RNGs for classes.

Basically guarantees that it can pass a random 8 bit number, a random 32 bit number, or a random boolean.

Seeding functionality is not required as it is assumed the RNG will be seeded outside this context.

Different functions might be optimal for different RNGs.

*/

class RandomInterface
{
	public:
	RandomInterface()
	{
	}
	
	virtual unsigned char rand8()=0; // return a random number from 0-255
	virtual unsigned char rand8(const unsigned char max)=0;
	
	virtual uint32_t rand32()=0; // return a random number from 0-4294967295
	virtual uint32_t rand32(const uint32_t _max)=0;
	
	// Return true 50% of the time.
	virtual bool flip()=0;
	
	// Return true one in prob times.
	virtual bool oneIn(const uint16_t prob)=0;
	
	// Return number between ranges. Range is inclusive.
	virtual int32_t range32(const int32_t _min, const int32_t _max)=0;
};


#endif
