#pragma once
#ifndef WILDCAT_WORLDGENERATOR_WORLDGENERATOR_HPP
#define WILDCAT_WORLDGENERATOR_WORLDGENERATOR_HPP

/*
 * WorldGenerator.hpp
 * #include <WorldGenerator/WorldGenerator.hpp>
 *  Code for creating 2D maps.
 * 
 * 
 * 
 * 
 */

#include <Math/Fractal/MidpointDisplacement.hpp>
#include <Math/Random/GlobalRandom.hpp>
#include <Graphics/Png/Png.hpp>
#include <NameGen/NameGen.hpp>
#include <Container/ArrayS2/ArrayS2.hpp>
#include <Container/ArrayS3/ArrayS3.hpp>
#include <Container/Vector/Vector.hpp>

class WorldGenerator
{
	private:

		// NODE REPLACED WITH HASXY.
	//class Node
	//{
	//	public:
	//	int x,y;

	//	Node(const int _x, const int _y)
	//	{ x=_x; y=_y; }
	//};


	public:
		
	bool verbose2; // DEFAULT FALSE. WILL OUTPUT CONSOLE INFO.
	// verbose is taken..?

	int seed;
	double oceanPercent;
	double mountainPercent;
	int mapSize;
	int mapArea;

	int riversProbability;

	/* Heightmap array:
		0-255.
	*/

	ArrayS2 <unsigned char> heightMap;
	ArrayS2 <int> oceanID;
	ArrayS2 <int> swampID;
	ArrayS3 <unsigned char> topoMap;
	ArrayS3 <unsigned char> riverMap;
	ArrayS2 <int> riverID;
	ArrayS2 <unsigned char> aRainfall;
	ArrayS2 <unsigned char> aMountain;

	ArrayS3 <unsigned char> simpleTopo;


	/* 0223519793
		My new idea of outputting world data for games: Just output a char array, where a value corresponds with a particular terrain type.
	*/
	enum enumType { OCEAN, GRASSLAND, FOREST, DESERT, MOUNTAIN, SNOW };
	ArrayS2 <unsigned char> aTerrainType;

	int heightTable [256];

	int seaLevel;
	int mountainLevel;

	Png png;
	NameGen nameGenerator;
	Random random;

	WorldGenerator()
	{
			// seed 0 is random seed.
		seed=0;
		oceanPercent=0.6;
		mountainPercent=0.04;
			// length along each side.
		mapSize = 4097;
		mapArea = mapSize * mapSize;

		for ( int i=0;i<256;++i)
		{
			heightTable[i]=0;
		}

		seaLevel = 0;
		mountainLevel = 0;

		riversProbability=1000;
		
		verbose2=false;
	}

	void reset()
	{
		if ( verbose2==true )
		{
			std::cout<<"WorldGenerator::reset()\n";
		}
		
			// seed 0 is random seed.
		seed=0;
		oceanPercent=0.6;
		mountainPercent=0.04;
			// length along each side.
		mapSize = 4097;
		mapArea = mapSize * mapSize;

		for ( int i=0;i<256;++i)
		{
			heightTable[i]=0;
		}

		seaLevel = 0;
		mountainLevel = 0;

		riversProbability=1000;
	}

	
	//static void midpointDisplacement(ArrayS2 <unsigned char>* const heightMap, const int SEED=0,
	//const int BREAK_SMOOTHNESS=0, const int BROKEN_MULTIPLIER=0, const int SQUARESIZE_LIMIT=0, int freeSteps=0, int* heightTable=0)
	
	
	void generate(int _mapSize = 513, const int SEED=0, const int BREAK_SMOOTHNESS=0, const int BROKEN_MULTIPLIER=0, const int SQUARESIZE_LIMIT=0, int freeSteps=0)
	{
		if ( verbose2==true )
		{
			std::cout<<"WorldGenerator::generate("<<_mapSize<<")\n";
		}
		

		mapSize = _mapSize;
		mapArea = mapSize * mapSize;


		if(seed==0)
		{
			Random randSeed;
			seed = randSeed.randomInt(100000);
		}
		random.seed(seed);

		heightMap.init(mapSize,mapSize,0);
		oceanID.init(mapSize,mapSize,-1);
		swampID.init(mapSize,mapSize,0);
		topoMap.init(mapSize,mapSize,3,0);
		simpleTopo.init(mapSize,mapSize,3,0);
		riverMap.init(mapSize,mapSize,3,0);
		riverID.init(mapSize,mapSize,0);
		aRainfall.init(mapSize,mapSize,0);
		aMountain.init(mapSize,mapSize,0);

		MidpointDisplacement::midpointDisplacement(&heightMap,SEED,BREAK_SMOOTHNESS,BROKEN_MULTIPLIER,SQUARESIZE_LIMIT,freeSteps,heightTable);

		/* Narrow the range of values. */
		/* 022-171 New algo */
		//for(int y=0;y<heightMap.nY;++y)
		//{
		//	for(int x=0;x<heightMap.nX;++x)
		//	{ heightMap(x,y)=heightMap(x,y)-(heightMap(x,y)%10);
		//	}
		//}

		seaLevel = calculateSeaLevel();
		mountainLevel = calculateMountainLevel();

		smoothCoast(2);

		fillOceans();

			// Encode greyscale heightmap png.
		colourTopo();

		//generateRivers();
		//png.encodeS3("river-class.png",&riverMap);
		generateRainfall();
		generateMountains();

			// Encode coloured topographic map.


		//png.encodeS3("09class.png",&topoMap);
		//png.encodeS3("09class.png",&simpleTopo);
		//png.encodeS2("08-class.png",&heightMap);

	}


