#pragma once
#ifndef WILDCAT_ARG_READER_HPP
#define WILDCAT_ARG_READER_HPP

#include <Container/Vector/Vector.hpp> // FOR HOLDING VECTOR OF ARGS.

#include <Data/DataTools.hpp>

//#include <boost/algorithm/string/predicate.hpp>

/*
	ArgReader
	#include <Misc/ArgReader.hpp>

	* PROBABLY FILE THIS IN LIB/DATA.

	Passing args through the console is pretty cool. This class makes it less painful to utilise them.

	EXAMPLE CODE:

		int main (int nArgs, char ** arg )
		{
			ArgReader argReader;
			argReader.feed(nArgs,arg);

			if ( argReader.hasTag("-help") || argReader.hasTag("--help") )
			{
				std::cout<<"\n  Output help text.\n";
				return 0;
			}
			return 0;
		}


*/


class ArgReader
{
	public:

		// THE RAW ARGS ARE STORED HERE. CONVERTED TO STRING BECAUSE IT'S EASIER FOR ME.
	Vector <std::string> vArg;

	ArgReader()
	{
	}

	ArgReader(const int _nArgs, char ** arg)
	{
		feed(_nArgs,arg);
	}

	void feed(const int _nArgs, char ** arg)
	{
		for ( int i=0;i<_nArgs;++i )
		{
			vArg.push(arg[i]);
		}
	}

	bool noArgs()
	{
		return (vArg.size()==1);
	}

	int getIntTag(std::string _tagName)
	{
		// MAKE SURE TO VERIFY IT'S A NUMBER.

		for ( int i=0;i<vArg.size();++i)
		{
			if (vArg(i)==_tagName)
			{
				if ( vArg.isSafe(i+1) )
				{
					return DataTools::toInt(vArg(i+1));
				}
			}
		}
		return -1;
	}
	std::string getStringTag (std::string _tag)
	{
		for ( int i=0;i<vArg.size();++i)
		{
			if (vArg(i)==_tag)
			{
				if ( vArg.isSafe(i+1) )
				{
					return vArg(i+1);
				}
			}
		}
		return "";
	}

		// Need to implement case-insensitive version of this.
	bool hasTag(std::string _tag)
	{
		for ( int i=0;i<vArg.size();++i)
		{
			if (vArg(i)==_tag)
			{
				return true;
			}
		}
		return false;
	}


	void print()
	{
		std::cout<<"Printing args.\n";
		for ( int i=0;i<vArg.size();++i)
		{
			std::cout<<vArg(i)<<"\n";
		}std::cout<<"\n";
	}
};

#endif
