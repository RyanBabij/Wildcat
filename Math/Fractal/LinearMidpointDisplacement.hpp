#pragma once
#ifndef WILDCAT_MATH_FRACTAL_LINEAR_MIDPOINT_DISPLACEMENT_HPP
#define WILDCAT_MATH_FRACTAL_LINEAR_MIDPOINT_DISPLACEMENT_HPP

/* Wildcat: LinearMidpointDisplacement
	#include <Math/Fractal/LinearMidpointDisplacement.hpp> */
	
/**
	Midpoint displacement algorithm to apply to a 1D array. The array must have a length of x^2+1.
*/

class LinearMidpointDisplacement
{
	private:
	
	RandomLehmer rng;
	
	public:
	
	LinearMidpointDisplacement()
	{
	}
	
	void seed(int seed)
	{
		rng.seed(seed);
	}
	void seed (RandomLehmer &seed)
	{
		rng.seed(seed);
	}
	
	// Run midpoint displacement algorithm on the line.
	// Ignore already set values.
	// Line should have n^2+1 elements.
	void generate(int * array, const int len, int maxValue)
	{
		if ( len < 3 )
		{
			std::cout<<"ERROR: LinearMidpointDisplacement: Array should be at least size 3.\n";
			return;
		}
		
		int currentSize = len/2;
		
		if (array[0]==0)
		{
			array[0] = rng.rand32() % maxValue;
		}
		if (array[len-1]==0)
		{
			array[len-1] = rng.rand32() % maxValue;
		}
		
		//while (currentSize>1)
		while (true)
		{
			for (int i=currentSize;i<len-1;i+=currentSize)
			{
				if (array[i]==0)
				{
					//array[i]=(array[i-currentSize]+array[i+currentSize]) / 2;
					int variance = (rng.rand32() % currentSize*3) - currentSize;
					array[i]=((array[i-currentSize]+array[i+currentSize]) / 2) + variance;
					if (array[i] > maxValue)
					{
						array[i] = maxValue;
					}
					if ( array[i]<1 ) // workaround: 0 tiles are considered ininitialized
					{
						array[i]=1;
					}
				}
			}
			if (currentSize==1)
			{
				break;
			}
			currentSize/=2;
		}

	}
};

#endif
