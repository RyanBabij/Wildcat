#pragma once
#ifndef WILDCAT_MATH_FRACTAL_DIAMONDSQUAREALGORITHM_HPP
#define WILDCAT_MATH_FRACTAL_DIAMONDSQUAREALGORITHM_HPP

#include <Math/Random/RandomLehmer.hpp>
#include <Container/ArrayS2/ArrayS2.hpp>

/* Wildcat: DiamondSquareAlgorithm.hpp
	#include <Math/Fractal/DiamondSquareAlgorithm.hpp>

   Uses the diamond-square variant of the midpoint displacement
   algorithm to reduce visible artifacts. Good for terrain generation.

	The passed array must be initialised with 0s.

	Implementation seems to be imperfect (most likely for diamond mode). Requires tweaking to get good results.

	High smoothing causes strange artifacts. Must investigate.

	When variance hits 0, artifacts happen.

	Using doubles for the array might reduce artifacts.

	In the future I would like multithreading.

	Algorithm will now respect non-zero entries by leaving them alone.

	Fixed bug where freesteps were only being done in diamond mode. (This may have made artifacts worse... Maybe change it. (just comment out the freesteps bit).

	Removed old code. Noticed that aValueTable isn't working properly. Improved performance, etc, etc. Removed thread related stuff, since this library has nothing to do with threads at the moment. Still needs work.

	The value table isn't working properly on the fly, so I made it do it again at the end. This can be fixed later. Value table seems to be working now.

	Smoothing now continues through free steps to ensure consistent smoothing while playing with free steps values.
	
	Variance should not be set excessively high otherwise it will bias towards extreme values. A good rule is to start it at half the maximum range.
	Tweaked variance random range to respect bounds.
	
	Readded basic cratering option.
   
   I changed the RNG from Mersenne Twister to Lehmer. This should significantly improve performance.
   
   Todo: Add repeatable seeding and null seeding.
*/

class DiamondSquareAlgorithm
{
	private:
   RandomLehmer rng;

	static void addToValueTable(int* valueTable, int value)
	{
		if ( valueTable!=0 )
		{
			valueTable[value]++;
		}
	}

	public:
	
	int seed;
	
	bool wrapX;
	bool wrapY;
	
	DiamondSquareAlgorithm()
	{
		seed = 0;
		wrapX=false;
		wrapY=false;
	}