	unsigned char calculateSeaLevel()
	{
		double total=0;
		for(int i=0;i<256;++i)
		{
			total+=heightTable[i];
			/* check if we've reached the desired amount of ocean */
			if(total/(heightMap.nX*heightMap.nY) >= oceanPercent)
			{ return i; }
		}
		std::cout<<"ERROR: getSeaLevel() failed.\n";
		return 0;
	}
	unsigned char calculateMountainLevel ()
	{
		double total=0;
		for(int i=255;i>=0;--i)
		{
			total+=heightTable[i];
			/* check if we've reached the desired amount of mountain */
			if(total/(heightMap.nX*heightMap.nY) >= mountainPercent)
			{ return i; }
		}
		std::cout<<"ERROR: getMountainLevel() failed.\n";
		return 0;
	}

	void smoothCoast (const int smoothness=3)
	{
		/* 022-171 New algo. */
		/* BUGFIX: Old algo processed edges of map. The corners would always be land. */
		/* Don't process edges. */
		for(int y=1;y<heightMap.nY-1;++y)
		{
			for(int x=1;x<heightMap.nX-1;++x)
			{
				const unsigned char currentTile = heightMap(x,y);
				/* number of bordering water tiles */
				const unsigned char nSea = heightMap.nNeighboursLessThan(x,y, seaLevel+1);
				/* if this tile is land */
				if(currentTile>seaLevel)
				{ /* if there are too many water tiles around this tile, then smooth it */
					if(nSea>=(9-smoothness))
					{ heightMap(x,y)=seaLevel; }
				}
				/* if this tile is water */
				else
				{ /* if there are too many land tiles around this tile, then smooth it */
					if(nSea<=smoothness)
					{ heightMap(x,y)=seaLevel+1; }
				}

			}
		}
	}

