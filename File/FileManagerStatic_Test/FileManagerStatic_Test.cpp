
#include "../FileManagerStatic.hpp"

#include <string>
#include <iostream>

const std::string testFile = "FileManagerStatic_Test.txt";


int main (int nArgs, char ** arg)
{
	if (FileManagerStatic::fileExists(testFile))
	{
		std::cout<<"File: "<<testFile<<" exists\n";
		
		std::string strFile = FileManagerStatic::getFile(testFile);
		std::cout<<"File content: "<<strFile<<"\n";
		std::cout<<"Success\n";
		return 0;
	}
	std::cout<<"Fail\n";
	return 0;
}