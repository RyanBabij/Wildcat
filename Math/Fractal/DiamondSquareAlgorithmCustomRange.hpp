#pragma once

#include <Math/Random/GlobalRandom.hpp>
#include <Container/ArrayS2/ArrayS2.hpp>

#include <Math/Random/GlobalRandomThreadSafe.hpp>

/*
	DiamondSquareAlgorithm

	This algorithm should have less artifacts than the midpoint displacement algorithm.

	The passed array must be initialised with 0s.

	Implementation seems to be imperfect (most likely for diamond mode). Requires tweaking to get good results.

	High smoothing causes strange artifacts. Must investigate.

	When variance hits 0, artifacts happen. Consider preventing variance from going under 1.

	Using doubles for the array might reduce artifacts.

	Consider multithreading, but not a high priority.

	0223413688. Algorithm will now respect non-zero entries by leaving them alone.

	023-236-2049: Fixed bug where freesteps were only being done in diamond mode. (This may have made artifacts worse... Maybe change it. (just comment out the freesteps bit).

	025-162-5792: Removed old code. Noticed that aValueTable isn't working properly. Improved performance, etc, etc. Removed thread related stuff, since this library has nothing to do with threads at the moment. Still needs work.

	The value table isn't working properly on the fly, so I made it do it again at the end. This can be fixed later. Value table seems to be working now.

	
	0253463172 - The fractal has been modified to be more flexible with the range. For the majority of cases, a range of values from 0-255 is enough. However sometimes there is a need for greater detail, for example when using a fractal to place rare resources on a map. With 256 values this means that we can't go under 1/256% chance of occurence. Unsigned chars had to be changed to int, and references to 256 have been changed to the required range.
	
	Note that in some cases the biome table must be rebuilt.
	
	Setting the freesteps too high is causing biases to 0 and max values. Anything above 3 seems to create a noticable bias in the value tables.
	
	0253463626 - Fixed a bug that caused freesteps to be 0, not a random value. Took over an hour to chase down...
	
	TODO: Merge DiamondSquareAlgorithm and DiamondSquareAlgorithmCustomRange possibly using templates. Alternatively we can stick with int for everything here and convert later if we need to save space.
*/

class DiamondSquareAlgorithmCustomRange
{
	private:

	//MTRand random;

	static void addToValueTable(int* valueTable, int value)
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

	void generate(ArrayS2 <int>* aMap=0, int* aValueTable=0, int freeSteps = 0, double smoothing = 0.85, double variance = 250, double varianceDecrement = 0.1)
	{
		//std::cout<<"Variance is: "<<variance<<".\n";
		//std::cout<<"Free steps: " <<freeSteps<<".\n";
		//std::cout<<"GenerateThreadSafe\n";
			// RESET VALUE TABLE
		if ( aValueTable!=0 )
		{
			std::cout<<"Reset value table.\n";
			for ( int i=0;i<=maxValue;++i)
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

		int squareSize = aMap->nX-1;

		// BASE CASE: SET CORNER VALUES (ONLY IF THEY AREN'T ALREADY SET).
		// NOTE THAT A VALUE OF 0 WILL ALWAYS BE OVERWRITTEN.
		if ( (*aMap)(0,0) == 0 )
		{
			(*aMap)(0,0)=Random::randInt(maxValue);
		}
		addToValueTable(aValueTable,(*aMap)(0,0));

		if ( (*aMap)(0,aMap->nY-1) == 0 )
		{
			(*aMap)(0,aMap->nY-1)=Random::randInt(maxValue);
		}
		addToValueTable(aValueTable,(*aMap)(0,aMap->nY-1));

		if ( (*aMap)(aMap->nX-1,0) == 0 )
		{
			(*aMap)(aMap->nX-1,0)=Random::randInt(maxValue);
		}
		addToValueTable(aValueTable,(*aMap)(aMap->nX-1,0));

		if ( (*aMap)(aMap->nX-1,aMap->nY-1) == 0 )
		{
			(*aMap)(aMap->nX-1,aMap->nY-1)=Random::randInt(maxValue);
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
									const int _rand = Random::randInt(maxValue);
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
								int result = average+Random::range(-variance,variance);
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
									const int _rand = Random::randInt(maxValue);
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

								int result = average+Random::range(-variance,variance);

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
			for ( int i=0;i<=maxValue;++i)
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
