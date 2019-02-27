#pragma once
#ifndef GLOBALRANDOM_HPP
#define GLOBALRANDOM_HPP

/*
#include <Math/Random/GlobalRandom.hpp>
Wrapper for multiple RNGs. Currently just uses Mersenne Twister, which works fine.

RNG is global to save performance for game development. Also good for preserving seed.

0272407085 - Replaced external Mersenne Twister with built-in version. External one was causing random crashes with certain compiler optimisations. Took about 4 hours to track it down.

C++11 introduced a lot of built-in functionality regarding random number generation (remember std::rand lol). Unfortunately it means that a lot of this code needs to be reworked. There also seems to be some kind of elusive bug caused by using the static Random class, not sure if it's thread-related or what. For now I'm sticking with the non-static class, but yeah it's a mess.

Oh man, I just noticed the same RNG is being used for both static and non-static instances. That was fun. Better to have a defined static function for global shared rng.
*/



// Mersenne twister is now part of c++
#include <random>
#include <functional>

#include <time.h> /* Required to call time() */

  // NOTE THAT THIS IS STATIC. USE RandomNonStatic when you are using predictable seeding.
  // This RNG is global. Basically it just makes life more convenient when you can seed once
  // and then call Random:: whenever you want a random number.
class Random
{
	private:
	static std::mt19937 r2;

  public:
  
	inline static int randomInt(const int maxVal)
	{
		std::uniform_int_distribution<int> randomDistribution(0,maxVal);
		return randomDistribution(r2);
	}
	inline static int randInt(const int maxVal)
	{ return randomInt(maxVal); }

    // Seed using an int
	void seed (const int s)
	{ r2.seed(s); }
  
    // By default we seed using time
	static void seed ()
	{ r2.seed(time(0)); }

	inline static double randDouble (const double maxVal)
	{
		std::uniform_real_distribution<double> randomDistribution(0,maxVal);
		return randomDistribution(r2);
	}
	inline static double randomDouble (const double maxVal)
	{ return randDouble(maxVal); }


	//0231263245. Range is inclusive. Seems to work with negative values.
	static int randomIntRange(const int _min, const int _max)
	{
		if (_min==_max) { return _min; }

		return (randInt(_max-_min)) + _min;
	}
		static int range ( const int _min, const int _max)
		{ return randomIntRange(_min,_max); }


	/* Will return true one in 'prob' times. Ie, with prob at 100, will return true roughly 1 in 100 times. */
	static bool oneIn(const int prob)
	{ return (randInt(prob)==0); }

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
				total+=randomInt(diceMax-1)+1;
			}
			else
			{
				total+=randomInt(diceMax);
			}

		}
		return total;
	}
	static bool percentTrue (double percentTrue)
	{
		int roll = randomInt(100);
		if ( roll < percentTrue )
		{
			return true;
		}
		return false;
	}
    // Lol what the hell is this
	static double randNorm (double a, double b)
	{
		return 7;
		//std::normal_distribution<double> distribution(5.0,2.0);
	}

};
//MTRand Random::r;
std::mt19937 Random::r2;

// Using the static random class can cause issues.
class RandomNonStatic
{
	private:
	std::mt19937 rng;
	
	public:

	inline int randomInt(const int maxVal)
	{
		std::uniform_int_distribution<int> randomDistribution(0,maxVal);
		return randomDistribution(rng);
	}
	inline int randInt(const int maxVal)
	{ return randomInt(maxVal); }

    // Seed using an int
	void seed (const int s)
	{ rng.seed(s); }
  
    // By default we seed using time
  void seed ()
	{ rng.seed(time(0)); }

	inline double randDouble (const double maxVal)
	{
		std::uniform_real_distribution<double> randomDistribution(0,maxVal);
		return randomDistribution(rng);
	}
	inline double randomDouble (const double maxVal)
	{ return randDouble(maxVal); }


	//0231263245. Range is inclusive. Seems to work with negative values.
	int randomIntRange(const int _min, const int _max)
	{
		if (_min==_max) { return _min; }
		return (randInt(_max-_min)) + _min;
	}
		int range ( const int _min, const int _max)
		{ return randomIntRange(_min,_max); }


	/* Will return true one in 'prob' times. Ie, with prob at 100, will return true roughly 1 in 100 times. */
	bool oneIn(const int prob)
	{ return (randInt(prob)==0); }

	/* Shorthand for randInt(). */
	int roll(const int _maxVal)
	{ return randInt(_maxVal); }

	int roll(const int min, const int max)
	{ return randInt(max-min)+min; }

	bool flip()
	{ return ( randInt(1) == 0 ); }

	int multiRoll (int nDices, int diceMax, bool allowZeroRoll=false)
	{
		int total=0;

		for ( int i=0;i<nDices;++i)
		{
			if ( allowZeroRoll == false )
			{
				total+=randomInt(diceMax-1)+1;
			}
			else
			{
				total+=randomInt(diceMax);
			}

		}
		return total;
	}
	bool percentTrue (double percentTrue)
	{
		int roll = randomInt(100);
		if ( roll < percentTrue )
		{
			return true;
		}
		return false;
	}
	
	double randNorm (double a, double b)
	{
		return 7;
		//std::normal_distribution<double> distribution(5.0,2.0);
	}

};

#endif
