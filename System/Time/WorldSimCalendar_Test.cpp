#include "WorldSimCalendar.hpp"

#include <string>

// g++ WorldSimCalendar_Test.cpp -I %WILDCAT%/

int main (int nArgs, char** arg)
{
	std::cout<<"Making a blank calendar...\n";
	WorldSimCalendar calendar;
	
	
	std::cout<<"Tostring: "<<calendar.toString()<<"\n";
	calendar.increment(10);
	std::cout<<"Tostring: "<<calendar.toString()<<"\n";
	
	WorldSimCalendar calendar2 (calendar);
	std::cout<<"Tostring: "<<calendar2.toString()<<"\n";
	
	
	
	std::cout<<"End of tests.\n";
	return 0;
}
