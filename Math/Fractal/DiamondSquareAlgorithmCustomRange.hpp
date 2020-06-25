#pragma once
#ifndef WILDCAT_MATH_FRACTAL_DIAMONDSQUAREALGORITHMCUSTOMRANGE_HPP
#define WILDCAT_MATH_FRACTAL_DIAMONDSQUAREALGORITHMCUSTOMRANGE_HPP

#include <Container/ArrayS2/ArrayS2.hpp>
#include <Math/Random/RandomLehmer.hpp>

/* Wildcat: DiamondSquareAlgorithmCustomRange.hpp
	#include <Math/Fractal/DiamondSquareAlgorithmCustomRange.hpp>
   
   Uses the diamond-square variant of the midpoint displacement
   algorithm to reduce visible artifacts. Good for terrain generation.
   This one allows a custom range to be specified, which is useful if
   you want high or low resolution output.

	The passed array must be initialised with 0s.

	Implementation seems to be imperfect (most likely for diamond mode). Requires tweaking to get good results.

	High smoothing causes strange artifacts. Must investigate.

	When variance hits 0, artifacts happen. Consider preventing variance from going under 1.

	Using doubles for the array might reduce artifacts.

	Consider multithreading, but not a high priority.

	Algorithm will respect non-zero entries by leaving them alone.
   This allows you to define some basic shapes that you want the
   world to use during generation. For example you can generate
   islands by setting all border values to 1.

	Fixed bug where freesteps were only being done in diamond mode.
   (This may have made artifacts worse... Maybe change it.
   (just comment out the freesteps bit).

	The value table isn't working properly on the fly, so I made it do it again at the end. This can be fixed later. Value table seems to be working now.
   
   The algorithm must support values above 256 because in some cases more precision is required. For example when using a fractal to place rare resources on a map. With 256 values this means that we can't go under 1/256% chance of occurence. Unsigned chars had to be changed to int, and references to 256 have been changed to the required range.
	
	Note that in some cases the biome table must be rebuilt.
	
	Setting the freesteps too high is causing biases to 0 and max values. Anything above 3 seems to create a noticable bias in the value tables.
   
   I changed the RNG from Mersenne Twister to Lehmer. This should significantly improve performance.
   
   Todo: Add repeatable seeding.
*/

class DiamondSquareAlgorithmCustomRange
{
	private:

   RandomLehmer rng;

	static void addToValueTable(int* valueTable, unsigned int value)
	{
		if ( valueTable!=0 )
		{
			valueTable[value]++;
		}
	}

	public:
	
		// Values of the fractal will range from 0 to maxValue inclusive.
   int maxValue;
	
	DiamondSquareAlgorithmCustomRange()
	{
			// Default range is 0-255 inclusive.
		maxValue=255;
	}
	
	void seed(unsigned int seed)
	{
		rng.seed(seed);
	}

