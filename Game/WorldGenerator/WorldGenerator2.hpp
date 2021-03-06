#pragma once
#ifndef WILDCAT_GAME_WORLDGENERATOR_WORLDGENERATOR2_HPP
#define WILDCAT_GAME_WORLDGENERATOR_WORLDGENERATOR2_HPP

/*	Wildcat: WorldGenerator2
	#include <WorldGenerator/WorldGenerator2.hpp>
   
   Version 2 of the WorldGenerator. The old version used a range of values but this one just assigns
   types to each tile. It also uses Diamond Square instead of Midpoint Displacement.
   
   I want to update this so that it uses custom Biome objects. It should be possible to define
   biomes at runtime.
   
   The WorldGenerator doesn't model complex things like geology and weather, that's outside the
   scope of the project.

   TODO:
   Ensure biome percentages are accurate. I think biome layers are currently overlapping each other.
   
   Like with my DiamondSquare algorithm, it should be possible to specify a template world and then
   use the WorldGenerator to fill in details.

   PNG export should probably be handled outside of this header.
   
   I need to implment a way for different biomes to be related to each other. For example not allow
   desert biomes to touch snow biomes. Also allow biomes to be biased to certain longitude.
   
   Make sure the seed values are properly making reproducible maps. Biomes, landmass, etc should
   get their own subseeds too.

	Multithreading support.
	Bring back smoothness-breaking.
	Seperate natural features to their own layer.
	Potential feature: Flip the map so that the most land is at the north. Just a psychological thing.
	We could find the longitude line with the least land tiles over it and set that as the wrapping point.

*/

// These are hardcoded globals for now.
#include <Game/WorldGenerator/Biome.hpp>

#include <Math/Fractal/DiamondSquareAlgorithm.hpp>
#include <Math/Fractal/DiamondSquareAlgorithmCustomRange.hpp>
#include <Math/Random/GlobalRandom.hpp> // Random::
#include <Math/Random/RandomLehmer.hpp> // Faster RNG
#include <Graphics/Png/Png.hpp> // FOR PNG EXPORT.
#include <Container/ArrayS2/ArrayS2.hpp>
#include <Container/Vector/Vector.hpp>
#include <Math/BasicMath/BasicMath.hpp> // TO CHECK IF MAPSIZE IS POW2+1.
#include <File/FileManagerStatic.hpp> /* For saving the world data to file. */
#include <System/Time/Timer.hpp>

#include <thread>
#include <limits.h> /* For INT_MAX */

/*
New approach: Each tile has more than a single piece of data for it, therefore we should use an object for each tile.
*/
class WorldGenerator2_Tile
{
   public:
      int localSeed; //Seed used to generate local terrain.
      int terrainType;
      int evilLevel; //Evil places are more difficult. 0=peaceful, 255=Evil.
      int height; //Its average height.
      int nCaves; //The number of caves to spawn on this tile.

   WorldGenerator2_Tile()
   {
      localSeed=0;
      terrainType=0;
      evilLevel=0;
      height=0;
      nCaves=0;
   }
   ~WorldGenerator2_Tile()
   {
   }

   //Returns a string which contains all the tile data.
   std::string getFullSeed()
   {
      return "";
   }

};

// IMPLEMENTING SUPPORT FOR BIOMES SPECIFIED AT RUNTIME.
// DESIGN TO RUN MULTIPLE IN PARALLEL.
class WorldGenerator2_Biome
{
   public:
      // THE NAME OF THE BIOME, EG: DESERT.
      std::string name;
      int seed;
      double percentLand;
      int freeSteps;
      double smoothing;

   WorldGenerator2_Biome ( const std::string _name, const double _percentLand, const int _freeSteps, const double _smoothing, const int _seed = 0 )
   {
      name = _name;
      percentLand = _percentLand;
      freeSteps = _freeSteps;
      smoothing = _smoothing;
      seed = _seed;
   }

};

class WorldGenerator2
{
	/* Function list:
		void CreateLand(); Creates the basic landmasses.
	*/
	
	private:
      RandomLehmer rng;
      
	public:

	int mapSize;
	int mapArea;

	double oceanPercent;
	double mountainPercent;
	double forestPercent;
	double desertPercent;
	double snowPercent;

	double goodPercent;
	double evilPercent;
	
	// BASE SEED. THIS SEED CAN SET ALL OTHER SEEDS.
	int seed;
		// LANDFORM SEED. INFLUENCES THE BASIC LANDFORM.
		int landformSeed;
	
		// FRACTAL SETTINGS.
	int freeSteps;
	int variance;
	double landSmoothing;

	bool islandMode; // SETS EDGES AS WATER.
	bool landMode; // SETS EDGES AS LAND.

	//enum enumBiome { NOTHING=0, OCEAN=1, GRASSLAND=2, FOREST=3, DESERT=4, MOUNTAIN=5, SNOW=6, HILLY=7, JUNGLE=8, WETLAND=9, STEPPES=10, CAVE=11, RUIN=12, ICE=13, RIVER=14};
	//const std::string biomeName [15] = { "nothing", "ocean", "grassland", "forest", "desert", "mountain", "snow", "hilly", "jungle", "wetland", "steppes", "cave", "ruin", "ice", "river" };
	static std::string biomeName [15];
	static int N_BIOMES;
	// Initialised as:
	//std::string WorldGenerator2::biomeName [15] = { "nothing", "ocean", "grassland", "forest", "desert", "mountain", "snow", "hilly", "jungle", "wetland", "steppes", "cave", "ruin", "ice", "river" };
	//enum enumFeature { NOTHING=0, CAVE=1, PRECURSOR_RUIN=2 };
	
	ArrayS2 <WorldGenerator2_Tile> aTile;

	ArrayS2 <enumBiome> aTerrainType;
	ArrayS3 <unsigned char> aTopoMap;
	ArrayS2 <unsigned char> aHeightMap; /* This one is for landmasses. It should really be called aLandMassMap */
	ArrayS2 <unsigned char> aHeightMap2; /* This one is for elevations */
  
  ArrayS2 <int> aRiverMap; /* Contains the river ID */
	
	ArrayS2 <unsigned char> aTectonicMap;
	
	ArrayS2 <unsigned char> aCaveMap;

	ArrayS2 <int> aGoodEvilMap; // 0 = neutral, 1 = good, 2 = evil.

	//Vector <WorldGenerator2_Biome*> vBiome;

	//ArrayS2 <bool> aLand; // TRUE IF LAND, FALSE IS WATER.

	int heightTable [256];

	int totalLandTiles;
	int totalOceanTiles;
	
	bool wrapX;
	bool wrapY;
	
