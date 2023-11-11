#pragma once
#ifndef WILDCAT_LANGUAGE_LANGUAGE_TOOLS_CPP
#define WILDCAT_LANGUAGE_LANGUAGE_TOOLS_CPP

/* Wildcat: LanguageTools
#include <Game/Language/LanguageTools.cpp> */

#include "LanguageTools.hpp"

namespace LanguageTools
{
	// Initialize the static constant array
	const char VOWELS[] = {'a', 'e', 'i', 'o', 'u', 's', 'y'};
	const size_t NUM_VOWELS = sizeof(VOWELS) / sizeof(VOWELS[0]);

	// Initialize the static constant array of consonants
	const char CONSONANTS[] = {'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 'n', 'p', 'r', 's', 't', 'v', 'w', 'y'};
	const size_t NUM_CONSONANTS = sizeof(CONSONANTS) / sizeof(CONSONANTS[0]);

	// Initialize the static constant array of double consonants
	const std::string DOUBLE_CONSONANTS[] = {"ch", "sh", "ph", "rh", "wh", "ck", "st", "ll", "qu", "ss", "nz", "ld", "hn", "gg", "gh", "fr", "rx", "rm"};
	const size_t NUM_DOUBLE_CONSONANTS = sizeof(DOUBLE_CONSONANTS) / sizeof(DOUBLE_CONSONANTS[0]);


	bool isVowel(char ch)
	{
		ch = tolower(ch); // Convert to lowercase to handle uppercase input

		for (char vowel : VOWELS)
		{
			if (ch == vowel)
			{
				return true; // Return true if the character matches a vowel
			}
		}
		return false; // Return false if no match is found
	}
	
	bool isVowel(const std::string& str)
	{
		if (str.length() != 1)
		{
			return false; // Return false if the string is not a single character
		}
		return isVowel(str[0]);
	}
	
	char isVowelGet(const std::string& str)
	{
		if (str.length() != 1)
		{
			return 0; // Return false if the string is not a single character
		}

		char ch = tolower(str[0]); // Convert to lowercase to handle uppercase input

		for (char vowel : VOWELS)
		{
			if (ch == vowel)
			{
				return ch; // Return true if the character matches a vowel
			}
		}

		return 0; // Return false if no match is found
	}
	
	char randomVowel(RandomLehmer& rng)
	{
		return LanguageTools::VOWELS[rng.rand8(LanguageTools::NUM_VOWELS)];
	}
	char randomVowelExcept(RandomLehmer& rng, char excludeVowel)
	{
		char vowel;
		do
		{
			vowel = randomVowel(rng);
		}
		while (vowel == excludeVowel);

		return vowel;
	}
	
	
	bool isConsonant(char ch)
	{
		ch = tolower(ch); // Convert to lowercase to handle uppercase input

		for (char consonant : CONSONANTS)
		{
			if (ch == consonant)
			{
				return true; // Return true if the character matches a consonant
			}
		}
		return false; // Return false if no match is found
	}
	bool isConsonant(const std::string& str)
	{
		if (str.length() != 1)
		{
			return false; // Return false if the string is not a single character
		}
		return isConsonant(str[0]);
	}
	char isConsonantGet(const std::string& str)
	{
		if (str.length() != 1)
		{
			return 0; // Return false if the string is not a single character
		}

		char ch = tolower(str[0]); // Convert to lowercase to handle uppercase input

		for (char consonant : CONSONANTS)
		{
			if (ch == consonant)
			{
				return ch; // Return true if the character matches a consonant
			}
		}

		return 0; // Return false if no match is found
	}
	
	char randomConsonant(RandomLehmer& rng)
	{
		return LanguageTools::CONSONANTS[rng.rand8(LanguageTools::NUM_CONSONANTS)];
	}
	char randomConsonantExcept(RandomLehmer& rng, char excludeConsonant)
	{
		char consonant;
		do
		{
			consonant = randomConsonant(rng);
		}
		while (consonant == excludeConsonant);

		return consonant;
	}

}

#endif // WILDCAT_LANGUAGE_LANGUAGE_TOOLS_CPP
