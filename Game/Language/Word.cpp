#pragma once
#ifndef WILDCAT_GAME_LANGUAGE_WORD_HPP
#define WILDCAT_GAME_LANGUAGE_WORD_HPP

/* Wildcat: Word
#include <Game/Language/Word.hpp> */

/**
	Class for storing a word with its individual phonemes. This is useful mainly for if you want to mutate a word,
	for example simulating a word evolving over time.
*/

#include <string>
#include <Math/Random/RandomLehmer.hpp>


class Word
{
	RandomLehmer random;
	
	Vector <std::string> vPhoneme; // stores the phonemes of the word.

	public:

	Word()
	{
		random.seed();
	}
	
	void clear()
	{
		vPhoneme.clear();
	}
	
	void seed(RandomLehmer& rng)
	{
		random.seed(rng);
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
	

};

#endif