	void fillOceans()
	{
		/* heightmap and oceanID must be the same size */
		if((heightMap.nX != oceanID.nX) || (heightMap.nY != oceanID.nY))
		{ std::cout<<"ERROR: Heightmap and oceanID need to be the same dimensions.\n"; return; }

		/* oceanID: -1 - uninitialized. 0 - no water. >0 - water. */
		int currentID = 1;

		//const int ARRAY_SIZE = oceanID.nX*oceanID.nY;
		/* for all tiles */
		for ( int pos = 0; pos < mapArea; ++pos)
		{
			/* we need to process this water */
			if ( oceanID(pos)==-1 && heightMap(pos)<=seaLevel )
			{
				/* vectors of coordinates to be checked */
				Vector <int> vX;
				Vector <int> vY;
				int currentV=-1;
				int posX, posY;
				heightMap.getIndexCoords(pos, &posX, &posY);
				/* scanline fill loop */
				while (true)
				{
					const int INITIAL_X=posX;
					/* go left to find the leftmost tile for this row which meets the criteria for water, filling along the way */
					/* we also check the tile directly above or below to see if there are any tiles that can be filled on those rows. */
					/* If we find such tiles, then we store the leftmost ones, for both the upper and lower row. */
					/* NOTE: y coords seems to encode upside-down, so we need to work upside-down here. */
					/* Fill left */
					while ( posX>=0 && heightMap(posX,posY)<=seaLevel )
					{
						oceanID(posX,posY)=currentID;
						/* check below */
						/* If we're not on the bottom row, and the below tile is water. */
						if(posY<heightMap.nY-1 && heightMap(posX,posY+1)<=seaLevel )
						{

							/* If we are at the leftmost or rightmost column of the map. */
							if(posX==0 || posX==heightMap.nX-1 )
							{
								/* push tile below us if it's valid. */
								if(oceanID(posX,posY+1)==-1)
								{ vX.push(posX); vY.push(posY+1); }
							}
							/* if we aren't at the leftmost or rightmost column of the map. */
							else
							{
								/* if the bottom-left tile is land, and the below tile has not been processed. */
								if ( heightMap(posX-1,posY+1)>seaLevel && oceanID(posX,posY+1)==-1 )
								{ vX.push(posX); vY.push(posY+1); }
							}
						}
						/* check above */
						/* If we're not on the top row, and the above tile is water. */
						if(posY>0 && heightMap(posX,posY-1)<=seaLevel )
						{

							/* If we are at the leftmost or rightmost column of the map. */
							if(posX==0 || posX==heightMap.nX-1 )
							{
								/* push tile above us if it's valid. */
								if(oceanID(posX,posY-1)==-1)
								{ vX.push(posX); vY.push(posY-1);
								}
							}
							/* if we aren't at the leftmost or rightmost column of the map. */
							else
							{
								/* if the top-left tile is land, and the above tile has not been processed. */
								if ( heightMap(posX-1,posY-1)>seaLevel && oceanID(posX,posY-1)==-1 )
								{ vX.push(posX); vY.push(posY-1); }
							}
						}
						--posX; /* move left 1 tile */
					}
					/* check the last X for this scan. */
					++posX;
					/* check below */
					/* If we're not on the bottom row, and the below tile is water, and the below tile hasn't been processed yet. */
					if(posY<heightMap.nY-1 && heightMap(posX,posY+1)<=seaLevel && oceanID(posX,posY+1)==-1 )
					{
						/* push coords onto vectors. */
						vX.push(posX); vY.push(posY+1);
					}
					/* check above */
					/* If we're not on the top row, and the above tile is water, and the above tile hasn't been processed yet. */
					if(posY>0 && heightMap(posX,posY-1)<=seaLevel && oceanID(posX,posY-1)==-1)
					{
						/* push coords onto vectors. */
						vX.push(posX); vY.push(posY-1);
					}
					posX=INITIAL_X+1;
					/* Go back to the initial x position, and now fill right */
					while ( posX < heightMap.nX && heightMap(posX,posY)<=seaLevel )
					{
						oceanID(posX,posY)=currentID;
						/* check below */
						/* If we're not on the bottom row, and the below tile is water. */
						if(posY<heightMap.nY-1 && heightMap(posX,posY+1)<=seaLevel )
						{
							/* If we are at the leftmost or rightmost column of the map. */
							if(posX==0 || posX==heightMap.nX-1 )
							{
								/* push tile below us if it's valid. */
								if(oceanID(posX,posY+1)==-1)
								{ vX.push(posX); vY.push(posY+1); }
							}
							/* if we aren't at the leftmost or rightmost column of the map. */
							else
							{
								/* if the bottom-left tile is land, and the below tile has not been processed. */
								if ( heightMap(posX-1,posY+1)>seaLevel && oceanID(posX,posY+1)==-1 )
								{ vX.push(posX); vY.push(posY+1); }
							}
						}
						/* check above */
						/* If we're not on the top row, and the above tile is water. */
						if(posY>0 && heightMap(posX,posY-1)<=seaLevel )
						{
							/* If we are at the leftmost or rightmost column of the map. */
							if(posX==0 || posX==heightMap.nX-1 )
							{
								/* push tile above us if it's valid. */
								if(oceanID(posX,posY-1)==-1)
								{ vX.push(posX); vY.push(posY-1);
								}
							}
							/* if we aren't at the leftmost or rightmost column of the map. */
							else
							{
								/* if the top-left tile is land, and the above tile has not been processed. */
								if ( heightMap(posX-1,posY-1)>seaLevel && oceanID(posX,posY-1)==-1 )
								{ vX.push(posX); vY.push(posY-1); }
							}
						}
						/* move right 1 tile */
						++posX;
					}
					/* entire row is complete. Go to next set of coords. */
					/* Look for some good coords on the vector. */
					++currentV;
					for (;currentV<vX.size();++currentV)
					{
						if ( oceanID(vX(currentV),vY(currentV)) == -1 )
						{
							posX=vX(currentV);
							posY=vY(currentV);
							break;
						}
					}
					//std::cout<<"\n";
					if(currentV==vX.size())
					{ break; }
				}
				vX.clear();
				vY.clear();
				++currentID;
			}
		}
	}

