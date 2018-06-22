#pragma once

#include <Math/Random/GlobalRandom.hpp>
#include <Container/ArrayS2/ArrayS2.hpp>

// Port this into a generalised fractal class at some point.
// First I'd like to implement other algorithms, such as
// Diamond-square and a proper midpoint displacement.


/* Update 0223413688. Algorithm will now respect non-zero entries by leaving them alone. */

class MidpointDisplacement
{
	private:
	static void addToValueTable(int* valueTable, int value)
	{
		if ( valueTable!=0 )
		{
			valueTable[value]++;
		}
		
	}
	

	public:
	
		//Simplified midpoint displacement algorithm. Useful for randomly generating terrain.
		// SEED. Set to 0 for random seed.
		// The following create random fluctuations, such as lakes and hills on flat terrain, good for making craters.
		// BREAK SMOOTHNESS. Set to 0 to disable. Higher is less frequent fluctuations.
		// BROKEN_MULTIPLIER. How much the standard variation may be multiplied by, to achieve a fluctuation.
		// SQUARESIZE_LIMIT. The minimum size of a fluctuation. Too small will cause artifacts.
		// freeSteps. Increasing this belond 0 will increase the amount of continents.
		
		/* heightTable. A table that records a count of each height generated. */
		
	void midpointDisplacement(ArrayS2 <unsigned char>* const heightMap, const int SEED=0,
	const int BREAK_SMOOTHNESS=0, const int BROKEN_MULTIPLIER=0, const int SQUARESIZE_LIMIT=0, int freeSteps=0, int* heightTable=0)
	{
	
		if(heightMap==0)
		{ std::cout<<"MidpointDisplacement. Error: heightMap is 0.\n"; return; }
	
		//int freeSteps = 0; //Makes more landmasses, kinda. Too high will make artifacts. 0 is normal, and probably gets best results.

		/*
		1024
		512
		256
		128
		64
		32
		16
		8
		4
		2
		1
		*/
		
		//heightMap->init(1025,1025,0);
		
		
		/* Configure RNG. 0 = random seed. */
		RandomNonStatic random;
		if(SEED!=0)
		{ random.seed(SEED); }
		

		/* Initial case: Set corner values. */
		/* 022-171 set corner values. */
		// BASE CASE: SET CORNER VALUES (ONLY IF THEY AREN'T ALREADY SET).
		if ( (*heightMap)(0,0) == 0 )
		{
			unsigned char _rand = random.randomInt(255);
			(*heightMap)(0,0)=_rand;
			addToValueTable(heightTable,_rand);
		}
		if ( (*heightMap)(0,heightMap->nY-1) == 0 )
		{
			unsigned char _rand = random.randomInt(255);
			(*heightMap)(0,heightMap->nY-1)=_rand;
			addToValueTable(heightTable,_rand);
		}
		if ( (*heightMap)(heightMap->nX-1,0) == 0 )
		{
			unsigned char _rand = random.randomInt(255);
			(*heightMap)(heightMap->nX-1,0)=_rand;
			addToValueTable(heightTable,_rand);
		}
		if ( (*heightMap)(heightMap->nX-1,heightMap->nY-1) == 0 )
		{
			unsigned char _rand = random.randomInt(255);
			(*heightMap)(heightMap->nX-1,heightMap->nY-1)=_rand;
			addToValueTable(heightTable,_rand);
		}
		

		//int squareSize = heightMap->nX-1; /* 1023 */
		int squareSize = heightMap->nX-1; /* 1024 */
		
		
		int noise = 200;
		double smoothing = 0.7;
		
		/* Terminate when the square size is 1. */
		while(squareSize>1)
		{
			//std::cout<<"Squaresize: "<<squareSize<<".\n";
			for(int y=0;y<heightMap->nY;y+=squareSize)
			{
				/* Horizontal pass. */
			//	std::cout<<"Horizontal pass.\n";
				for(int x=0;x<heightMap->nX-squareSize;x+=squareSize)
				{
						// ONLY MODIFY 0 VALUES.
					//if ( (*heightMap)(x+(squareSize/2),y)==0 )
					//{
						//std::cout<<"MPA: ("<<x<<","<<y<<").\n";
						//sleep (20);
						
						/* Free step: The tile gets a random value to increase the fragmentation of the map. */
						if(freeSteps>0)
						{
							
							
							if ( (*heightMap)(x+(squareSize/2),y) != 0 )
							{
								const int rHeight = random.randomInt(255);
								(*heightMap)(x+(squareSize/2),y) = rHeight;
							}
							
							
							if(heightTable!=0)
							{ ++heightTable[(*heightMap)(x+(squareSize/2),y)]; }
							
						}
						else
						{
							// x axis midpoints...
							(*heightMap)(x+(squareSize/2),y) = ((*heightMap)(x,y) + (*heightMap)(x+squareSize,y))/2;
							//std::cout<<"Midpoint values: ("<<x<<","<<y<<") ("<<x+squareSize<<","<<y<<"). MP: ("<<x+(squareSize/2)<<","<<y<<")\n";
							
							//std::cout<<"P1: ("<<x<<","<<y<<") = "<<(int)(*heightMap)(x,y)<<". P2: ("<<x+squareSize<<","<<y<<") = "<<(int)heightMap->vIndex(x+squareSize,y)<<".\n";
							
							//std::cout<<"MP: ("<<x+(squareSize/2)<<","<<y<<") = "<<(int)*heightMap->index(x+(squareSize/2),y)<<".\n";
							//sleep(20);
						
							int variation;
							if ( BREAK_SMOOTHNESS>0 && squareSize>SQUARESIZE_LIMIT && random.randomInt(BREAK_SMOOTHNESS)==0)
							{ variation = (random.randomInt(noise)-(noise/2));
								variation*=random.randomInt(BROKEN_MULTIPLIER)+1;
							}
							else
							{ variation = (random.randomInt(noise)-(noise/2));
							}
							int finalValue = (*heightMap)(x+(squareSize/2),y)+variation;
							
							if(finalValue<0) { (*heightMap)(x+(squareSize/2),y)=0; if(heightTable!=0) { ++heightTable[0]; } }
							else if(finalValue>255) { (*heightMap)(x+(squareSize/2),y)=255; if(heightTable!=0) { ++heightTable[255]; } }
							else
							{
								if ( (*heightMap)(x+(squareSize/2),y) != 0 )
								{ (*heightMap)(x+(squareSize/2),y)=finalValue; }
								
								if(heightTable!=0)
								{ ++heightTable[(*heightMap)(x+(squareSize/2),y)]; }
							}
							
						}
					//}
				}
			}
			/* Vertical pass. */
			for(int y=0;y<heightMap->nY-squareSize;y+=squareSize)
			{
				for(int x=0;x<heightMap->nX;x+=(squareSize/2))
				{
				
						// ONLY MODIFY 0 VALUES.
					//if ( (*heightMap)(x,y+(squareSize/2))==0 )
					//{
				
						if(freeSteps>0)
						{
							const int rHeight = random.randomInt(255);
							(*heightMap)(x,y+(squareSize/2)) = rHeight;
							if(heightTable!=0)
							{ ++heightTable[rHeight]; }
						}
						else
						{
							int variation;
							if ( BREAK_SMOOTHNESS>0 && squareSize>SQUARESIZE_LIMIT && random.randomInt(BREAK_SMOOTHNESS)==0)
							{ variation = (random.randomInt(noise)-(noise/2));
								variation*=random.randomInt(BROKEN_MULTIPLIER)+1;
							}
							else
							{ variation = (random.randomInt(noise)-(noise/2));
							}
							(*heightMap)(x,y+(squareSize/2)) = ((*heightMap)(x,y) + (*heightMap)(x,y+squareSize))/2;
							//const int variation = (random.randomInt(noise)-(noise/2));
							int finalValue = (*heightMap)(x,y+(squareSize/2))+variation;
							if(finalValue<0) { (*heightMap)(x,y+(squareSize/2))=0; if(heightTable!=0) { ++heightTable[0]; } }
							else if(finalValue>255) { (*heightMap)(x,y+(squareSize/2))=255; if(heightTable!=0) { ++heightTable[255]; } }
							else
							{
								if ( (*heightMap)(x,y+(squareSize/2)) != 0 )
								{ (*heightMap)(x,y+(squareSize/2))=finalValue; }
								
								if(heightTable!=0)
								{ ++heightTable[(*heightMap)(x,y+(squareSize/2))]; }
							}
						}
					//}
				}
			}
			if(freeSteps>0) { --freeSteps; }
			squareSize/=2;
			noise*=smoothing;
		}
		/* Square size is 1. */

	}
};
