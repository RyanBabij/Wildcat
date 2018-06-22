#pragma once
#ifndef WILDCAT_CONTAINER_TABLE_TABLE_INTERFACE_HPP
#define WILDCAT_CONTAINER_TABLE_TABLE_INTERFACE_HPP

/*
	#include <Container/Table/TableInterface.hpp>
		TABLE INTERFACE. Works with table GUI. Allows data to be queried.
		
	NOTES:
	Probably would be better to use templates and pointers in the future. However right now this is not a major issue.

	
	EXAMPLE CODE:
	
std::string ClassName::getColumn(std::string _column)
{
	if ( _column=="name" )
	{
		return name;
	}
	else if ( _column=="id" )
	{
		return DataTools::toString(id);
	}
	
	return "?";
}


std::string ClassName::getColumnType(std::string _column)
{
	if ( _column == "id" )
	{
		return "number";
	}
	return "string";
}
*/

	// OBJECTS TO BE INCLUDED IN THE TABLE MUST INHERIT THE TABLE INTERFACE.
class TableInterface
{
	public:
	// RETURN STRING REPRESENTING THE FIELD VALUE.
	virtual std::string getColumn(std::string _column)=0;
	
	//virtual int nColumns()=0;
	
	virtual std::string getColumnType(std::string _column)=0;
};

#endif
