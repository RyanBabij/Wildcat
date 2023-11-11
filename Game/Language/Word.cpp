#pragma once
#ifndef WILDCAT_GAME_LANGUAGE_WORD_HPP
#define WILDCAT_GAME_LANGUAGE_WORD_HPP

/* Wildcat: Word
#include <Game/Language/Word.hpp> */

/**
	Class for storing a word with its individual phonemes. This is useful mainly for if you want to mutate a word,
	for example simulating a word evolving over time.
*/

#include <Game/Language/LanguageTools.cpp>
#include <Math/Random/RandomLehmer.hpp>

#include <string>

class Word
{
	Vector <std::string> vPhoneme; // stores the phonemes of the word.

	public:

	Word()
	{
	}
	
	Word(std::string _word)
	{
		//for now if a word is passed we will just break it down into letters
	  for (char letter : _word)
	  {
			vPhoneme.push(std::string(1, letter));
	  }
	}
	
	void clear()
	{
		vPhoneme.clear();
	}
	
	void addPhoneme(std::string phoneme)
	{
		if (phoneme.size() != 0)
		{
			vPhoneme.add(phoneme);
		}
	}

	// Overload the += operator
	Word& operator+=(const std::string& phoneme)
	{
		addPhoneme(phoneme);
		return *this;
	}
	
	// Overload the += operator for a single char
	Word& operator+=(char phonemeChar)
	{
		std::string phoneme(1, phonemeChar); // Convert char to string
		addPhoneme(phoneme);
		return *this;
	}

	// Overload the assignment operator
	Word& operator=(const std::string& phoneme)
	{
		vPhoneme.clear(); // Clear existing phonemes
		addPhoneme(phoneme); // Add the new phoneme
		return *this;
	}

	std::string toString()
	{
		std::string result;
		for (auto& phoneme : vPhoneme)
		{
			result += phoneme;
		}
		return result;
	}
	
	void mutate(RandomLehmer& random, int minLength=3, int maxLength=8)
	{
		// pick a random phoneme and then alter it
		// To start with we'll make it simple and just replace the vowel or consonant with a different vowel or consonant.
		
		int iPhoneme=0;
		if (vPhoneme.size()>1)
		{
			iPhoneme = random.rand8(vPhoneme.size()-1);
		}
		
		std::string randomPhoneme = vPhoneme(iPhoneme);
		//std::cout<<"Mutate "<<randomPhoneme<<"\n";
		
		// For now we will only change single letter phonemes
		if ( vPhoneme(iPhoneme).length() == 1 )
		{
			char c = vPhoneme(iPhoneme)[0];
			
			
			if (LanguageTools::isVowel(c))
			{
				// mutate vowel
				vPhoneme(iPhoneme)=std::string(1, LanguageTools::randomVowelExcept(random,c));
			}
			else
			{
				// mutate consonant
				vPhoneme(iPhoneme)=std::string(1, LanguageTools::randomConsonantExcept(random,c));
			}
			
			
		}
		
	}

};

#endif

