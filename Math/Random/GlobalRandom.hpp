#pragma once

/*
#include <Math/Random/GlobalRandom.hpp>
Wrapper for multiple RNGs. Currently just uses Mersenne Twister, which works fine.

RNG is global to save performance for game development. Also good for preserving seed.

0272407085 - Replaced external Mersenne Twister with built-in version. External one was causing random crashes with certain compiler optimisations. Took about 4 hours to track it down.
*/

#ifndef GLOBALRANDOM_HPP
#define GLOBALRANDOM_HPP

//#include <Math/Random/MersenneTwister.hpp>

// We must migrate to c++ standard mersenne twister.

// Mersenne twister is now part of c++
#include <random>
#include <functional>

//std::mt19937 gen (123);
		//std::uniform_real_distribution<double> rnd(0,255);
		//int rand = dis(generator);
		
//std::mt19937::result_type seed2 = time(0);
//auto rand2 = std::bind(std::uniform_int_distribution<int>(0,255),
                          // std::mt19937(seed2));

class Random
{
	public:
	//static MTRand r;
	static std::mt19937 r2;

	inline static int randomInt(const int maxVal)
	{
		//return r.randInt(maxVal);
	

		// std::mt19937::result_type seed2 = time(0);
		// std::mt19937 rng (seed2);
		std::uniform_int_distribution<int> randomDistribution(0,maxVal);
		return randomDistribution(r2);
	}
	inline static int randInt(const int maxVal)
	{ return randomInt(maxVal); }

	void seed (const int s)
	{
		//r.seed(s);
		r2.seed(s);
	}

		//0231263121. THIS SHOULD SEED RANDOMLY...
	static void seed ()
	{
		//r.seed();
		r2.seed(time(0));
	}

	inline static double randDouble (const double maxVal)
	{
		std::uniform_real_distribution<double> randomDistribution(0,maxVal);
		return randomDistribution(r2);
		//return r.rand(maxVal);
	}
	inline static double randomDouble (const double maxVal)
	{ return randDouble(maxVal); }


	//0231263245. Range is inclusive. Seems to work with negative values.
	static int randomIntRange(const int _min, const int _max)
	{
		if (_min==_max) { return _min; }


		//int range = max-min;
		//int _ret = (r.randInt(range)) + min;
		return (randInt(_max-_min)) + _min;
	}
		static int range ( const int _min, const int _max)
		{ return randomIntRange(_min,_max); }


	/* Will return true one in 'prob' times. Ie, with prob at 100, will return true roughly 1 in 100 times. */
	static bool oneIn(const int prob)
	{
		return (randInt(prob)==0);
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
	std::mt19937 r2;
	
	public:

	inline int randomInt(const int maxVal)
	{
		//return r.randInt(maxVal);
	

		// std::mt19937::result_type seed2 = time(0);
		// std::mt19937 rng (seed2);
		std::uniform_int_distribution<int> randomDistribution(0,maxVal);
		return randomDistribution(r2);
	}
	inline int randInt(const int maxVal)
	{ return randomInt(maxVal); }

	void seed (const int s)
	{
		r2.seed(s);
	}

		//0231263121. THIS SHOULD SEED RANDOMLY...
	void seed ()
	{
		//r.seed();
		r2.seed(time(0));
	}

	inline double randDouble (const double maxVal)
	{
		std::uniform_real_distribution<double> randomDistribution(0,maxVal);
		return randomDistribution(r2);
		//return r.rand(maxVal);
	}
	inline double randomDouble (const double maxVal)
	{ return randDouble(maxVal); }


	//0231263245. Range is inclusive. Seems to work with negative values.
	int randomIntRange(const int _min, const int _max)
	{
		if (_min==_max) { return _min; }


		//int range = max-min;
		//int _ret = (r.randInt(range)) + min;
		return (randInt(_max-_min)) + _min;
	}
		int range ( const int _min, const int _max)
		{ return randomIntRange(_min,_max); }


	/* Will return true one in 'prob' times. Ie, with prob at 100, will return true roughly 1 in 100 times. */
	bool oneIn(const int prob)
	{
		return (randInt(prob)==0);
		//if(r.randInt(prob)==0) { return true; }
		//else { return false; }
		//return false;
	}

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