	unsigned char seaLevel;

WorldGenerator2()
{
   // seed is a core seed. If this is selected, it is used to create the other seeds.
   // seed 0 is random seed.
   seed=0;

   landformSeed=0;
   oceanPercent=0.5;
   mountainPercent=0.025;
   forestPercent=0.25;
   desertPercent=0.33;
   snowPercent=0.04;
   goodPercent=0.05;
   evilPercent=0.05;

   // length along each side.
   mapSize = 4096;
   mapArea = mapSize * mapSize;

   // I WOULD LIKE TO BRING THESE BACK, BECAUSE THEY CREATED INTERESTING CRATER-LIKE FEATURES.
   //breakSmoothness=0;
   //brokenMultiplier=0;
   //squareSizeLimit=0;

   //ORIGINAL VALUES
   freeSteps=4;
   variance=400;
   landSmoothing=0.86;

   //freeSteps=20;
   //variance=1;
   //landSmoothing=0.5;

   islandMode=false;
   landMode=false;

   totalLandTiles=0;
   totalOceanTiles=0;

   wrapX=false;
   wrapY=false;

   seaLevel=0;
}

void createLand(int _seed = 0)
{
   for ( int i=0;i<256;++i)
   { heightTable[i] = 0; }

   aHeightMap.init(mapSize,mapSize,0);
   totalLandTiles=0;
   totalOceanTiles=0;

   // FORCE ISLAND MAP.
   if ( islandMode == true )
   { aHeightMap.fillBorder(1); }
   // FORCE LAND MAP
   if ( landMode == true )
   { aHeightMap.fillBorder(255); }

   DiamondSquareAlgorithm dsa;
   landSmoothing=0.78;

   // HEIGHTMAP TABLE FREESTEPS SMOOTHING VARIANCE
   dsa.seed = _seed;
   dsa.wrapX = wrapX;
   dsa.wrapY = wrapY;

   dsa.generate(&aHeightMap, heightTable, freeSteps, landSmoothing, variance, 0);
   //exit(1);

   //unsigned char seaLevel=0;
   seaLevel=0;
   double total=0;
   for (int i=0; i<256; ++i)
   {
      total+=heightTable[i];
      if(total/mapArea >= oceanPercent)
      {
         seaLevel = i;
         break;
      }
   }

   for (int _y=0;_y<mapSize;++_y)
   {
      for (int _x=0;_x<mapSize;++_x)
      {
         if ( aHeightMap(_x,_y) > seaLevel )
         {
            aTerrainType(_x,_y) = GRASSLAND;
            aTile(_x,_y).terrainType=GRASSLAND;
            //Count the land tiles for later use.
            ++totalLandTiles;
         }
         else
         {
            aTerrainType(_x,_y) = OCEAN;
            aTile(_x,_y).terrainType = OCEAN;
            ++totalOceanTiles;
         }
      }
   }

   // TODO: OPTION TO REMOVE DIAGONAL LAND/SEA CONNECTIONS FOR BETTER GAMEPLAY.
}
	

void generateHeightmap()
{
   std::cout<<"Generating heightmap.\n";

   aHeightMap2.init(mapSize,mapSize,0);


   DiamondSquareAlgorithm dsa;
   landSmoothing=0.78;

   // HEIGHTMAP TABLE FREESTEPS SMOOTHING VARIANCE
   dsa.seed = landformSeed+1; /* Temporary */
   dsa.wrapX = wrapX;
   dsa.wrapY = wrapY;

   dsa.generate(&aHeightMap, 0, freeSteps, landSmoothing, variance, 0);



   for (int _y=0;_y<mapSize;++_y)
   {
      for (int _x=0;_x<mapSize;++_x)
      {
         if ( aHeightMap(_x,_y) > 250 )
         {
            //aTerrainType(_x,_y) = MOUNTAIN;
            //aTile(_x,_y).terrainType=MOUNTAIN;
            //Count the land tiles for later use.
            //++totalLandTiles;
         }
         else
         {
            // aTerrainType(_x,_y) = OCEAN;
            // aTile(_x,_y).terrainType = OCEAN;
            // ++totalOceanTiles;
         }
      }
   }

   // TODO: OPTION TO REMOVE DIAGONAL LAND/SEA CONNECTIONS FOR BETTER GAMEPLAY.
}

// Rivers flow from mountains and go downhill.
// Rivers can collect in areas to form freshwater lakes.
// Some rivers are major, and others are minor. Only major rivers should be shown on world view.
// Major rivers are basically those that can't be easily crossed.
void createRivers(int nRivers=1, int _seed=0)
{
   std::cout<<"Creating rivers.\n";
   std::cout<<"River seed: "<<_seed<<".\n";
   //Rivers always flow from mountain tiles.

   Vector <HasXY*> vMountainTiles;
   rng.seed(_seed);

   int nOceanTiles=0;
   for(int _y=0;_y<mapSize;++_y)
   {
      for(int _x=0;_x<mapSize;++_x)
      {
         if (aTerrainType(_x,_y)==MOUNTAIN)
         {
            vMountainTiles.push(new HasXY (_x,_y));
         }
      }
   }
   if (_seed == 0)
   {
      vMountainTiles.shuffle();
   }
   else
   {
      vMountainTiles.shuffle(_seed);
   }


   for (int i=0;i<nRivers && i<vMountainTiles.size();++i)
   {
      //aTerrainType(vMountainTiles(i)) = RIVER;
      aRiverMap(vMountainTiles(i)) = i;

      int currentX = vMountainTiles(i)->x;
      int currentY = vMountainTiles(i)->y;

      int maxRiverLength = 200;
      while (maxRiverLength-- > 0)
      {
         // Spread to lowest neighbor which isn't river. Abort when next to ocean.
         Vector <HasXY*>* vNeighbors = aTerrainType.getNeighborsOrthogonal(currentX,currentY,false);

         if ( vNeighbors == 0 ) { break; }

         //Shuffle vectors to randomly resolve equal height neighbors.
         if ( _seed==0)
         {
            vNeighbors->shuffle();
         }
         else
         {
            vNeighbors->shuffle(rng);
         }

         //int i2=0;
         int lowestHeight = 256;
         HasXY* lowestTile = 0;

         bool abortRiver = false;

         for (int i2=0;i2<vNeighbors->size();++i2)
         {
            // Abort when the river is touching an ocean.
            if (aTerrainType((*vNeighbors)(i2)) == OCEAN )
            {
               abortRiver=true;
               break;
            }
            // Abort if touching another river. (River will flow into this one)
            if ( aRiverMap((*vNeighbors)(i2)) != i && aRiverMap((*vNeighbors)(i2)) != -1)
            {
               abortRiver=true;
               break;
            }
         }
         if ( abortRiver )
         {
            break;
         }

         for (int i2=0;i2<vNeighbors->size();++i2)
         {
            if ( aRiverMap((*vNeighbors)(i2)) == -1 && aHeightMap((*vNeighbors)(i2)) < lowestHeight )
            {
               lowestTile = (*vNeighbors)(i2);
               lowestHeight = aHeightMap((*vNeighbors)(i2));
            }
         }
         if ( lowestTile != 0 )
         {
            //aTerrainType(lowestTile) = RIVER;
            aRiverMap(lowestTile) = i;
            currentX = lowestTile->x;
            currentY = lowestTile->y;
         }
         else
         {
            break;
         }


         for (int i3=0;i3<vNeighbors->size();++i3)
         {
            delete (*vNeighbors)(i3);
         }
         vNeighbors->clear();
         delete vNeighbors;
      }

   }

   for ( int i=0;i<vMountainTiles.size();++i)
   {
      delete vMountainTiles(i);
   }
   vMountainTiles.clear();
}

	
// Possible biome configuration options:
// Pass it a map to share with other biomes.
// Specify it to use the highest, lowest, or middle values.
void addBiome(const std::string _name, const double _percentLand, const int _freeSteps, const int _smoothing, const int _seed=0)
{
   //std::cout<<"Biome added: "<<_name<<".\n";
   //vBiome.push(new WorldGenerator2_Biome(_name, _percentLand, _freeSteps, _smoothing, _seed));
}

void randomiseBiomes()
{
   oceanPercent=(double)rng.rand8(100) / 100;
   std::cout<<"oceanPercent "<<oceanPercent<<".\n";
   mountainPercent=(double)rng.rand8(100) / 100;
   forestPercent=(double)rng.rand8(100) / 100;
   std::cout<<"forestPercent "<<forestPercent<<".\n";
   desertPercent=(double)rng.rand8(100) / 100;
   std::cout<<"desertPercent "<<desertPercent<<".\n";
   snowPercent=(double)rng.rand8(100) / 100;
   std::cout<<"snowPercent "<<snowPercent<<".\n";
}

