#pragma once
#ifndef WILDCAT_TABLE_HPP
#define WILDCAT_TABLE_HPP

#include <string>

	//C++0x
//#include <unordered_map>


// 0230747205: I'm thinking this should be converted to templates.





/* #include <Container/Table/Table.hpp>
Class to store multiple columns of data, with the ability to sort all data by a particular column.
*/
class Table_Column
{
	public:
	Table_Column()
	{
		//currentID=0;
	}
	~Table_Column()
	{
	}
	
		// UNIQUE ID FOR ALL ENTRIES IN COLUMN. ALLOWS OTHER COLUMNS TO SORT THEMSELVES AGAINST THIS COLUMN.
	//Vector <int> vID;
	//int currentID;
	
	virtual Vector <int>* sortAscending()=0;
	virtual Vector <int>* sortDescending()=0;
	
	virtual void push(std::string _data)=0;
	virtual void push(int _data)=0;
	
	virtual std::string getData(int _slot)=0;
	virtual int size()=0;
	
	virtual void clear()=0;
	
	virtual void deleteSlot(int _slot)=0;

};

class Table_Column_String: public Table_Column
{
	public:
	Vector <std::string> vData;
	
	Table_Column_String(): Table_Column()
	{
	}
	
	void clear()
	{
		vData.clear();
	}
	void deleteSlot(int _slot)
	{
		vData.deleteSlot(_slot);
	}
	
	Vector <int> * sortAscending()
	{
		// std::cout<<"Sorting column ascending.\n";
		// Vector <int> vOrder;
		// // Get lowest number not currently added.
		// while (vOrder.size() < vData.size() )
		// {
			// int lowestSlot=-1;
			// for (int i=0;i<vData.size();++i)
			// {
				// if ( vOrder.contains(i) == false )
				// {
					// if (lowestSlot==-1 || vData(i) < vData(lowestSlot))
					// {
						// lowestSlot=i;
					// }
				// }
			// }
			// vOrder.push(lowestSlot);
			// std::cout<<vData(lowestSlot)<<".\n";
		// }
		return vData.getIndexesAscending();
		//for ( int i=0;i<vData.
	}
	Vector <int> * sortDescending()
	{
		// std::cout<<"Sorting column descending.\n";
		// Vector <int> vOrder;
		// // Get lowest number not currently added.
		// while (vOrder.size() < vData.size() )
		// {
			// int lowestSlot=-1;
			// for (int i=0;i<vData.size();++i)
			// {
				// if ( vOrder.contains(i) == false )
				// {
					// if (lowestSlot==-1 || vData(i) > vData(lowestSlot))
					// {
						// lowestSlot=i;
					// }
				// }
			// }
			// vOrder.push(lowestSlot);
			// std::cout<<vData(lowestSlot)<<".\n";
		// }
		return vData.getIndexesDescending();
	}
	
	void push(std::string _data)
	{
		//vID.push(currentID);
		vData.push(_data);
		//++currentID;
	}
	void push(int _data)
	{
		//vID.push(currentID);
		vData.push(DataTools::toString(_data));
		//++currentID;
	}
	std::string getData(int _slot)
	{
		return vData(_slot);
	}
	int size()
	{
		return vData.size();
	}
};

class Table_Column_Int: public Table_Column
{
	public:
	Vector <int> vData;
	
	Table_Column_Int(): Table_Column()
	{
	}
	
	void clear()
	{
		vData.clear();
	}
	
	void deleteSlot(int _slot)
	{
		vData.deleteSlot(_slot);
	}
	

	
	Vector <int> * sortAscending()
	{
		// std::cout<<"Sorting column ascending.\n";
		// Vector <int> vOrder;
		// // Get lowest number not currently added.
		// while (vOrder.size() < vData.size() )
		// {
			// int lowestSlot=-1;
			// for (int i=0;i<vData.size();++i)
			// {
				// if ( vOrder.contains(i) == false )
				// {
					// if (lowestSlot==-1 || vData(i) < vData(lowestSlot))
					// {
						// lowestSlot=i;
					// }
				// }
			// }
			// vOrder.push(lowestSlot);
			// std::cout<<vData(lowestSlot)<<".\n";
		// }
		return vData.getIndexesAscending();
	}
	Vector <int> * sortDescending()
	{
		// std::cout<<"Sorting column descending.\n";
		// Vector <int> vOrder;
		// // Get lowest number not currently added.
		// while (vOrder.size() < vData.size() )
		// {
			// int lowestSlot=-1;
			// for (int i=0;i<vData.size();++i)
			// {
				// if ( vOrder.contains(i) == false )
				// {
					// if (lowestSlot==-1 || vData(i) > vData(lowestSlot))
					// {
						// lowestSlot=i;
					// }
				// }
			// }
			// vOrder.push(lowestSlot);
			// std::cout<<vData(lowestSlot)<<".\n";
		// }
		return vData.getIndexesDescending();
	}
	void push(std::string _data)
	{
		//vID.push(currentID);
		vData.push(DataTools::toInt(_data));
		//++currentID;
	}
	void push(int _data)
	{
		//vID.push(currentID);
		vData.push(_data);
		//++currentID;
	}
	std::string getData(int _slot)
	{
		return DataTools::toString(vData(_slot));
	}
	int size()
	{
		return vData.size();
	}
};

