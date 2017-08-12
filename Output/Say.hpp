#include <iostream>

#define say (x) std::cout << "DBG " << #x << std::endl; x

void say (const std::string str)
{
	std::cout<<str;
}