		// NEED TO IMPLEMENT FALSE RETURN IF WORLD CAN'T BE GENERATED.
	bool generate()
	{
		std::cout<<"WorldGenerator::generate(). Seed is "<<seed<<".\n";

		if ( mapSize <= 0 )
		{
			std::cout<<"WARNING: mapSize must be greater than 0.\n";
			std::cout<<"Setting mapSize to 1.\n";
			mapSize=1;
		}
		if ( BasicMath::isPowerOfTwo(mapSize-1) == false )
		{
			std::cout<<"WARNING: World size must be 2^x + 1.\n";
			mapSize = BasicMath::roundUpPowerOfTwo(mapSize)+1;
			std::cout<<"I rounded it up to "<<mapSize<<" for you.\n";
		}

		mapArea = mapSize * mapSize;

		// WE CAN'T MODIFY THE CLASS SEED VALUE, BECAUSE WE NEED TO REMEMBER IT LATER.
		if ( seed==0 )
		{
				// Randomly generate the base seed.
			seed = Random::randomInt(INT_MAX);
			rng.seed(seed);
		}
		else
		{
			rng.seed(seed);
		}
		
			// Derive the sub seeds from the main seed.
			// There's a bunch here so I can pull more later without
			// changing the RNG too much over time.
		int subSeed [100];
		for (int& s : subSeed)
		{
			s = rng.rand32(INT_MAX);
		}
		

		landformSeed = subSeed[0];
		

		//std::cout<<"Base seed: "<<seed<<".\n";
		//std::cout<<"Landform seed: "<<landformSeed<<".\n";

		// MAKE DEFAULT TILE OCEAN.
		aTerrainType.init(mapSize,mapSize,OCEAN);
		aRiverMap.init(mapSize,mapSize,-1);
		
		WorldGenerator2_Tile nullTile;
    delete [] aTile.data;
		aTile.init(mapSize,mapSize,nullTile);
		

		
			// For some reason, desert is special, with
			//dsa.generate(&aBiomeMap,0,0,_smoothing,80);
			// everything else is:
			// dsa.generate(&aBiomeMap,0,_freeSteps,_smoothing,200);
			
			
			// if (_biomeName=="Desert")
		// {		dsa.generate(&aBiomeMap,0,0,_smoothing,80);
		// }
		// else
		// {		dsa.generate(&aBiomeMap,0,_freeSteps,_smoothing,200);
		// }
		

		std::cout<<"Creating landmasses with seed: "<<landformSeed<<".\n";
		createLand(subSeed[0]);

		std::cout<<"Eroding coast.\n";
		erodeCoast(2);
		
		
		//std::thread * t[8];
		// GENERATE BIOMES HERE IN PARALLEL
			// GENERATE HEIGHTMAP
		//DiamondSquareAlgorithm dsa [8];
		
		// for (int i=0;i<8;++i)
		// {
			// //dsa[i].wrapX = wrapX;
			// //dsa[i].wrapY = wrapY;
			// //dsa[i].seed = subSeed[i+1];
			
			// //td::thread t (&WorldGenerator2::test, this);
			
			// //t[i] (&WorldGenerator2::test, &this);
			// //t[i] (&WorldGenerator2::test, &this->test, 1);
			// //t[i] (&ArrayS2 <unsigned char>::init, &this->aHeightMap, x,y,0);
		// }
		// //ArrayS2 <unsigned char> aBiomeMap;
		// //aBiomeMap.init(mapSize,mapSize,0);
		
		// //unsigned char biomeTable [256] = {0};
		// //int * const valueTableNull = 0;
		// //std::thread t1 (&DiamondSquareAlgorithm::generate, &dsa[0],&aBiomeMap,valueTableNull,0,0,200,0.01,false);
		// //std::thread t2 (&DiamondSquareAlgorithm::generate, &dsa[0],&aBiomeMap,valueTableNull,0,0,200,0.01,false);
		// //std::thread t3 (&DiamondSquareAlgorithm::generate, &dsa[0],&aBiomeMap,valueTableNull,0,0,200,0.01,false);
		
		
		
		// //std::thread t1 (&DiamondSquareAlgorithm::generate, &dsa[0]);
		// //std::thread t1 (&DiamondSquareAlgorithm::test, &dsa[0]);
		// //std::thread t1 (&DiamondSquareAlgorithm::test2, &dsa[0], &aBiomeMap, valueTableNull);
		
		// for (int i=0;i<8;++i)
		// {
		// //	t[i].join();
		// }
		
		//int biomeTable [256] = {0};
		
		
		//std::thread t1 (&ArrayS2 <unsigned char>::init, &this->aHeightMap, x,y,0);
		
		//std::thread t1 (&ArrayS2 <unsigned char>::init, &this->aHeightMap, x,y,0);

		
		//void createBiome (const enumBiome BIOME_TYPE, const double biomePercent, const int _freeSteps=0, const double _smoothing=0.85, const std::string _biomeName="?", ArrayS2 <unsigned char> * biomeMap=0, int _seed=0)
		
		
		//void createBiome (const enumBiome BIOME_TYPE, const double biomePercent, const int _freeSteps=0, const double _smoothing=0.85, const std::string _biomeName="?", ArrayS2 <unsigned char> * biomeMap=0, int _seed=0)
		
		Timer timerBiome;
		timerBiome.init();
		timerBiome.start();
		
		// This section benefits greatly from multithreading.
		//Biome creation time reduced from 3.2 seconds to 0.8 on my quad core.
    // Unfortunately the biomes are not merging in the correct order,
    // so that will need to be fixed. We will probably want to define a heirarchy of
    // biomes which can overwrite other biomes.
		

   //#define THREADED_WORLDGEN
   #if defined THREADED_WORLDGEN

    std::thread t1( [this, subSeed] { createBiome(JUNGLE, 0.33, 4, 0.78, "Jungle", 0 ,subSeed[1]); });
    std::thread t2( [this, subSeed] { createBiome(FOREST, 0.5, 8, 0.8, "Forest", 0 ,subSeed[2]); });
    std::thread t3( [this, subSeed] { createBiome(WETLAND, 0.05, 11, 0.79, "Wetland", 0 ,subSeed[3]); });
    std::thread t4( [this, subSeed] { createBiome(STEPPES, 0.05, 2, 0.77, "Steppes", 0 ,subSeed[4]); });
    std::thread t5( [this, subSeed] { createBiome(SNOW, 0.25, 2, 0.76, "Tundra", 0 ,subSeed[5]); });
    std::thread t6( [this, subSeed] { createBiome(DESERT, 0.11, 1, 0.8, "Desert", 0 ,subSeed[6]); });
    std::thread t7( [this, subSeed] { createBiome(HILLY, 0.05, 8, 0.8, "Hills", 0 ,subSeed[7]); });
    std::thread t8( [this, subSeed] { createBiome(MOUNTAIN, 0.07, 13, 0.78, "Mountains", 0 ,subSeed[8]); });

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
		
  #else
    
    createBiome (JUNGLE, 0.33, 4, 0.78, "Jungle", 0, subSeed[1]);
    createBiome (FOREST, 0.5, 8, 0.8, "Forest", 0, subSeed[2]);
    createBiome (WETLAND, 0.05, 11, 0.79, "Wetland", 0, subSeed[3]);
    createBiome (STEPPES, 0.05, 2, 0.77, "Steppes", 0, subSeed[4]);
    createBiome (SNOW, 0.25, 2, 0.76, "Tundra", 0, subSeed[5]);
    createBiome (DESERT, 0.11, 1, 0.8, "Desert", 0, subSeed[6]);
    createBiome (HILLY, 0.05, 8, 0.8, "Hills", 0, subSeed[7]);
    createBiome (MOUNTAIN, 0.07, 13, 0.78, "Mountains", 0, subSeed[8]);
			
  #endif

		timerBiome.update();
		std::cout<<"Biomes created in "<<timerBiome.fullSeconds<<" seconds.\n";

			// Place caves. (somehow layer this over base biome)
			
			// Freesteps set high enough basically makes static.
		//createBiome (CAVE, 0.01, 100, 0.86, "Cave");
		//createBiomePrecise (RUIN, 0.0001, 100, 0.86, "Ruin");
		
    
		Timer timerRiver;
		timerRiver.init();
		timerRiver.start();
		createRivers(50, subSeed[11]);
		timerRiver.update();
		std::cout<<"Rivers created in "<<timerRiver.fullSeconds<<" seconds.\n";

		std::cout<<"Creating ice caps.\n";
		createIceCaps(true, subSeed[10]);
		//std::cout<<"Creating good/evil zones.\n";
		createGoodEvil();
		// createFeatures();
		//std::cout<<"Creating world info.\n";
		//createWorldInfo();
		
		
		
		// Build a topographical map. This should be ported because it should be build within the viewing app.
		//buildTopoMap();
		

		return false;
	}
	