	void generate(ArrayS2 <int>* aMap=0, int* aValueTable=0, unsigned short int freeSteps = 0, float smoothing = 0.85, float variance = 250, float varianceDecrement = 0.1)
	{
			// RESET VALUE TABLE
		if ( aValueTable!=0 )
		{
			std::cout<<"Reset value table.\n";
			for (int i=0;i<=maxValue;++i)
			{
				aValueTable[i]=0;
			}
		}
    
    if ( aMap == 0 || aMap->nX==0 || aMap->nY == 0 )
    {
      std::cout<<"Error DiamondSquareAlgorithmCustomRange: Empty or zero-size array passed.\n";
      return;
    }
    
    //Quick check to make sure the array has dimensions (n^2)+1.
    if ( (((aMap->nX-1) & (aMap->nX - 2)) != 0)  || (((aMap->nY-1) & (aMap->nY - 2)) != 0 ) )
    {
      std::cout<<"Error DiamondSquareAlgorithmCustomRange: Array dimensions must be (n^2)+1.\n";
      return;
    }
    

			// Warning: Messing with this can destroy several hours of your life.
		int freeStepValue = -1; // -1 == random.

		unsigned int squareSize = aMap->nX-1;

		// BASE CASE: SET CORNER VALUES (ONLY IF THEY AREN'T ALREADY SET).
		// NOTE THAT A VALUE OF 0 WILL ALWAYS BE OVERWRITTEN.
		if ( (*aMap)(0,0) == 0 )
		{
			(*aMap)(0,0)=rng.rand32(maxValue);
		}
		addToValueTable(aValueTable,(*aMap)(0,0));

		if ( (*aMap)(0,aMap->nY-1) == 0 )
		{
			(*aMap)(0,aMap->nY-1)=rng.rand32(maxValue);
		}
		addToValueTable(aValueTable,(*aMap)(0,aMap->nY-1));

		if ( (*aMap)(aMap->nX-1,0) == 0 )
		{
			(*aMap)(aMap->nX-1,0)=rng.rand32(maxValue);
		}
		addToValueTable(aValueTable,(*aMap)(aMap->nX-1,0));

		if ( (*aMap)(aMap->nX-1,aMap->nY-1) == 0 )
		{
			(*aMap)(aMap->nX-1,aMap->nY-1)=rng.rand32(maxValue);
		}
		addToValueTable(aValueTable,(*aMap)(aMap->nX-1,aMap->nY-1));

		bool squareMode = true;

		while ( squareSize > 1 )
		{
			if ( squareMode==true)
			{
				// Skip the last tile because there's no need to do them.
				for (int _y=0;_y<aMap->nY-1;_y+=squareSize)
				{
					for (int _x=0;_x<aMap->nX-1;_x+=squareSize)
					{
						int targetX = _x + (squareSize/2);
						int targetY = _y + (squareSize/2);

						if ( (*aMap)(targetX,targetY) == 0 ) // DON'T OVERWRITE NON-ZERO ENTRIES.
						{
							if ( freeSteps > 0 )
							{
								if ( freeStepValue == -1 )
								{
									const int _rand = rng.rand32(maxValue);
									(*aMap)(targetX,targetY)=_rand;
									addToValueTable(aValueTable,_rand);
								}
								else
								{
									(*aMap)(targetX,targetY)=freeStepValue;
									addToValueTable(aValueTable,freeStepValue);
								}
							}
							else
							{
								int nCorners = 0;
								int totalCorners = 0;

									// ADD UP CORNERS TO GET AVERAGE.
								if ( aMap->isSafe(_x,_y) == true )
								{
									++nCorners;
									totalCorners+=(*aMap)(_x,_y);
								}
								if ( aMap->isSafe(_x+squareSize,_y) == true )
								{
									++nCorners;
									totalCorners+=(*aMap)(_x+squareSize,_y);
								}
								if ( aMap->isSafe(_x,_y+squareSize) == true )
								{
									++nCorners;
									totalCorners+=(*aMap)(_x,_y+squareSize);
								}
								if ( aMap->isSafe(_x+squareSize,_y+squareSize) == true )
								{
									++nCorners;
									totalCorners+=(*aMap)(_x+squareSize,_y+squareSize);
								}

									// NCORNERS SHOULD ALWAYS EQUAL. SO THE BOUNDS TESTING WOULD NOT NORMALLY BE REQUIRED.
									const int average = totalCorners/4;

								//int variance2 = Random::randInt(variance) - (variance/2);
								int result = average+rng.range32(-variance,variance);
								if (result<0) { result=0; }
								if (result>maxValue) { result=maxValue; }

								(*aMap)(targetX,targetY)=result;
								addToValueTable(aValueTable,result);
							}
						}
						else
						{
							// MAKE SURE NON-ZERO VALUES STILL GET COUNTED IN THE TABLE.
							addToValueTable(aValueTable,(*aMap)(targetX,targetY));
						}
					}
				}
			}
			else // DIAMOND MODE
			{
				squareSize/=2;

					// NEED TO RECHECK THIS. EARLIER NOTES INDICATE POSSIBLE PROBLEM.

				for (int _y=0;_y<aMap->nY;_y+=squareSize)
				{
					for (int _x=0;_x<aMap->nX;_x+=squareSize)
					{
						if ( (*aMap)(_x,_y) == 0 ) // DON'T OVERWRITE NON-ZERO ENTRIES.
						{
							if ( freeSteps > 0 )
							{
								if ( freeStepValue == -1 )
								{
									const unsigned int _rand = rng.rand32(maxValue);
									(*aMap)(_x,_y)=_rand;
									addToValueTable(aValueTable,_rand);
								}
								else
								{
									(*aMap)(_x,_y)=freeStepValue;
									addToValueTable(aValueTable,freeStepValue);
								}
							}
							else
							{
								double total=0;
								int nSides=0; // NOTE THAT IN THIS CASE NSIDES MAY NOT BE 4.

								if ( aMap->isSafe(_x-squareSize,_y) == true )
								{
									++nSides;
									total+= (*aMap)(_x-squareSize,_y);
								}
								if ( aMap->isSafe(_x,_y+squareSize) == true )
								{
									++nSides;
									total+= (*aMap)(_x,_y+squareSize);
								}
								if ( aMap->isSafe(_x+squareSize,_y) == true )
								{
									++nSides;
									total+= (*aMap)(_x+squareSize,_y);
								}
								if ( aMap->isSafe(_x,_y-squareSize) == true )
								{
									++nSides;
									total+= (*aMap)(_x,_y-squareSize);
								}


								int average = 0;
								if (nSides!=0)
								{
									average = total/nSides;
								}

								int result = average+rng.range32(-variance,variance);

								if (result<0) { result=0; }
								if (result>maxValue) { result=maxValue; }

								(*aMap)(_x,_y)=result;
								addToValueTable(aValueTable,result);

							}
						}
						else
						{
							// MAKE SURE NON-ZERO VALUES STILL GET COUNTED IN THE TABLE.
							//std::cout<<"Notouch\n";
							//addToValueTable(aValueTable,(*aMap)(_x,_y));
						}
					}
				}
			}
			squareMode = !squareMode;


				// WE DON'T MODIFY VARIANCE UNTIL THE FREE STEPS RUN OUT.
			if ( freeSteps > 0 )
			{
				--freeSteps;
			}
			else
			{
				if ( variance > 0 )
				{
					variance *= smoothing;
					variance-=varianceDecrement;
				}
				if ( variance < 1 )
				{ variance=1; }
				//{ variance=1; }

				//variance *= smoothing;
			}
		}

			// TEMPORARY FIX FOR VALUE TABLE PROBLEMS.
		if (aValueTable!=0)
		{
			for (int i=0;i<=maxValue;++i)
			{
				aValueTable[i]=0;
			}

			for (int _y=0;_y<aMap->nY;++_y)
			{
				for (int _x=0;_x<aMap->nX;++_x)
				{
									//const int _rand = Random::randInt(maxValue);
									//	std::cout<<"Rand value: "<<_rand<<".\n";
									//(*aMap)(_x,_y)=_rand;
									//addToValueTable(aValueTable,_rand);
					//addToValueTable(aValue);
					aValueTable[(*aMap)(_x,_y)]++;
				}
			}
		}
	}

};

#endif