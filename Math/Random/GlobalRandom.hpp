#pragma once

/*
#include <Math/Random/GlobalRandom.hpp>
Wrapper for multiple RNGs.
*/

#ifndef GLOBALRANDOM_HPP
#define GLOBALRANDOM_HPP

#include <Math/Random/MersenneTwister.hpp>

class Random
{
	public:
	static MTRand r;


	inline static int randomInt(const int maxVal)
	{ return r.randInt(maxVal); }
	inline static int randInt(const int maxVal)
	{ return r.randInt(maxVal); }

	void seed (const int s)
	{ r.seed(s); }

		//0231263121. THIS SHOULD SEED RANDOMLY...
	static void seed ()
	{ r.seed(); }

	inline static double randDouble (const double maxVal)
	{
		return r.rand(maxVal);
	}
	inline static double randomDouble (const double maxVal)
	{ return randDouble(maxVal); }


	//0231263245. Range is inclusive. Seems to work with negative values.
	static int randomIntRange(const int _min, const int _max)
	{
		if (_min==_max) { return _min; }


		//int range = max-min;
		//int _ret = (r.randInt(range)) + min;
		return (r.randInt(_max-_min)) + _min;
	}
		static int range ( const int _min, const int _max)
		{ return randomIntRange(_min,_max); }


	/* Will return true one in 'prob' times. Ie, with prob at 100, will return true roughly 1 in 100 times. */
	static bool oneIn(const int prob)
	{
		return (r.randInt(prob)==0);
		//if(r.randInt(prob)==0) { return true; }
		//else { return false; }
		//return false;
	}

	/* Shorthand for randInt(). */
	static int roll(const int _maxVal)
	{ return randInt(_maxVal); }

	static int roll(const int min, const int max)
	{ return randInt(max-min)+min; }

	static bool flip()
	{ return ( randInt(1) == 0 ); }

	static int multiRoll (int nDices, int diceMax, bool allowZeroRoll=false)
	{
		int total=0;

		for ( int i=0;i<nDices;++i)
		{
			if ( allowZeroRoll == false )
			{
				total+=Random::randomInt(diceMax-1)+1;
			}
			else
			{
				total+=Random::randomInt(diceMax);
			}

		}
		return total;
	}
	static bool percentTrue (double percentTrue)
	{
		int roll = Random::randomInt(100);
		if ( roll < percentTrue )
		{
			return true;
		}
		return false;
	}

};
MTRand Random::r;

#endif