		// This should be done externally.
	void buildTopoMap()
	{
		aTopoMap.init(mapSize,mapSize,3,255);
		
// Create a lightening darkening texture fractal.
		ArrayS2 <int> aLightModifier2 (mapSize,mapSize,0);
		//aLightModifier2.init(mapSize,mapSize,0);

			// GENERATE HEIGHTMAP
		DiamondSquareAlgorithmCustomRange dsa2;
		dsa2.maxValue=12;
		//HEIGHTMAP TABLE FREESTEPS SMOOTHING
		dsa2.generate(&aLightModifier2,0,2,0.75,250);

		for (int _y=0;_y<mapSize;++_y)
		{
			for (int _x=0;_x<mapSize;++_x)
			{
				const int lightModifier = aLightModifier2(_x,_y);
				//const int lightModifier = 0;
				
				int _red;
				int _green;
				int _blue;
				
				if ( aTerrainType(_x,_y) == OCEAN )
				{
					_red=50;
					_green=50;
					_blue=240;
				}
				else if ( aTerrainType(_x,_y) == RIVER )
				{
					 _red=100;
					 _green=100;
					 _blue=240;
					// _red=255;
					// _green=0;
					// _blue=0;
				}
				else if ( aTerrainType(_x,_y) == MOUNTAIN )
				{
					_red=80;
					_green=80;
					_blue=80;
				}
				else if ( aTerrainType(_x,_y) == HILLY )
				{
					_red=80;
					_green=140;
					_blue=80;
				}
				else if ( aTerrainType(_x,_y) == FOREST )
				{
					_red=0;
					_green=120;
					_blue=0;
				}
				else if ( aTerrainType(_x,_y) == DESERT )
				{
					_red=200;
					_green=200;
					_blue=20;
				}
				else if ( aTerrainType(_x,_y) == SNOW )
				{
					_red=220;
					_green=220;
					_blue=240;
				}
				else if ( aTerrainType(_x,_y) == JUNGLE )
				{
					_red=0;
					_green=70;
					_blue=0;
				}
				else if ( aTerrainType(_x,_y) == WETLAND )
				{
					_red=20;
					_green=150;
					_blue=200;
				}
				else if ( aTerrainType(_x,_y) == STEPPES )
				{
					_red=180;
					_green=120;
					_blue=40;
				}
				else if ( aTerrainType(_x,_y) == ICE )
				{
					_red=255;
					_green=255;
					_blue=255;
				}
				else if ( aTerrainType(_x,_y) == CAVE )
				{
					_red=0;
					_green=0;
					_blue=0;
				}
				else if ( aTerrainType(_x,_y) == RUIN )
				{
					_red=255;
					_green=0;
					_blue=255;
				}
				else
				{
					_red=66;
					_green=180;
					_blue=66;
				}
				
				//mof light values.
				// _red += lightModifier;
				// if ( _red > 255 ) { _red = 255; }

				// _green += lightModifier;
				// if ( _green > 255 ) { _green = 255; }
				
				// _blue += lightModifier;
				//if ( _blue > 255 ) { _blue = 255; }
				
				_red -= lightModifier;
				if ( _red < 0 ) { _red = 0; }

				_green -= lightModifier;
				if ( _green < 0 ) { _green = 0; }
				
				_blue -= lightModifier;
				if ( _blue < 0 ) { _blue = 0; }
				
				
				aTopoMap(_x,_y,0) = _red;
				aTopoMap(_x,_y,1) = _green;
				aTopoMap(_x,_y,2) = _blue;
			}
		}
	}
	
		// WE MAY NEED TO CONSIDER WRAPPING IN ARRAY CHECKS, HOWEVER THE QUICK AND DIRTY NORMAL METHOD SEEMS TO BE WORKING OKAY FOR NOW.
	void erodeCoast(const int iterations = 1)
	{
		for (int i=0;i<iterations;++i)
		{
			ArrayS2 <bool> aChangeTile (mapSize,mapSize,false);
			ArrayS2 <enumBiome> aChangeTo (mapSize,mapSize,OCEAN);

			// The edges aren't smoothed because they don't have a 'normal' amount of neighboring tiles.
			for(int y=1;y<mapSize-1;++y)
			{
				for(int x=1;x<mapSize-1;++x)
				{
					// REMOVE JAGGED COASTAL FEATURES.
					if ( aTerrainType(x,y) == GRASSLAND )
					{
						const unsigned char nOcean = aTerrainType.nNeighborsEqual(x,y,OCEAN);
						if (nOcean >= 5)
						{
							aChangeTile(x,y) = true;
							aChangeTo(x,y) = OCEAN;
						}
					}
					// REMOVE SMALL OCEANS
					if ( aTerrainType(x,y) == OCEAN )
					{
						const unsigned char nOcean = aTerrainType.nNeighborsEqual(x,y,OCEAN);
						if (nOcean <= 1)
						{
							aChangeTile(x,y) = true;
							aChangeTo(x,y) = GRASSLAND;
						}
					}
				}
			}

			for(int _y=1;_y<mapSize-1;++_y)
			{
				for(int _x=1;_x<mapSize-1;++_x)
				{
					if (aChangeTile(_x,_y)==true)
					{
						aTerrainType(_x,_y) = aChangeTo(_x,_y);
					}
				}
			}
		}
	}

	void createGoodEvil()
	{
		const int MAX_VALUE = 1000;

		int goodEvilTable [MAX_VALUE+1];
		for ( int i=0;i<=MAX_VALUE;++i)
		{ goodEvilTable[i] = 0; }

		aGoodEvilMap.init(mapSize,mapSize,0);

			// GENERATE HEIGHTMAP
		//DiamondSquareAlgorithm dsa;
		DiamondSquareAlgorithmCustomRange dsa;
		dsa.maxValue = MAX_VALUE;
		//HEIGHTMAP TABLE FREESTEPS SMOOTHING
		//dsa.wrapX = wrapX;
		//dsa.wrapY = wrapY;
		dsa.generate(&aGoodEvilMap,0,0,0.76,200,0.1);

			// MANUALLY BUILD THE BIOME TABLE (WE NEED TO ONLY COUNT LAND TILES).
		int countedLandTiles=0;
		for (int _y=0;_y<mapSize;++_y)
		{
			for (int _x=0;_x<mapSize;++_x)
			{
				if ( aTerrainType(_x,_y) != OCEAN )
				{
					goodEvilTable [ aGoodEvilMap(_x,_y) ] ++;
					++countedLandTiles;
				}
			}
		}
	
		//std::cout<<"Counted land tiles: "<<countedLandTiles<<".\n";
		//std::cout<<"Desired good percent: "<<goodPercent<<".\n";
		//std::cout<<"Desired bad percent: "<<evilPercent<<".\n";

		// Must take into account sealevel.

		int goodThreshold=0;
		int total=0;
		for (int i=0; i<=MAX_VALUE; ++i)
		{
			total+=goodEvilTable[i];
			goodThreshold = i;

			//std::cout<<"Current good percent: "<<(double)total/countedLandTiles <<".\n";


				// I CHANGED IT SO THAT PERCENTAGES ARE RELATIVE TO LAND, RATHER THAN THE ENTIRE MAP.
			if ((double)total/countedLandTiles >= goodPercent)
			{ break; }
		}

			// WORK THE BAD THRESHOLD IN REVERSE
		int evilThreshold=0;
		int total2=0;
		for (int i=MAX_VALUE; i>=0; --i)
		{
			total2+=goodEvilTable[i];
			evilThreshold = i;

			//std::cout<<"Current evil percent: "<<(double)total2/countedLandTiles <<".\n";


				// I CHANGED IT SO THAT PERCENTAGES ARE RELATIVE TO LAND, RATHER THAN THE ENTIRE MAP.
			if ((double)total2/countedLandTiles >= evilPercent)
			{ break; }
		}





		//std::cout<<"Total tiles: "<<aTerrainType.nX*aTerrainType.nY<<"\n";
		//std::cout<<"Percent: "<<(double)total/countedLandTiles<<"\n";
		//std::cout<<"Total land tiles: "<<countedLandTiles<<"\n";
		//std::cout<<"Total good tiles: "<<total<<"\n";
		//std::cout<<"Total evil tiles: "<<total2<<"\n";
		//std::cout<<"Good threshold: "<<(int)goodThreshold<<"\n";
		//std::cout<<"Evil threshold: "<<(int)evilThreshold<<"\n";
		int nnn=0;


			// MERGE DOWN THE BIOME LAYER
		for (int _y=0;_y<mapSize;++_y)
		{
			for (int _x=0;_x<mapSize;++_x)
			{
				if ( aGoodEvilMap(_x,_y) <= goodThreshold && aTerrainType(_x,_y) != OCEAN )
				{
					nnn++;
					aGoodEvilMap(_x,_y) = 1;
				}
			}
		}
		//std::cout<<"# good tiles: "<<nnn<<"\n";


		int nnn2=0;
			// MERGE DOWN THE BIOME LAYER
		for (int _y=0;_y<mapSize;++_y)
		{
			for (int _x=0;_x<mapSize;++_x)
			{
				if ( aGoodEvilMap(_x,_y) >= evilThreshold && aTerrainType(_x,_y) != OCEAN )
				{
					nnn2++;
					aGoodEvilMap(_x,_y) = 2;
				}
			}
		}
		//std::cout<<"# evil tiles: "<<nnn2<<"\n";

	}
	