	void colourTopo()
	{
		for(unsigned int y=0;y<topoMap.nY;++y)
		{
			for(unsigned int x=0;x<topoMap.nX;++x)
			{
				if(oceanID(x,y) != -1) //Ocean
				{
					int depth = seaLevel-heightMap(x,y);
					int blueAmount = 80-depth;
					if(blueAmount<0){blueAmount=0;}

					topoMap(x,y,0)=0;
					topoMap(x,y,1)=0;
					topoMap(x,y,2)=blueAmount;

					simpleTopo(x,y,0)=0;
					simpleTopo(x,y,1)=0;
					simpleTopo(x,y,2)=150;


				}
				else
				{
					int height = (heightMap(x,y)-seaLevel)+40;
					int r=height-20;
					if(r<0) { r=0; }
					if (r>255) { r=255; }
					int g=r+20;
					if(g>255) { g=255; }
					int b=g/2;

					topoMap(x,y,0)=r;
					topoMap(x,y,1)=g;
					topoMap(x,y,2)=b;

					simpleTopo(x,y,0)=0;
					simpleTopo(x,y,1)=100;
					simpleTopo(x,y,2)=0;

				}

				if(swampID(x,y)==0) //Candidate for swamp (lies at or slightly below sealevel, may lie on ocean tiles.
				{ }
				if(swampID(x,y)==1) //Defined as swampland, using midpoint displacement.
				{
					topoMap(x,y,0)=0;
					topoMap(x,y,1)=255;
					topoMap(x,y,2)=0;
				}
				if(swampID(x,y)==2) //Defined as a beach.
				{
					topoMap(x,y,0)=255;
					topoMap(x,y,1)=255;
					topoMap(x,y,2)=0;
				}
			}
		}
	}
	
	
	void generateRivers() {}
#ifdef NOPE

	void generateRivers()
	{
		const int MAX_ITERATIONS = 1000;
		int currentIteration = 0;

		int currentRiver=1;

		for(unsigned int y=0;y<topoMap.nY;y+=3)
		{
			for(unsigned int x=0;x<topoMap.nX;x+=3)
			{
				if(heightMap(x,y)>mountainLevel && random.randomInt(riversProbability)==0 && riverID(x,y)==0)
				{
					currentIteration=0;

					//Okay, we can run a river.
					riverID(x,y)=currentRiver;

					riverMap(x,y,0)=0;
					riverMap(x,y,1)=0;
					riverMap(x,y,2)=255;

					topoMap(x,y,0)=0;
					topoMap(x,y,1)=255;
					topoMap(x,y,2)=255;


					Vector < HasXY > vNodes;
					Vector <HasXY*> vRiver;
					vRiver.push(new HasXY (x,y));
					heightMap.addNeighborNodes(x,y,&vNodes);


					while (currentIteration++ < MAX_ITERATIONS)
					{
						//std::cout<<".";
						//go through all the river tiles, and load their neigbors into a vector.

						//for(int i2=0;i2<vRiver.size();++i2)
						//{
						//	heightMap.addNeighborNodes(vRiver(i2)->x,vRiver(i2)->y,&vNodes);
						//}
						// go through all nodes, and find the lowest legitimate tile.
						int lowestHeight=300;
						int bestX=0;
						int bestY=0;
						for(int i2=0;i2<vNodes.size();++i2)
						{
							//heightMap.addNeighborNodes(vRiver(i2)->x,vRiver(i2)->y,&vNodes);
							int currentX=vNodes(i2)->x;
							int currentY=vNodes(i2)->y;

							if(heightMap(currentX,currentY)<lowestHeight && riverID(currentX,currentY)!=currentRiver)
							{
								bestX=vNodes(i2)->x;
								bestY=vNodes(i2)->y;
								lowestHeight = heightMap(bestX,bestY);
							}
						}
						//now we should have the best tile to put the next river tile.
						if(lowestHeight==300)
						{ std::cout<<"\nNo good tile found.\n"; break; }
						if(heightMap(bestX,bestY)<seaLevel) // no good tile found (can't imagine this happening...) || the river found the ocean.
						{ std::cout<<"\nRiver found ocean.\n"; break; }
						else if(riverID(bestX,bestY)!=0)
						{ std::cout<<"\nThe river merged with another river, or something messed up.\n"; break; }
						else
						{
							riverID(bestX,bestY)=currentRiver;
							vRiver.push(new Node (bestX,bestY) );

							int depth = seaLevel-heightMap(bestX,bestY);
							int blueAmount = 80-depth;
							if(blueAmount<0){blueAmount=0;}
							topoMap(bestX,bestY,0)=0;
							topoMap(bestX,bestY,1)=0;
							topoMap(bestX,bestY,2)=blueAmount;

							riverMap(bestX,bestY,0)=0;
							riverMap(bestX,bestY,1)=0;
							riverMap(bestX,bestY,2)=255;

							heightMap.addNeighborNodes(bestX,bestY,&vNodes);

						}
					}
					vNodes.clearPtr();

					++currentRiver;
					vRiver.clearPtr();
				}
			}
		}
		std::cout<<"Done. Total rivers: " <<currentRiver-1<<".\n";
	}
	
#endif	


