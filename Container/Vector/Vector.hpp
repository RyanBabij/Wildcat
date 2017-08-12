#pragma once

/* #include <Container/Vector/Vector.hpp>
Class to store an expandable list of items. Currently just a wrapper of std::vector with some additional functions.
*/

#include <iostream>
#include <vector>

/* Debug macro */
#ifdef CONTAINER_VECTOR_VERBOSE
#include <iostream>
#undef verbose
#define verbose std::cout<<
#else
#undef verbose
#define verbose //
#endif


template <class T>
class Vector
{
	public:
	std::vector <T> data;

	/* Should be unsigned. */
	inline int size()
	{ return data.size(); }
	int length() { return size(); }

	inline T& operator() (const int i)
	{ return data.at(i); }
	inline T& at(const int i)
	{ return data.at(i); }
	inline void push(const T value)
	{ data.push_back(value); }
	inline void add(const T value)
	{ data.push_back(value); }
	
	Vector()
	{}

	//Support added for initializer lists. Now you can build a Vector quick and easy:
	// Vector <int> v {1,2,3,4};
	Vector(std::initializer_list<T> init) : data(init)
	{}
	
	
	void copy (Vector <T> * _vector)
	{
		clear();
		if(_vector==0)
		{
			return;
		}
		for ( int i=0;i<_vector->size();++i)
		{
			push((*_vector)(i));
		}
	}

		// RETURN THE SLOT WITH THE PASSED VALUE. OTHERWISE RETURN -1.
	int findSlot(T _value)
	{
		for (int i=0;i<size();++i)
		{
			if ( (*this)(i) == _value )
			{
				return i;
			}
		}
		return -1;
	}


		// RETURN A VECTOR OF THE INDEXES SORTED ASCENDING (USING OPERATOR <).
	Vector <int> * getIndexesAscending()
	{
		Vector <int> * _vIndex = new Vector <int>;

		//Vector <int> vOrder;
		// Get lowest number not currently added.
		while (_vIndex->size() < size() )
		{
			int lowestSlot=-1;
			for (int i=0;i<size();++i)
			{
				if ( _vIndex->contains(i) == false )
				{
					if (lowestSlot==-1 || (*this)(i) < (*this)(lowestSlot))
					{
						lowestSlot=i;
					}
				}
			}
			_vIndex->push(lowestSlot);
			//std::cout<<(*this)(lowestSlot)<<".\n";
		}

		return _vIndex;
	}
		// RETURN A VECTOR OF THE INDEXES SORTED DESCENDING (USING OPERATOR >).
	Vector <int> * getIndexesDescending()
	{
		Vector <int> * _vIndex = new Vector <int>;

		//Vector <int> vOrder;
		// Get lowest number not currently added.
		while (_vIndex->size() < size() )
		{
			int lowestSlot=-1;
			for (int i=0;i<size();++i)
			{
				if ( _vIndex->contains(i) == false )
				{
					if (lowestSlot==-1 || (*this)(i) > (*this)(lowestSlot))
					{
						lowestSlot=i;
					}
				}
			}
			_vIndex->push(lowestSlot);
			//std::cout<<(*this)(lowestSlot)<<".\n";
		}

		return _vIndex;
	}

		// RETURN A VECTOR WITH NO REPEATING ENTRIES.

	// NOTE THAT THIS TAKES A WHILE WITH LARGE VECTORS ( > 10,000 ENTRIES )
	Vector <T> * removeDuplicates()
	{
		Vector <T> * vUnique = new Vector <T>;
		for(unsigned int i=0;i<data.size();++i)
		{
			//for(unsigned int i2=0;i2<vUnique->size();++i2)
			//{
				// IF NOT IN UNIQUE. ADD IT.
				if ( vUnique->findSlot(data[i]) == -1 )
				{
					vUnique->push(data[i]);
				}
			//}
		}
		return vUnique;
	}


	void removeNulls()
	{
		if ( size()==0 )
		{ return; }

		for(unsigned int i=0;i<data.size();++i)
		{
			if ( data[i]==0)
			{
				eraseSlot(i--);
			}
		}
	}