	void createIceCaps(bool overrideAll = true, int _seed = 0)
	{
		//double biomePercent = 0.05;
		double biomePercent = 0.035;
		
		//std::cout<<"Creating ice caps.\n";
		int biomeTable [256];
		for ( int i=0;i<256;++i)
		{ biomeTable[i] = 0; }

		ArrayS2 <unsigned char> aBiomeMap;
		aBiomeMap.init(mapSize,mapSize,0);
		
		for ( int _x=0;_x<aBiomeMap.nX;++_x)
		{
			aBiomeMap(_x,0)=1;
			aBiomeMap(_x,aBiomeMap.nY-1)=1;
			
			//NO ICE NEAR EQUATOR PLS.
			aBiomeMap(_x,aBiomeMap.nY/2)=255;
		}

			// GENERATE HEIGHTMAP
		DiamondSquareAlgorithm dsa;
		dsa.wrapX = wrapX;
		dsa.wrapY = wrapY;
		dsa.seed = _seed;
		
		//HEIGHTMAP TABLE FREESTEPS SMOOTHING
		dsa.generate(&aBiomeMap,0,0,0.8,100);
		
		for ( int _x=0;_x<aBiomeMap.nX;++_x)
		{
			aBiomeMap(_x,0)=1;
			aBiomeMap(_x,1)=1;
			aBiomeMap(_x,aBiomeMap.nY-1)=1;
			aBiomeMap(_x,aBiomeMap.nY-2)=1;
		}

			// MANUALLY BUILD THE BIOME TABLE (WE NEED TO ONLY COUNT LAND TILES).
		int countedLandTiles=0;
		for (int _y=0;_y<mapSize;++_y)
		{
			for (int _x=0;_x<mapSize;++_x)
			{
				//if ( aTerrainType(_x,_y) != OCEAN )
				{
					biomeTable [ aBiomeMap(_x,_y) ] ++;
					++countedLandTiles;
				}
			}
		}

		//std::cout<<"Counted land tiles: "<<countedLandTiles<<".\n";
		//std::cout<<"Desired biome percent: "<<biomePercent<<".\n";

		// Must take into account sealevel.

		unsigned char biomeThreshold=0;
		int total=0;
		for (int i=0; i<256; ++i)
		{
			total+=biomeTable[i];
			biomeThreshold = i;

			//std::cout<<"Current biome percent: "<<(double)total/(mapSize*mapSize) <<".\n";


				// I CHANGED IT SO THAT PERCENTAGES ARE RELATIVE TO LAND, RATHER THAN THE ENTIRE MAP.
			if ((double)total/(mapSize*mapSize) >= biomePercent)
			{ break; }
		}
		//std::cout<<"Total tiles: "<<aTerrainType.nX*aTerrainType.nY<<"\n";
		//std::cout<<"Percent: "<<(double)total/countedLandTiles<<"\n";
		//std::cout<<"Total land tiles: "<<countedLandTiles<<"\n";
		//std::cout<<"Total biome tiles: "<<total<<"\n";
		//std::cout<<"Biome threshold: "<<(int)biomeThreshold<<"\n";
		int nnn=0;

			// MERGE DOWN THE BIOME LAYER
			
		for (int _y=0;_y<mapSize;++_y)
		{
			for (int _x=0;_x<mapSize;++_x)
			{
					// NEW: ONLY PUT ICE OVER THE WATER, SINCE ICE REPRESENTS FLOATING ICE.
				if ( overrideAll==false && aBiomeMap(_x,_y) <= biomeThreshold  && aTerrainType(_x,_y) == OCEAN )
				{
					nnn++;
					aTerrainType(_x,_y) = ICE;
				}
				else if (overrideAll==true && aBiomeMap(_x,_y) <= biomeThreshold )
				{
					nnn++;
					aTerrainType(_x,_y) = ICE;
				}
			}
		}
	//	std::cout<<"Final biome tiles: "<<nnn<<"\n";
		
		//std::cout<<"Debug biome table:\n";
		
		//for ( int i=0;i<256;++i)
		//{
		//std::cout<<"["<<i<<"] : "<<biomeTable[i]<<".\n";
		//}
	}

		// CREATE A BIOME
		// PERCENTBIOME IS RELATIVE TO LAND MASS. SO 50% WILL MEAN 50% OF THE LAND IS COVERED IN IT.
		// BIOMES CAN ONLY OVERWRITE GRASSLAND.
		// I WILL NEED TO ADD ADDITIONAL FUNCTION PARAMETERS... SMOOTHING, JAGGEDNESS ETC.

		// OKAY WE HAVE A PROBLEM. ONLY GRASSLAND IS BEING COUNTED WHEN FIGURING OUT WHAT PERCENT OF THE WORLD SHOULD BE FILLED WITH A BIOME.
	void createBiomePrecise (const enumBiome BIOME_TYPE, const double biomePercent, const int _freeSteps=0, const double _smoothing=0.85, const std::string _biomeName="?")
	{
		const int MAX_VALUE = 1000;
		
		std::cout<<"Creating biome precise: "<<_biomeName<<".\n";
		int biomeTable [MAX_VALUE+1];
		for ( int i=0;i<=MAX_VALUE;++i)
		{ biomeTable[i] = 0; }

		ArrayS2 <int> aBiomeMap;
		//ArrayS2 <unsigned char> aBiomeMap;
		aBiomeMap.init(mapSize,mapSize,0);

			// GENERATE HEIGHTMAP
		//DiamondSquareAlgorithm dsa;
		DiamondSquareAlgorithmCustomRange dsa;
		dsa.maxValue=MAX_VALUE;
		//HEIGHTMAP TABLE FREESTEPS SMOOTHING
		dsa.generate(&aBiomeMap,0,_freeSteps,_smoothing, MAX_VALUE/2);
		//dsa.generate(&aBiomeMap,0,_freeSteps,_smoothing,1);

			// MANUALLY BUILD THE BIOME TABLE (WE NEED TO ONLY COUNT LAND TILES).
		int countedLandTiles=0;
		for (int _y=0;_y<mapSize;++_y)
		{
			for (int _x=0;_x<mapSize;++_x)
			{
				if ( aTerrainType(_x,_y) != OCEAN )
				{
					biomeTable [ aBiomeMap(_x,_y) ] ++;
					++countedLandTiles;
				}
			}
		}

		//std::cout<<"Counted land tiles: "<<countedLandTiles<<".\n";
		//std::cout<<"Desired biome percent: "<<biomePercent<<".\n";
		
		//std::cout<<"Debug biome table:\n";
		
		for ( int i=0;i<=MAX_VALUE;++i)
		{
		//std::cout<<"["<<i<<"] : "<<biomeTable[i]<<".\n";
		}

		// Must take into account sealevel.

		int biomeThreshold=0;
		int total=0;
		for (int i=0; i<=MAX_VALUE; ++i)
		{
			total+=biomeTable[i];
			biomeThreshold = i;

			//std::cout<<"Current biome percent: "<<(double)total/countedLandTiles <<".\n";


				// I CHANGED IT SO THAT PERCENTAGES ARE RELATIVE TO LAND, RATHER THAN THE ENTIRE MAP.
			if ((double)total/countedLandTiles >= biomePercent)
			{ break; }
		}
		//std::cout<<"Total tiles: "<<aTerrainType.nX*aTerrainType.nY<<"\n";
		//std::cout<<"Percent: "<<(double)total/countedLandTiles<<"\n";
		//std::cout<<"Total land tiles: "<<countedLandTiles<<"\n";
		//std::cout<<"Total biome tiles: "<<total<<"\n";
		//std::cout<<"Biome threshold: "<<(int)biomeThreshold<<"\n";
		int nnn=0;

			// MERGE DOWN THE BIOME LAYER
		for (int _y=0;_y<mapSize;++_y)
		{
			for (int _x=0;_x<mapSize;++_x)
			{
				if ( aBiomeMap(_x,_y) <= biomeThreshold && aTerrainType(_x,_y) != OCEAN )
				{
					nnn++;
					aTerrainType(_x,_y) = BIOME_TYPE;
				}
			}
		}
		std::cout<<"Final biome tiles: "<<nnn<<"\n";
	}
		
