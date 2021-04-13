
#include "../FileManagerStatic.hpp"
#include "../WTFManager.hpp"

#include <Math/Random/RandomLehmer.hpp>

#include <string>
#include <iostream>

const std::string testFile = "WTFManager_Test.txt";


int main (int nArgs, char ** arg)
{
	RandomLehmer rng;
	rng.seed(time(NULL));
	
	if (FileManagerStatic::fileExists(testFile))
	{
		std::cout<<"File: "<<testFile<<" exists\n";
		
		std::string strFile = FileManagerStatic::getFile(testFile);
		std::cout<<"File content: "<<strFile<<"\n";
		std::cout<<"Success\n";
		
		WTFManager wtfManager;
		std::cout<<"Parsing raw\n";
		wtfManager.parse(strFile);
		std::cout<<"END Parsing raw\n";
		
		std::cout<<"Outputting parsed raw:\n";
		std::string strRaw = wtfManager.getAll();
		std::cout<<strRaw<<"\n";
		
		// Tag searching
		if (wtfManager.hasTag("COLOUR.RED") )
		{
			std::cout<<"Found COLOUR.RED\n";
		}
		else
		{
			std::cout<<"COLOUR.RED not found\n";
		}
		if (wtfManager.hasTag("COLOUR.BLUEE"))
		{
			std::cout<<"Found COLOUR.BLUEE\n";
		}
		else
		{
			std::cout<<"COLOUR.BLUEE not found.\n";
		}
		if (wtfManager.hasTag("COLOUR.RED.DARK_RED") )
		{
			std::cout<<"Found COLOUR.RED.DARK_RED\n";
		}
		else
		{
			std::cout<<"COLOUR.RED.DARK_RED not found\n";
		}
		
		std::cout<<"Getting random COLOUR node.\n";
		
		WTFNode* node = wtfManager.getRandom("COLOUR", rng);
		
		if (node==0)
		{
			std::cout<<"Search returned null ptr\n";
		}
		else
		{
			std::cout<<"COLOUR found: "<<node->toString()<<"\n";
		}
		
		std::cout<<"Getting random COLOUR.RED node.\n";
		
		node = wtfManager.getRandom("COLOUR.RED", rng);
		
		if (node==0)
		{
			std::cout<<"Search returned null ptr\n";
		}
		else
		{
			std::cout<<"COLOUR.RED found: "<<node->toString()<<"\n";
		}
		
		return 0;
	}
	std::cout<<"Fail\n";
	return 0;
}