		// Note that random numbers are generated even if they aren't used, to map rng output to each tile regardless of settings.
	void generate(ArrayS2 <unsigned char>* aMap=0, int* aValueTable=0, unsigned short int freeSteps = 0, float smoothing = 0.85, float variance = 250, float varianceDecrement = 0.1, const bool cratering=false)
	{
		if ( seed==0 )
		{
			rng.seed();
		}
		else
		{
			rng.seed(seed);
		}
		
			// RESET VALUE TABLE
		if ( aValueTable!=0 )
		{
			for ( int i=0;i<256;++i)
			{
				aValueTable[i]=0;
			}
		}

		int freeStepValue = -1; // -1 == random.


		if (aMap==0) { std::cout<<"ERROR\n"; return; }
		unsigned int mapSize = aMap->nX;

		unsigned int squareSize = aMap->nX-1;

		
		// BASE CASE: SET CORNER VALUES (ONLY IF THEY AREN'T ALREADY SET).
		// NOTE THAT A VALUE OF 0 WILL ALWAYS BE OVERWRITTEN.
		//if ( wrappingMode==false )
		{
			if ( (*aMap)(0,0) == 0 )
			{
				(*aMap)(0,0)=rng.rand8();
			}
		//	addToValueTable(aValueTable,(*aMap)(0,0));

			if ( (*aMap)(0,aMap->nY-1) == 0 )
			{
				(*aMap)(0,aMap->nY-1)=rng.rand8();
			}
		//	addToValueTable(aValueTable,(*aMap)(0,aMap->nY-1));

			if ( (*aMap)(aMap->nX-1,0) == 0 )
			{
				(*aMap)(aMap->nX-1,0)=rng.rand8();
			}
		//	addToValueTable(aValueTable,(*aMap)(aMap->nX-1,0));

			if ( (*aMap)(aMap->nX-1,aMap->nY-1) == 0 )
			{
				(*aMap)(aMap->nX-1,aMap->nY-1)=rng.rand8();
			}
		//	addToValueTable(aValueTable,(*aMap)(aMap->nX-1,aMap->nY-1));
		}
		// else
		// {
			// const int cornerValue = random.randInt(255);
			// addToValueTable(aValueTable,cornerValue);
			// addToValueTable(aValueTable,cornerValue);
			// addToValueTable(aValueTable,cornerValue);
			// addToValueTable(aValueTable,cornerValue);
			// (*aMap)(0,0)=cornerValue;
			// (*aMap)(0,aMap->nY-1)=cornerValue;
			// (*aMap)(aMap->nX-1,0)=cornerValue;
			// (*aMap)(aMap->nX-1,aMap->nY-1)=cornerValue;
		// }

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
									const int _rand = rng.rand8();
									(*aMap)(targetX,targetY)=_rand;
							//		addToValueTable(aValueTable,_rand);
								}
								else
								{
									(*aMap)(targetX,targetY)=freeStepValue;
							//		addToValueTable(aValueTable,freeStepValue);
									// Generate a random number to keep rng predictable for each tile.
									rng.rand8();
								}
							}
							else if ( squareSize > 6 && cratering==true && rng.rand32(100)==0 )
							{
								const int _rand = rng.rand8();
								(*aMap)(targetX,targetY)=_rand;
								//(*aMap)(targetX,targetY)=0;
						//		addToValueTable(aValueTable,_rand);
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

								//int variance2 = random.randInt(variance) - (variance/2);
								int result = average;
								if ( variance > 0 )
								{
											// Ensure that random range stays in bounds to prevent biases from rounding. This pokes lots of holes in landmasses though, so I've disabled it.
									int lowerRange = -variance;
									//if (lowerRange<0) { lowerRange=0; }
									int higherRange = variance;
									//if (higherRange > 255) { higherRange = 255; }
									result = average+rng.range32(lowerRange,higherRange);
								}
								else
								{
									// Generate a random number to keep rng predictable for each tile.
									rng.rand8();
								}
								
									// Just to be sure.
								if (result<0) { result=0; }
								if (result>255) { result=255; }

								(*aMap)(targetX,targetY)=result;
							//	addToValueTable(aValueTable,result);
							}
						}
						else
						{
							// MAKE SURE NON-ZERO VALUES STILL GET COUNTED IN THE TABLE.
							//addToValueTable(aValueTable,(*aMap)(targetX,targetY));
							// Generate a random number to keep rng predictable for each tile.
							rng.rand8();
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
									const unsigned char _rand = rng.rand8();
									(*aMap)(_x,_y)=_rand;
								//	addToValueTable(aValueTable,_rand);
								}
								else
								{
									(*aMap)(_x,_y)=freeStepValue;
								//	addToValueTable(aValueTable,freeStepValue);
								
									// Generate a random number to keep rng predictable for each tile.
									rng.rand8();
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
								if (result>255) { result=255; }

								(*aMap)(_x,_y)=result;
							//	addToValueTable(aValueTable,result);
								
								// Wrap X and Y. This is a basic implementation which works well enough. Need to improve base case.
								if ( wrapX == true && _x == 0 )
								{
									(*aMap)(aMap->nX-1,_y) = result;
								}
								if ( wrapY == true && _y == 0 )
								{
									(*aMap)(_x,aMap->nY-1) = result;
								}
							}
						}
						else
						{
							// MAKE SURE NON-ZERO VALUES STILL GET COUNTED IN THE TABLE.
							//std::cout<<"Notouch\n";
							//addToValueTable(aValueTable,(*aMap)(_x,_y));
							
							// Generate a random number to keep rng predictable for each tile.
							rng.rand8();
						}
					}
				}
			}

				// WE DON'T MODIFY VARIANCE UNTIL THE FREE STEPS RUN OUT.
			if ( freeSteps > 0 )
			{ --freeSteps; }

				// Smoothing will not be ignored by freesteps because this changes smoothing when changing freesteps.
				// This looks like it needs to be debugged.
			if ( variance > 0 )
			{
				variance *= smoothing;
				variance -= varianceDecrement;
			}
			if ( variance < 1 )
			{ variance=0; }

			squareMode=!squareMode;
		}

			// TEMPORARY FIX FOR VALUE TABLE PROBLEMS.
		if (aValueTable!=0)
		{
			for ( int i=0;i<256;++i)
			{
				aValueTable[i]=0;
			}

			for (int _y=0;_y<aMap->nY;++_y)
			{
				for (int _x=0;_x<aMap->nX;++_x)
				{
					//addToValueTable(aValue);
					aValueTable[(*aMap)(_x,_y)]++;
				}
			}
		}
	}

};

#endif