		/* 0272144293 NEW FEATURE: Can pass a pre-existing map for the biome generator to use. */
	void createBiome (const enumBiome BIOME_TYPE, const double biomePercent, const int _freeSteps=0, const double _smoothing=0.85, const std::string _biomeName="?", ArrayS2 <unsigned char> * const biomeMap=0, const int _seed=0)
	{
		//std::lock_guard<std::mutex> guard(mutexArrayAccess);
		//std::lock_guard<std::mutex> guard2(render_mutex);
		
		std::cout<<"Creating biome: "<<_biomeName<<".\n";
		//int biomeTable [256] = {0};
		int biomeTable [256] = {0};
		//for ( int i=0;i<256;++i)
		//{ biomeTable[i] = 0; }

		ArrayS2 <unsigned char> aBiomeMap (mapSize,mapSize,0);
		//aBiomeMap.init(mapSize,mapSize,0);
		

		
		
		// DESERT TEST
		if (_biomeName=="Desert")
		{
			for ( int _x=0;_x<aBiomeMap.nX;++_x)
			{
					//std::cout<<"Moving desert\n";
					// DESERT GOES AT BOTTOM THIRD AND TOP THIRD.
					
					// NO DESERT NEAR POLES
				aBiomeMap(_x,0)=255;
				aBiomeMap(_x,aBiomeMap.nY-1)=255;
					// NO DESERT ON EQUATOR
				aBiomeMap(_x,aBiomeMap.nY/2)=255;
				
				int third = aBiomeMap.nY/3;
				//aBiomeMap(_x,third)=0;
				//aBiomeMap(_x,third*2)=1;
			}
		}
			// JUNGLE AT EQUATOR.
		else if (_biomeName=="Jungle")
		{
			for ( int _x=0;_x<aBiomeMap.nX;++_x)
			{
					//std::cout<<"Moving jungle\n";
					// DESERT GOES AT BOTTOM THIRD AND TOP THIRD.
					
					// NO DESERT NEAR POLES
				aBiomeMap(_x,0)=255;
				aBiomeMap(_x,aBiomeMap.nY-1)=255;
					// NO DESERT ON EQUATOR
				aBiomeMap(_x,aBiomeMap.nY/2)=1;
				
				//int third = aBiomeMap.nY/3;
				//aBiomeMap(_x,third)=0;
				//aBiomeMap(_x,third*2)=1;
			}
		}
		else if (_biomeName=="Tundra")
		{
			//std::cout<<"SNOW\n";
			for ( int _x=0;_x<aBiomeMap.nX;++_x)
			{
					//std::cout<<"Moving snow\n";
					// DESERT GOES AT BOTTOM THIRD AND TOP THIRD.
					
					// NO DESERT NEAR POLES
				aBiomeMap(_x,0)=1;
				aBiomeMap(_x,aBiomeMap.nY-1)=1;
					// NO DESERT ON EQUATOR
				aBiomeMap(_x,aBiomeMap.nY/2)=255;
				
				//int third = aBiomeMap.nY/3;
				//aBiomeMap(_x,third)=0;
				//aBiomeMap(_x,third*2)=1;
			}
		}

			// GENERATE HEIGHTMAP
		DiamondSquareAlgorithm dsa;
		dsa.wrapX = wrapX;
		dsa.wrapY = wrapY;
		dsa.seed = _seed;
		
		//HEIGHTMAP TABLE FREESTEPS SMOOTHING
		if (_biomeName=="Desert")
		{		dsa.generate(&aBiomeMap,0,0,_smoothing,80);
		}
		else
		{		dsa.generate(&aBiomeMap,0,_freeSteps,_smoothing,200);
		}
		
		


			// MANUALLY BUILD THE BIOME TABLE (WE NEED TO ONLY COUNT LAND TILES).
		int countedLandTiles=0;
		for (int _y=0;_y<mapSize;++_y)
		{
			for (int _x=0;_x<mapSize;++_x)
			{
				if ( aTerrainType(_x,_y) != OCEAN )
				{
					biomeTable [ aBiomeMap(_x,_y) ] ++;
					++countedLandTiles;
				}
			}
		}

		//std::cout<<"Counted land tiles: "<<countedLandTiles<<".\n";
		//std::cout<<"Desired biome percent: "<<biomePercent<<".\n";

		// Must take into account sealevel.

		unsigned char biomeThreshold=0;
		int total=0;
		for (int i=0; i<256; ++i)
		{
			total+=biomeTable[i];
			biomeThreshold = i;

			//std::cout<<"Current biome percent: "<<(double)total/countedLandTiles <<".\n";


				// I CHANGED IT SO THAT PERCENTAGES ARE RELATIVE TO LAND, RATHER THAN THE ENTIRE MAP.
			if ((double)total/countedLandTiles >= biomePercent)
			{ break; }
		}
		//std::cout<<"Total tiles: "<<aTerrainType.nX*aTerrainType.nY<<"\n";
		//std::cout<<"Percent: "<<(double)total/countedLandTiles<<"\n";
		//std::cout<<"Total land tiles: "<<countedLandTiles<<"\n";
		//std::cout<<"Total biome tiles: "<<total<<"\n";
		//std::cout<<"Biome threshold: "<<(int)biomeThreshold<<"\n";
		int nnn=0;

		
		//std::lock_guard<std::mutex> guard(mutexArrayAccess);
		
		//cvDesertFinished
		
		// std::thread t1 (WorldGenerator2::createBiome, this, WorldGenerator2::JUNGLE, 0.33, 4, 0.78, "Jungle", biomeMap2, subSeed[1]);
		// std::thread t2 (WorldGenerator2::createBiome, this, WorldGenerator2::FOREST, 0.5, 8, 0.8, "Forest", biomeMap2, subSeed[2]);
		// std::thread t3 (&WorldGenerator2::createBiome, this, WorldGenerator2::WETLAND, 0.05, 11, 0.79, "Wetland", biomeMap2, subSeed[3]);
		// std::thread t4 (&WorldGenerator2::createBiome, this, WorldGenerator2::STEPPES, 0.05, 2, 0.77, "Steppes", biomeMap2, subSeed[4]);
		// std::thread t5 (&WorldGenerator2::createBiome, this, WorldGenerator2::SNOW, 0.25, 2, 0.76, "Tundra", biomeMap2, subSeed[5]);
		// std::thread t6 (&WorldGenerator2::createBiome, this, WorldGenerator2::DESERT, 0.11, 1, 0.8, "Desert", biomeMap2, subSeed[6]);
		// std::thread t7 (&WorldGenerator2::createBiome, this, WorldGenerator2::HILLY, 0.05, 8, 0.8, "Hills", biomeMap2, subSeed[7]);
		// std::thread t8 (&WorldGenerator2::createBiome, this, WorldGenerator2::MOUNTAIN, 0.07, 13, 0.78, "Mountains", biomeMap2, subSeed[8]);
		
			// MERGE DOWN THE BIOME LAYER
			// THIS MUST BE DONE IN ORDER.

			
		for (int _y=0;_y<mapSize;++_y)
		{
			for (int _x=0;_x<mapSize;++_x)
			{
				if ( aBiomeMap(_x,_y) <= biomeThreshold && aTerrainType(_x,_y) != OCEAN )
				{
					nnn++;
					aTerrainType(_x,_y) = BIOME_TYPE;
				}
			}
		}
		
		// if ( BIOME_TYPE == JUNGLE)
		// {
			// cvDesertFinished.notify_all();
		// }
		
		//std::cout<<"Final biome tiles: "<<nnn<<"\n";
		
		//std::cout<<"Debug biome table:\n";
		
		//for ( int i=0;i<256;++i)
		//{
		//std::cout<<"["<<i<<"] : "<<biomeTable[i]<<".\n";
		//}
		std::cout<<"END creating biome: "<<_biomeName<<".\n";
		//std::cout<<"finished\n";
	}

		// DO NOT INCLUDE .PNG IN THE FILENAME, IT WILL BE INCLUDED AUTOMATICALLY.
		
		// WE EXPORT A NUMBER OF PNGS WHICH CAN BE USED FOR DIFFERENT THINGS.
		
		// GREYSCALE.PNG
			// THE ORIGINAL FRACTAL.
		