	void generateRainfall() {}
	
#ifdef NOPE
	void generateRainfall()
	{
		MidpointDisplacement::midpointDisplacement(&aRainfall,0,5,3,10,2,0);



		// int bottomY1 = mapSize * 0.3;
		// int bottomY2 = mapSize * 0.4;
		// int bottomY3 = mapSize * 0.6;
		// int bottomY4 = mapSize * 0.7;

		// for(int y=0;y<heightMap.nY;++y)
		// {
			// for(int x=0;x<heightMap.nX;++x)
			// {
				// if ( ( y < bottomY2 && y > bottomY1 ) || ( y < bottomY4 && y > bottomY3 ) )
				// {
					// //aRainfall(x,y) *= 0.9;
				// }
			// }
		// }


		aRainfall.fillConditional("<",60,0);
		aRainfall.fillConditional(">",140,1);

		for(int y=0;y<heightMap.nY;++y)
		{
			for(int x=0;x<heightMap.nX;++x)
			{


				if(aRainfall(x,y)==0 && heightMap(x,y)>seaLevel && heightMap(x,y)<mountainLevel)
				{
					topoMap (x,y,0) = 150;
					topoMap (x,y,1) = 150;
					topoMap (x,y,2) = 0;

					simpleTopo(x,y,0)=150;
					simpleTopo(x,y,1)=150;
					simpleTopo(x,y,2)=0;

					int rgAvg = (topoMap(x,y,0)+topoMap(x,y,1))/2;
					int amountRed = rgAvg + 20;
					int amountGreen = rgAvg + 20;
					if(amountRed>255) { amountRed=255; }
					if(amountRed<0) { amountRed=0; }
					if(amountGreen>255) { amountGreen=255; }
					if(amountGreen<0) { amountGreen=0; }

					topoMap(x,y,0)=amountRed;
					topoMap(x,y,1)=amountGreen;
					//topoMap(x,y,2)=0;
				}
				else if(aRainfall(x,y)==1 && heightMap(x,y)>seaLevel && heightMap(x,y)<mountainLevel)
				{
					//int rgAvg = (topoMap(x,y,0)+topoMap(x,y,1))/2;
					//int amountRed = rgAvg + 20;
					int amountGreen = topoMap(x,y,1) + 40;
					//if(amountRed>255) { amountRed=255; }
					//if(amountRed<0) { amountRed=0; }
					if(amountGreen>255) { amountGreen=255; }
					if(amountGreen<0) { amountGreen=0; }

					//topoMap(x,y,0)=amountRed;
					topoMap(x,y,1)=amountGreen;
					//topoMap(x,y,2)=0;

					simpleTopo (x,y,0) = 0;
					simpleTopo (x,y,1) = 60;
					simpleTopo (x,y,2) = 0;

				}
			}
		}

	}
#endif

	void generateMountains()
	{
		MidpointDisplacement::midpointDisplacement(&aMountain,0,50,5,8,3,0);



		for(int y=0;y<heightMap.nY;++y)
		{
			for(int x=0;x<heightMap.nX;++x)
			{
				if (aMountain(x,y)>210)
				{
					aMountain(x,y)=1;

					topoMap (x,y,0) = 150;
					topoMap (x,y,1) = 150;
					topoMap (x,y,2) = 150;

					simpleTopo(x,y,0)=150;
					simpleTopo(x,y,1)=150;
					simpleTopo(x,y,2)=150;
				}
				else
				{
					aMountain(x,y)=0;
				}



			}
		}
	}

};

#endif
