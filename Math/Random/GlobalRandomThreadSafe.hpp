#pragma once

/*
#include <Math/Random/GlobalRandomThreadSafe.hpp>
*/

#ifndef GLOBALRANDOMTHREADSAFE_HPP
#define GLOBALRANDOMTHREADSAFE_HPP

#include <Math/Random/MersenneTwister.hpp>
#include <Thread/Thread.hpp>

class Random_ThreadSafe
{
	public:
	ThreadLock randomLock;

	MTRand random;
	
	Random_ThreadSafe()
	{}
	

	
	int randomInt(const int val)
	{
		randomLock.lock();
		const int randomNumber = random.randInt(val);
		randomLock.unlock();
		return randomNumber;
	}
};

#endif
