#pragma once
#ifndef WILDCAT_MATH_PRIMES_PRIMES_HPP
#define WILDCAT_MATH_PRIMES_PRIMES_HPP

/* Basic math #include <Math/Primes/Primes.hpp>



*/



#include <math.h>

/* NOTE: All decimal numbers should use double datatype. */

class Primes
{
	public:
	
	/* Function to determine if a number is prime, copied from http://www.cplusplus.com/forum/general/1125/ */
	static bool isPrime (const int num)
	{
		if (num <=1)
			return false;
		else if (num == 2)         
			return true;
		else if (num % 2 == 0)
			return false;
		else
		{
			bool prime = true;
			int divisor = 3;
			double num_d = static_cast<double>(num);
			int upperLimit = static_cast<int>(sqrt(num_d) +1);
			
			while (divisor <= upperLimit)
			{
				if (num % divisor == 0)
					prime = false;
				divisor +=2;
			}
			return prime;
		}
	}

	/* Returns a number which is equal to or greater than baseNumber which is also a prime. */
	static int nearestPrime(int baseNumber)
	{
		while(Primes::isPrime(baseNumber)==false)
		{ ++baseNumber; }
	
		return baseNumber;
		
	}

};

#endif