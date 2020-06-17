#pragma once
#ifndef WILDCAT_CONTAINER_WORD_LIST_HPP
#define WILDCAT_CONTAINER_WORD_LIST_HPP

/* WordList.hpp
	#include <Container/WordList/WordList.hpp>
Class to store a list of strings which can be randomly returned and strung together.
*/

#include <Data/Tokenize.hpp>

#include <string>
#include <vector>

class WordList
{
	public:
	
	double rerollChance;
	
	Vector <std::string> vWord;
	
	RandomLehmer rng;

	
	WordList()
	{
		rerollChance=0;
		rng.seed(SEEDER);
	}
	
	void add(const std::string word)
	{
		vWord.push(word);
	}
	
	std::string get()
	{
		return "";
	}
	
	
	// string can be delimited with either newline or spaces. Null strings are ignored.
	void loadString(const std::string input)
	{
		Tokenize tokenizer;
		std::string delimiters = " ,\n\r";
		Vector <std::string> * vToke = tokenizer.tokenize(input,delimiters);
		
		for (int i=0;i<vToke->size();++i)
		{
			vWord.push( (*vToke)(i) );
		}
	}
	
	std::string toString()
	{
		std::string output = "";
		
		for (int i=0;i<vWord.size();++i)
		{
			output+=vWord(i);
			if ( i<vWord.size()-1 )
			{ output+=", "; }
			
		}
		return output;
	}
	
	std::string getRandom()
	{
		if (vWord.size()==0)
		{ return ""; }
		return vWord(rng.rand(vWord.size()));
	}

	
};

#endif