		// LANDMASS.PNG
			// SOLID GREEN FOR LAND, SOLID BLUE FOR WATER. USEFUL FOR DETERMINING THE DIFFERENT LANDMASSES.
			// We shouldn't be using ArrayS3 for the topo map, but rather a PNG object.
	void exportPNG(std::string fileName)
	{
		std::cout<<"Exporting png\n";
		
		//ArrayS3 <unsigned char> aTopoMap (mapSize,mapSize,3,0);
		aTopoMap.init(mapSize,mapSize,3,0);
		Png png;
		//png.useCompression=true;
		
		//TODO: Add tuple support. For now, allow passing tuple data to Z and value. Size must match of course.
		
		//std::tuple grasslandColour = (0,255,0);
		//std::tuple<unsigned char, unsigned char, unsigned char> tplGrasslandRGB(0,255,0);
		//unsigned char grasslandColour = std::make_tuple(0,255,0);
		//auto tplRGB = std::make_tuple(0,1,2);
		Vector <unsigned char> vOceanRGB {0,0,255};
		Vector <unsigned char> vGrassRGB {0,255,0};
		
				std::string landFormSaveData = "";
		
		
		//NOTHING=0, OCEAN=1, GRASSLAND=2, FOREST=3, DESERT=4, MOUNTAIN=5, SNOW=6, HILLY=7, JUNGLE=8, WETLAND=9, STEPPES=10, CAVE=11, RUIN=12, ICE=13};
		
			// Testing tile object
		for (int _y=0;_y<mapSize;++_y)
		{
			for (int _x=0;_x<mapSize;++_x)
			{
				// LANDFORM TILE DATA
				if (aTile(_x,_y).terrainType == OCEAN)
				{
					aTopoMap(_x,_y,vOceanRGB);
					landFormSaveData+="O";
					
				}
				else
				{
					aTopoMap(_x,_y,vGrassRGB);
					landFormSaveData+="G";
				}
				
				// BIOME DATA. THIS SYSTEM ALLOWS UP TO 256 BIOMES, WHICH IS PLENTY.
				if (aTerrainType(_x,_y) == OCEAN)
				{ landFormSaveData+="O"; }
				else if (aTerrainType(_x,_y) == GRASSLAND)
				{ landFormSaveData+="G"; }
				else if (aTerrainType(_x,_y) == FOREST)
				{ landFormSaveData+="F"; }
				else if (aTerrainType(_x,_y) == DESERT)
				{ landFormSaveData+="D"; }
				else if (aTerrainType(_x,_y) == MOUNTAIN)
				{ landFormSaveData+="M"; }
				else if (aTerrainType(_x,_y) == SNOW)
				{ landFormSaveData+="S"; }
				else
				{ landFormSaveData+="?"; }
				
				// HEIGHTMAP DATA. MAYBE NOT USEFUL FOR ANYTHING (MAYBE RAINFALL SEED DATA), BUT MIGHT AS WELL PUT IT IN.
				landFormSaveData+=DataTools::toString((int)aHeightMap(_x,_y),3);
				
				// 9 digit local seed. Even with such a high seed value, there is a high chance of a repeating seed on large maps.
				landFormSaveData+=DataTools::toString(Random::randomInt(999999999),9);
				
				// GOOD/EVIL MODIFIER
				if ( aGoodEvilMap(_x,_y) == 0 )
				{ landFormSaveData+="N"; }
				else if ( aGoodEvilMap(_x,_y) == 1 )
				{ landFormSaveData+="G"; }
				else if ( aGoodEvilMap(_x,_y) == 2 )
				{ landFormSaveData+="E"; }
				else
				{ landFormSaveData+="?"; }
				
				landFormSaveData+=",";
				
			}
				// Push heightmap data, ensuring 3 digits.
			landFormSaveData+="\n";
		}
		png.encodeS3(fileName+"-tileObjectTest.png",&aTopoMap);
		
		FileManagerStatic::clearFile("worldSaveData.txt");
		FileManagerStatic::writeString("WILDCAT_WORLDGEN_SAVEDATA 10005.\nWorld Metadata goes here. Tile data follows.\n","worldSaveData.txt");
		FileManagerStatic::writeString("Format:[Land/ocean][Biome][Heightmap (3 digits)][tile seed (9 digits)][Good/evil][delimiter (comma)]\n","worldSaveData.txt");
		
		std::string saveDataMapSize="<MAP_SIZE>"+DataTools::toString(mapSize,5)+"</MAP_SIZE>";
		FileManagerStatic::writeString(saveDataMapSize+"\n<WORLD_DATA>","worldSaveData.txt");

		FileManagerStatic::writeString(landFormSaveData+"</WORLD_DATA>","worldSaveData.txt");
		
		
			// GREYSCALE.PNG
		for (int _y=0;_y<mapSize;++_y)
		{
			for (int _x=0;_x<mapSize;++_x)
			{	
				aTopoMap.fillZ(_x,_y,aHeightMap(_x,_y));
			}
		}
		png.encodeS3(fileName+"-greyscale.png",&aTopoMap);

			// PASS 1: EXPORT LAND MAP. HERE WE WILL EXPORT A MAP WHICH ONLY SHOWS LAND AND WATER.
		for (int _y=0;_y<mapSize;++_y)
		{
			for (int _x=0;_x<mapSize;++_x)
			{
				//int heightMod = ((double)aHeightMap(_x,_y)/255) * 100;
				//std::cout<<"Heightmod: "<<heightMod<<".\n";
				if ( aTerrainType(_x,_y) == OCEAN )
				{
					aTopoMap(_x,_y,0)=50;
					aTopoMap(_x,_y,1)=50;
					aTopoMap(_x,_y,2)=150;
					//aTopoMap(_x,_y,2)=aHeightMap(_x,_y);
				}
				else
				{
					aTopoMap(_x,_y,0)=50;
					aTopoMap(_x,_y,1)=150;
					//aTopoMap(_x,_y,1)=aHeightMap(_x,_y);
					aTopoMap(_x,_y,2)=50;
				}
			}
		}
		png.encodeS3(fileName+"-landmass.png",&aTopoMap);
		
		
		//EDGE DETECTION ON HEIGHTMAP, MIGHT BE GOOD FOR TECTONICS OR OCEAN CURRENTS...
		
		//TECTONICS. LAND NEAR HOTSPOTS WILL HAVE HIGHER CHANCE OF VOLCANOS.
		
		//unsigned int minEdge = (256/2)-5;
		
		unsigned int minEdge = seaLevel-60;
		//unsigned int maxEdge = (256/2)+5;
		unsigned int maxEdge = seaLevel-50;
		
		//unsigned int minTectonic = 
		
		for (int _y=0;_y<mapSize;++_y)
		{
			for (int _x=0;_x<mapSize;++_x)
			{
				if ( aHeightMap(_x,_y) <= maxEdge &&  aHeightMap(_x,_y) >= minEdge )
				{
					aTopoMap(_x,_y,0)=250;
					aTopoMap(_x,_y,1)=0;
					aTopoMap(_x,_y,2)=0;
				}
				else if (aTerrainType(_x,_y) == OCEAN)
				{
					aTopoMap(_x,_y,vOceanRGB);
				}
				else
				{
					aTopoMap(_x,_y,vGrassRGB);
				}
			}
		}
		png.encodeS3(fileName+"-edge.png",&aTopoMap);
		
		
		for (int _y=0;_y<mapSize;++_y)
		{
			for (int _x=0;_x<mapSize;++_x)
			{
				//int heightMod = ((double)aHeightMap(_x,_y)/255) * 100;
				//std::cout<<"Heightmod: "<<heightMod<<".\n";
				if ( aTerrainType(_x,_y) == OCEAN )
				{
					aTopoMap(_x,_y,0)=aHeightMap(_x,_y)/3;
					aTopoMap(_x,_y,1)=aHeightMap(_x,_y)/3;
					aTopoMap(_x,_y,2)=aHeightMap(_x,_y);
				}
				else
				{
					aTopoMap(_x,_y,0)=aHeightMap(_x,_y)/3;
					aTopoMap(_x,_y,1)=aHeightMap(_x,_y);
					aTopoMap(_x,_y,2)=aHeightMap(_x,_y)/3;
				}
			}
		}
		png.encodeS3(fileName+"-landmassHeightmap.png",&aTopoMap);
		
		// PASS 2: EXPORT GOOD/EVIL MAP.
		
		// ADDITIONAL PASS: GOOD / EVIL MODIFIER LAYER. No biomes, just land, water, good and evil. Only land has a good/evil modifier.

		for (int _y=0;_y<mapSize;++_y)
		{
			for (int _x=0;_x<mapSize;++_x)
			{
				if ( aTerrainType(_x,_y) == OCEAN )
				{
					//aTopoMap(_x,_y,0)=50;
					//aTopoMap(_x,_y,1)=50;
					//aTopoMap(_x,_y,2)=240;
				}
				else
				{
					if ( aGoodEvilMap(_x,_y) == 1 )
					{
						aTopoMap(_x,_y,0)=255;
						aTopoMap(_x,_y,1)=200;
						aTopoMap(_x,_y,2)=180;
					}
					else if ( aGoodEvilMap(_x,_y) == 2 )
					{
						aTopoMap(_x,_y,0)=160;
						aTopoMap(_x,_y,1)=70;
						aTopoMap(_x,_y,2)=160;
					}
					else
					{
						aTopoMap(_x,_y,0)=0;
						aTopoMap(_x,_y,1)=250;
						aTopoMap(_x,_y,2)=0;
					}
				}
			}
		}

		png.encodeS3(fileName+"-goodevil.png",&aTopoMap);
		

			// CLEAR THE TOPO MAP.
		//aTopoMap.init(mapSize,mapSize,3,0);
		
// Create a lightening darkening texture fractal.
		ArrayS2 <int> aLightModifier;
		aLightModifier.init(mapSize,mapSize,0);

			// GENERATE HEIGHTMAP
		DiamondSquareAlgorithmCustomRange dsa;
		dsa.maxValue=12;
		//HEIGHTMAP TABLE FREESTEPS SMOOTHING
		dsa.generate(&aLightModifier,0,2,0.75,250);

		for (int _y=0;_y<mapSize;++_y)
		{
			for (int _x=0;_x<mapSize;++_x)
			{
				//const int lightModifier = aLightModifier(_x,_y);
				const int lightModifier = 0;
				
				int _red = 0;
				int _green = 0;
				int _blue = 0;
				
				if ( aTerrainType(_x,_y) == OCEAN )
				{
					_red=50;
					_green=50;
					_blue=240;
				}
				else if ( aTerrainType(_x,_y) == MOUNTAIN )
				{
					_red=80;
					_green=80;
					_blue=80;
				}
				else if ( aTerrainType(_x,_y) == HILLY )
				{
					_red=80;
					_green=140;
					_blue=80;
				}
				else if ( aTerrainType(_x,_y) == FOREST )
				{
					_red=0;
					_green=120;
					_blue=0;
				}
				else if ( aTerrainType(_x,_y) == DESERT )
				{
					_red=200;
					_green=200;
					_blue=20;
				}
				else if ( aTerrainType(_x,_y) == SNOW )
				{
					_red=220;
					_green=220;
					_blue=240;
				}
				else if ( aTerrainType(_x,_y) == JUNGLE )
				{
					_red=0;
					_green=70;
					_blue=0;
				}
				else if ( aTerrainType(_x,_y) == WETLAND )
				{
					_red=20;
					_green=150;
					_blue=200;
				}
				else if ( aTerrainType(_x,_y) == STEPPES )
				{
					_red=180;
					_green=120;
					_blue=40;
				}
				else if ( aTerrainType(_x,_y) == ICE )
				{
					_red=255;
					_green=255;
					_blue=255;
				}
				else if ( aTerrainType(_x,_y) == CAVE )
				{
					_red=0;
					_green=0;
					_blue=0;
				}
				else if ( aTerrainType(_x,_y) == RUIN )
				{
					_red=255;
					_green=0;
					_blue=255;
				}
				else
				{
					_red=66;
					_green=180;
					_blue=66;
				}
				
				//mof light values.
				// _red += lightModifier;
				// if ( _red > 255 ) { _red = 255; }

				// _green += lightModifier;
				// if ( _green > 255 ) { _green = 255; }
				
				// _blue += lightModifier;
				// if ( _blue > 255 ) { _blue = 255; }
				
				_red -= lightModifier;
				if ( _red < 0 ) { _red = 0; }

				_green -= lightModifier;
				if ( _green < 0 ) { _green = 0; }
				
				_blue -= lightModifier;
				if ( _blue < 0 ) { _blue = 0; }
				
				
				aTopoMap(_x,_y,0) = _red;
				aTopoMap(_x,_y,1) = _green;
				aTopoMap(_x,_y,2) = _blue;
			}
		}
		//worldGenerator2_threadLock.lock();
		//Png::encodeS3(fileName,&aTopoMap);
		//worldGenerator2_threadLock.unlock();
		//Png png;
		//std::cout<<"1\n";
		
		png.encodeS3(fileName+".png",&aTopoMap);
		//std::cout<<"2\n";
		//png.saveToFile(fileName+"2.png");
		//std::cout<<"3\n";
		
		// EXPORT TEXTURED
			// CLEAR THE TOPO MAP.
		//aTopoMap.init(mapSize,mapSize,3,0);
		
// Create a lightening darkening texture fractal.
		ArrayS2 <int> aLightModifier2;
		aLightModifier2.init(mapSize,mapSize,0);

			// GENERATE HEIGHTMAP
		DiamondSquareAlgorithmCustomRange dsa2;
		dsa2.maxValue=12;
		//HEIGHTMAP TABLE FREESTEPS SMOOTHING
		dsa2.generate(&aLightModifier2,0,2,0.75,250);

		for (int _y=0;_y<mapSize;++_y)
		{
			for (int _x=0;_x<mapSize;++_x)
			{
				const int lightModifier = aLightModifier2(_x,_y);
				//const int lightModifier = 0;
				
				int _red = 0;
				int _green = 0;
				int _blue = 0;
				
				if ( aTerrainType(_x,_y) == OCEAN )
				{
					_red=50;
					_green=50;
					_blue=240;
				}
				else if ( aTerrainType(_x,_y) == MOUNTAIN )
				{
					_red=80;
					_green=80;
					_blue=80;
				}
				else if ( aTerrainType(_x,_y) == HILLY )
				{
					_red=80;
					_green=140;
					_blue=80;
				}
				else if ( aTerrainType(_x,_y) == FOREST )
				{
					_red=0;
					_green=120;
					_blue=0;
				}
				else if ( aTerrainType(_x,_y) == DESERT )
				{
					_red=200;
					_green=200;
					_blue=20;
				}
				else if ( aTerrainType(_x,_y) == SNOW )
				{
					_red=220;
					_green=220;
					_blue=240;
				}
				else if ( aTerrainType(_x,_y) == JUNGLE )
				{
					_red=0;
					_green=70;
					_blue=0;
				}
				else if ( aTerrainType(_x,_y) == WETLAND )
				{
					_red=20;
					_green=150;
					_blue=200;
				}
				else if ( aTerrainType(_x,_y) == STEPPES )
				{
					_red=180;
					_green=120;
					_blue=40;
				}
				else if ( aTerrainType(_x,_y) == ICE )
				{
					_red=255;
					_green=255;
					_blue=255;
				}
				else if ( aTerrainType(_x,_y) == CAVE )
				{
					_red=0;
					_green=0;
					_blue=0;
				}
				else if ( aTerrainType(_x,_y) == RUIN )
				{
					_red=255;
					_green=0;
					_blue=255;
				}
				else
				{
					_red=66;
					_green=180;
					_blue=66;
				}
				
				//mof light values.
				// _red += lightModifier;
				// if ( _red > 255 ) { _red = 255; }

				// _green += lightModifier;
				// if ( _green > 255 ) { _green = 255; }
				
				// _blue += lightModifier;
				//if ( _blue > 255 ) { _blue = 255; }
				
				_red -= lightModifier;
				if ( _red < 0 ) { _red = 0; }

				_green -= lightModifier;
				if ( _green < 0 ) { _green = 0; }
				
				_blue -= lightModifier;
				if ( _blue < 0 ) { _blue = 0; }
				
				
				aTopoMap(_x,_y,0) = _red;
				aTopoMap(_x,_y,1) = _green;
				aTopoMap(_x,_y,2) = _blue;
			}
		}
		//worldGenerator2_threadLock.lock();
		//Png::encodeS3(fileName,&aTopoMap);
		//worldGenerator2_threadLock.unlock();
		//Png png;
		png.encodeS3(fileName+"-textured"+".png",&aTopoMap);
		std::cout<<"Finished exporting png\n";

	}
};

std::string WorldGenerator2::biomeName [15] = { "nothing", "ocean", "grassland", "forest", "desert", "mountain", "snow", "hilly", "jungle", "wetland", "steppes", "cave", "ruin", "ice", "river" };

#endif