class Table_Column_Int_Pointer: public Table_Column
{
	public:
	Vector <int*> vData;
	
	Table_Column_Int_Pointer(): Table_Column()
	{
	}
	
	void clear()
	{
		vData.clear();
	}
	
	void deleteSlot(int _slot)
	{
		vData.deleteSlot(_slot);
	}
	

	
	Vector <int> * sortAscending()
	{
		Vector <int> vDataV;
		for ( int i=0;i<vData.size();++i)
		{
			vDataV.push(*vData(i));
		}
		return vDataV.getIndexesAscending();
	}
	Vector <int> * sortDescending()
	{
		Vector <int> vDataV;
		for ( int i=0;i<vData.size();++i)
		{
			vDataV.push(*vData(i));
		}
		return vDataV.getIndexesDescending();
	}
	void push(std::string _data)
	{
		//vID.push(currentID);
		//vData.push(DataTools::toInt(_data));
		//++currentID;
	}
	void push(int* _data)
	{
		//vID.push(currentID);
		vData.push(_data);
		//++currentID;
	}
	std::string getData(int _slot)
	{
		return DataTools::toString(*vData(_slot));
	}
	int size()
	{
		return vData.size();
	}
};


class Table
{
	public:
	
	Vector <Table_Column*> vColumn;
		// PROVIDES THE CURRENT SORTING OF THE TABLE.
	Vector <int> vID;
	
	Table()
	{
	}
	
		// DELETE ALL ROWS WITH THE GIVEN VALUE IN THE GIVEN COLUMN (BEST TO USE A UNIQUE ID).
	void deleteEntry(int _column, std::string _value)
	{
		std::cout<<"Deleting entry.\n";
		//int _deleteSlot = -1;
		
		for ( int i=0;i<nRows();++i)
		{
			if ( vColumn(_column)->getData(i) == _value )
			{
				//std::cout<<"FOUND.\n";
				//_deleteSlot = i;
				
				for ( int i2=0;i2<vColumn.size();++i2)
				{
					vColumn(i2)->deleteSlot(i);
				}
				
			}
		}
	std::cout<<"END Deleting entry.\n";
	}
	
	void clear()
	{
		for ( int i=0;i<vColumn.size();++i)
		{
			vColumn(i)->clear();
		}
	}
	
	int nColumns()
	{
		return vColumn.size();
	}
		// ONLY COMPLETE ROWS ARE COUNTED.
	int nRows()
	{
		int leastRows = -1;
		for ( int i=0;i<vColumn.size();++i)
		{
			if (leastRows==-1 || vColumn(i)->size() < leastRows )
			{
				leastRows = vColumn(i)->size();
			}
		}
		return leastRows;
	}
	
	void addStringColumn()
	{
		Table_Column_String * c = new Table_Column_String;
		vColumn.push(c);
	}
	void addIntColumn()
	{
		Table_Column_Int * c = new Table_Column_Int;
		vColumn.push(c);
	}
	
	void pushData(int _column, std::string _data)
	{
		vColumn(_column)->push(_data);
	}
	void pushData(int _column, int _data)
	{
		vColumn(_column)->push(_data);
	}
	
	void cout()
	{
			std::cout<<"Printing table.\n";
		for ( int i2=0;i2<vColumn(0)->size();++i2)
		{
			for ( int i=0;i<vColumn.size();++i)
			{
				//std::cout<<"Column: "<<i<<".\n";
				//Table_Column* c = vColumn(i);
			

				std::cout<<get(i,i2)<<", ";
			}
			std::cout<<"\n";
		}
	}
	
	void sortAscendingBy(int _column)
	{
		vID.clear();
		Vector <int>* vIndex = vColumn(_column)->sortAscending();
		
		//std::cout<<"Sorted... Indexes:\n";
		
		for (int i=0;i<vIndex->size();++i)
		{
			//std::cout<<(*vIndex)(i)<<".\n";
		}
		
		vID.copy(vIndex);
		
		//std::cout<<"VID:\n";
		
		for (int i=0;i<vIndex->size();++i)
		{
			//std::cout<<vID(i)<<".\n";
		}
		delete vIndex;
	}
	void sortDescendingBy(int _column)
	{
		vID.clear();
		Vector <int>* vIndex = vColumn(_column)->sortDescending();
		vID.copy(vIndex);
		delete vIndex;
	}
	
	bool isSafe(const int _column, const int _row)
	{
		if ( _column < nColumns() && _row < nRows() )
		{
			return true;
		}
		return false;
	}
	
