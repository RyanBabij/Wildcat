#ifndef DATA_DATADIAGNOSTICS_HPP
#define DATA_DATADIAGNOSTICS_HPP

#include <iostream>

#include <Data/DataTools.hpp>

class DataDiagnostics
{
	public:
	static void checkDatatypes()
	{
		std::cout<<"Checking datatypes... ";
		if(DataTools::datatypeSizesOkay())
		{ std::cout<<"OK\n"; }
		else
		{ std::cout<<"Error - The datatypes do not have assumed sizes.\n"; }
	}
};

#endif