#pragma once
#ifndef WILDCAT_CONTAINER_TABLE_TABLE_INTERFACE_HPP
#define WILDCAT_CONTAINER_TABLE_TABLE_INTERFACE_HPP

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