	std::string get(const int _column, const int _row)
	{
		if ( isSafe(_column,_row)==true )
		{
			return vColumn(_column)->getData(vID(_row));
		}
		return "?";
	}
	
	
};

	// NEW IMPLEMENTATION OF TABLE USING TABLE INTERFACE OBJECTS.
#include <Container/Table/TableInterface.hpp>

class Table2
{
	public:
	
	Vector <TableInterface*> vRow;
		// PROVIDES THE CURRENT SORTING OF THE TABLE.
	Vector <int> vID;
		// COLUMN TYPE: 
	
	Table2()
	{
	}
	
		// REMOVE ALL INSTANCES OF THE PASSED OBJECT IN THE TABLE.
	void deleteRow ( TableInterface* _object )
	{
		vRow.removeAll(_object);
	}
	void erase( TableInterface* _object )
	{ deleteRow (_object); }
	
	
	void clear()
	{
		vRow.clear();
		vID.clear();
	}

	TableInterface* getObject ( int slot )
	{
		//std::cout<<"Requested slot: "<<slot<<".\n";
		//std::cout<<"Linked ID: "<<vID(slot)<<".\n";
		return vRow(vID(slot));
	}
	
	// int nColumns()
	// {
		// int leastColumns=-1;
		// for ( int i=0;i<vRow.size();++i)
		// {
			// if ( leastColumns==-1 || vRow(i)->nColumns() < leastColumns )
			// {
				// leastColumns=vRow(i)->nColumns();
			// }
		// }
		// return leastColumns;
	
		// //return vRow.size();
	// }
		// ONLY COMPLETE ROWS ARE COUNTED.
	int nRows()
	{
		return vRow.size();
	}
	
	void addRow( TableInterface* _object)
	{
		vRow.push(_object);
		vID.push(vRow.size()-1);
	}
	
	void sortAscendingBy(const std::string _column)
	{
		//std::cout<<"Sorting table\n";
		if ( vRow.size()==0)
		{ return; }
		
		if ( vRow(0)->getColumnType(_column) == "number" )
		{
			Vector <int> vSort;
			vSort.reserve(nRows());
			for ( int i=0;i<nRows();++i)
			{
				vSort.push(DataTools::toInt(vRow(i)->getColumn(_column)));
			}
			vID.clear();
			vID.copy(vSort.getIndexesAscending());
		}
		else
		{
			//std::cout<<"sorting strings\n";
			Vector <std::string> vSort;
			vSort.reserve(nRows());
			//std::cout<<"for loop\n";
			for ( int i=0;i<nRows();++i)
			{
				vSort.push(vRow(i)->getColumn(_column));
			}
			vID.clear();
			//std::cout<<"vid copy\n";
			vID.copy(vSort.getIndexesAscending());
			//std::vector <size_t> sortedIndices = vSort.
			//std::cout<<"Finished sorting strings\n";
		}
	}
	void sortDescendingBy(const std::string _column)
	{
		if ( vRow.size()==0)
		{ return; }
		
		if ( vRow(0)->getColumnType(_column) == "number" )
		{
			Vector <int> vSort;
			for ( int i=0;i<nRows();++i)
			{
				vSort.push(DataTools::toInt(vRow(i)->getColumn(_column)));
			}
			vID.clear();
			vID.copy(vSort.getIndexesDescending());
		}
		else
		{
			Vector <std::string> vSort;
			for ( int i=0;i<nRows();++i)
			{
				vSort.push(vRow(i)->getColumn(_column));
			}
			vID.clear();
			vID.copy(vSort.getIndexesDescending());
		}
	}
	

	bool isSafe(const int _column, const int _row)
	{
		//if ( _row<nRows() )
		//{
		//	if (vRows(_row)
		//}
	
		return false;
	}
	
	std::string get ( std::string _column, const int _row)
	{
		//const int slot = vID.findSlot(_row);
		//std::cout<<"Slot: "<<slot<<".\n";
		
		if ( vID.isSafe(_row) )
		{
			return vRow (vID(_row))->getColumn(_column);
		}
		
		return vRow(_row)->getColumn(_column);
		
		//if ( slot == -1 )
		//{
		//	return vRow(_row)->getColumn(_column);
		//}
		//return vRow(slot)->getColumn(_column);
	
		// if ( vID.size() == 0 )
		// {
			// return vRow(_row)->getColumn(_column);
		// }
		
		
		
		// return vRow(_row)->getColumn(_column);
		
		
		
		// return vColumn(_column)->getData(vID(_row));
		//if ( isSafe(_column,_row)==true )
		//{
			//return vRow(
		//}
		//return "?";
	}
	

	
	// std::string get(const int _column, const int _row)
	// {
		// if ( isSafe(_column,_row)==true )
		// {
			// return vColumn(_column)->getData(vID(_row));
		// }
		// return "?";
	// }
	
	
};

#endif
