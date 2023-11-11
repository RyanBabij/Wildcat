#pragma once
#ifndef WILDCAT_GAME_LANGUAGE_LANGUAGE_HPP
#define WILDCAT_GAME_LANGUAGE_LANGUAGE_HPP

/* Wildcat: Language
#include <Game/Language/Language.hpp> */

/**
	Class for maintaining different game languages. Simply a dictionary lookup at this stage with support for word mutations.
*/

#include <Game/Language/Word.cpp>

#include <string>
#include <Math/Random/RandomLehmer.hpp>


class Language
{
	RandomLehmer random;
	
	Vector <Word> vWord;
	Vector <std::string> vOriginal;
	
	Word errorWord;

	public:

	Language()
	{
		random.seed();
		errorWord.addPhoneme("<ERROR>");
	}
	
	void seed(RandomLehmer& rng)
	{
		random.seed(rng);
	}
	
	Word& operator()(const std::string& original)
	{
		for (int i = 0; i < vOriginal.size(); ++i)
		{
			if (vOriginal(i) == original)
			{
				return vWord(i);
			}
		}
		return errorWord;
	}
	
	void add(Word word, std::string original)
	{
		vWord.push(word);
		vOriginal.push(original);
	}
	
	void addWords(std::string words)
	{
		std::istringstream iss(words);
		std::string word;
		while (iss >> word)
		{
			if (vOriginal.contains(word) == false)
			{
				// Assuming Word has a constructor that takes a std::string
				Word newWord(word);
				vWord.push(newWord);

				// If you also want to keep track of the original form (which is just the word itself in this case)
				vOriginal.push(word);
			}
		}
	}
	
	void mutate (const std::string& original)
	{
		for (int i = 0; i < vOriginal.size(); ++i)
		{
			if (vOriginal(i) == original)
			{
				vWord(i).mutate(random);
				return;
			}
		}
	}
	
	void mutateAll ()
	{
		for (int i = 0; i < vOriginal.size(); ++i)
		{
			vWord(i).mutate(random);
		}
	}
	
	
	void printAll()
	{
		for (int i=0;i<vWord.size();++i)
		{
			std::cout<<vWord(i).toString()<<" ";
		} std::cout<<"\n";
	}
	


};

#endif