		// REMOVES ALL ENTRIES WITH A PARTICULAR VALUE.
	void removeAll(T _value)
	{
		if ( size()==0 )
		{ return; }

		//std::cout<<"Remove all.\n";
		for(unsigned int i=0;i<data.size();++i)
		{
			if ( data[i]==_value)
			{
				eraseSlot(i--);
			}


				// 0230747101: NOTE: THE BELOW CODE DOESN'T SEEM TO WORK.
			//if(data[i]==_value)
			//{
			//	std::cout<<"Match for deletion.\n";
				// 0230660013
				// http://stackoverflow.com/questions/3487717/erasing-multiple-objects-from-a-stdvector
				// Also see Erase-remove idiom.
			//	data[i] = data.back();
			//	data.pop_back();
			//}
		}
	}

	// 0230587450
	// RETURNS FALSE IF INDEX IS OUT OF BOUNDS.
	bool isSafe( const int _index )
	{
		return ( _index < size() && _index >= 0 );
	}

	// 0230242133
	// RETURN THE NUMBER OF INDEXES WHICH ARE EQUAL TO THE PASSED VALUE.
	int count ( const T _value )
	{
		int total = 0;
		for(unsigned int i=0;i<data.size();++i)
		{
			if(data[i]==_value)
			{
				++total;
			}
		}
		return total;
	}

	//T pop()
	//{
	//	T popped = data.at(data.size()-1);
	//	data.erase(data.at(data.size()-1));
	//	return popped;
	//}

	/* Added: 0221993924 */
	// bool hasMatchWith ( Vector <T>* target )
	// {
		// for(unsigned int i=0;i<data.size();++i)
		// {
			// for(unsigned int i2=0;i2<target->data.size();++i2)
			// {

				// if( data.at(i) == target->data.at(i2))
				// {
					// return true;
				// }

			// }
		// }
		// return false;
	// }



	void eraseSlot(const int _slot)
	{
		data.erase(data.begin()+_slot);
	}
	void removeSlot (const int _slot)
	{ eraseSlot(_slot); }
	void deleteSlot (const int _slot)
	{ eraseSlot(_slot); }

	bool erasePosition(const int _slot)
	{
		data.erase(data.begin()+_slot);
		return true;
	}

	bool erase(const T item)
	{
		/* Erase a specific item (match by address). */
		for(unsigned int i=0;i<data.size();++i)
		{
			if(data.at(i)==item)
			{
				data.erase(data.begin()+i);
				return true;
			}
		}
		return false;
	}

	/* Finds the average of a value and the X values surrounding it, for every value in the vector. */
	/* Radius is the range in one direction, therefore the total range is 2*radius. */
	void slidingAverage(int radius)
	{
		/* Copy the old data to an array. */
		if(radius==0) {return;}

		std::cout<<"Sliding average.\n";
		//std::cout<<"vSize: "<<size()<<".\n";
		const long int vSize = size();
		//std::cout<<"making array.\n";
		T * oldValue = new T [vSize];
		//T oldValue [vSize];
		//std::cout<<"copying data.\n";
		for (int i=0;i<vSize;++i)
		{ oldValue[i] = data.at(i); }


		/* Overwrite the vector data with the sliding average. */

		for(int i=0;i<vSize;++i)
		{
			//std::cout<<"For i: "<<i<<".\n";
			//for(int j

			int startIndex = i-radius;
			int endIndex = i+radius;
			int nValues = radius*2;
			double finalValue = 0;

			if(startIndex<0)
			{ startIndex=0; }
			if(endIndex>=vSize)
			{ endIndex=vSize-1; }
			//std::cout<<"Search range: "<<startIndex<<"->"<<endIndex<<".\n";
			const int rangeSize = endIndex-startIndex;
			//std::cout<<"Range size: "<<rangeSize<<".\n";

			//std::cout<<"Calculating avg.\n";
			for(int j=startIndex; j<endIndex;++j)
			{

				/* Doublecheck vector bounds. */
				if(j>=0 && j<vSize)
				{
					//std::cout<<oldValue[j]<<",";
					finalValue+=(double)oldValue[j]/rangeSize;
				}
				else
				{ std::cout<<"DEBUG: Vector::slidingAverage. Shouldn't overflow here.\n"; }
			} //std::cout<<"\n";
			//std::cout<<"Avg to: "<<finalValue<<".\n";
			data.at(i) = finalValue;


		}


	}

