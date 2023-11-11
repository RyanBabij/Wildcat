#pragma once
#ifndef WILDCAT_LANGUAGE_LANGUAGE_TOOLS_HPP
#define WILDCAT_LANGUAGE_LANGUAGE_TOOLS_HPP

/* Wildcat: LanguageTools
#include <Game/Language/LanguageTools.hpp> */

/**
	Library for miscellaneous common functions involving languages.
*/

#include <string>
#include <cstddef> // For size_t

namespace LanguageTools
{
	extern const char VOWELS[];
	extern const size_t NUM_VOWELS;

	extern const char CONSONANTS[];
	extern const size_t NUM_CONSONANTS;

	extern const std::string DOUBLE_CONSONANTS[];
	extern const size_t NUM_DOUBLE_CONSONANTS;
	
	bool isVowel(char ch);
	bool isVowel(const std::string& str);
		// Return the vowel if the passed string is a vowel, otherwise return 0.
	char isVowelGet(const std::string& str);
	
	char randomVowel(RandomLehmer& rng);
	char randomVowelExcept(RandomLehmer& rng, char vowel);
	
	bool isConsonant(char ch);
	bool isConsonant(const std::string& str);
		// Return the vowel if the passed string is a vowel, otherwise return 0.
	char isConsonantGet(const std::string& str);
	
	char randomConsonant(RandomLehmer& rng);
	char randomConsonantExcept(RandomLehmer& rng, char vowel);
}

#endif // WILDCAT_LANGUAGE_LANGUAGE_TOOLS_HPP