	/* This can find the average value of the vector without causing buffer overflows. */
	/* We divide each value by the total number of values, and add them together. Due to
	decimal precision, this is less accurate than using the proper average method. */
	double safeAverage()
	{
		const double nValues = size();
		double finalValue=0;

		for(int i=0;i<nValues;++i)
		{ finalValue+=(data.at(i)/nValues); }

		return finalValue;
	}


	// bool erase(const T* item)
	// {
		// /* Erase a specific item (match by address). */
		// for(unsigned int i=0;i<data.size();++i)
		// {
			// if(data.at(i)==item)
			// {
				// data.erase(data.begin()+i);
				// return true;
			// }
		// }
		// return false;
	// }

	T* find(T* item)
	{
		for(int i=0;i<data.size();++i)
		{
			if(&data.at(i)==item)
			{
				return &data.at(i);
			}
		}
		return 0;
	}
	T find(T item)
	{
		for(int i=0;i<data.size();++i)
		{
			if(data.at(i)==item)
			{
				return data.at(i);
			}
		}
		return 0;
	}

	/* 0221932820 - Added bool find. */
	bool contains(T item)
	{
		for(unsigned int i=0;i<data.size();++i)
		{
			if(data.at(i)==item)
			{
				return true;
			}
		}
		return false;
	}

	/* This function seems to be a pain with polymorphism. This is why T2 is used. */
	/* NOTE: In the case of polymorphism, the general class must call the specialised class. The general class is returned. */
	template <class T2>
	Vector <T>* getCommonVector(Vector <T2>* target)
	{
		Vector <T>* vMatches = new Vector <T>;

		for(unsigned int i=0;i<data.size();++i)
		{
			for(unsigned int i2=0;i2<target->data.size();++i2)
			{
				if( data.at(i) == target->data.at(i2) )
				{
					vMatches->push(data.at(i));
				}
			}

		}
		return vMatches;
	}

	template <class T2>
	int hasClass() /*returns the number of objects of class T2 in this vector*/
	{ verbose "Vector::hasClass():";
		int numInstances=0;
		for(unsigned int i=0;i<data.size();++i)
		{
			if(dynamic_cast <T2> (data.at(i)))
			{ verbose "*";
				++numInstances;
			}
		}
		verbose "end Vector::hasClass()\n";
		return numInstances;
	}

	template <class T2>
	T2 findClass()
	{ verbose "Vector::findClass()\n";
		/* This finds a particular category of object */
		for(unsigned int i=0;i<data.size();++i)
		{
			if(dynamic_cast <T2> (data.at(i)))
			{ verbose "end Vector::findClass(), found instance.\n";
				return (T2) data.at(i);
			}
		}
		verbose "end Vector::findClass(), returning 0.\n";
		return 0;
	}
	template <class T2>
	void clearClass()
	{ verbose "Vector::clearClass(): "; /* finds class T2 and deletes all of em from the vector */
		for(unsigned int i=0;i<data.size();++i)
		{
			if(dynamic_cast <T2> (data.at(i)))
			{ verbose "*";
				data.erase(data.begin()+i);
			}
		}
		verbose " end Vector::findClass()\n";
	}

	template <class T3>
	Vector <T3>* getVectorOf()
	{
		Vector <T3>* vClass = new Vector <T3>;

		for(int i=data.size()-1;i>=0;--i)
		{
			if(dynamic_cast <T3> (data.at(i)))
			{
				vClass->push( (T3) data.at(i));
			}
		}
		return vClass;
	}
	template <class T3>
	void appendVectorOf(Vector <T3>* _v)
	{
		for(int i=data.size()-1;i>=0;--i)
		{
			if(dynamic_cast <T3> (data.at(i)))
			{
				_v->push( (T3) data.at(i));
			}
		}
	}


	void clear()
	{
		data.clear();
	}
	void clearPtr()
	{
		for(int i=data.size()-1;i>=0;--i)
		{
			delete data.at(i);
		}
		data.clear();
	}
	void reserveSpace(const int nSpace)
	{
		data.reserve(nSpace);
	}
	inline bool isEmpty()
	{
		if(data.size()==0)
		{ return true; }
		return false;
	}
};
