#pragma once
#ifndef WILDCAT_LANGUAGE_NAME_GENERATOR_WORD_HPP
#define WILDCAT_LANGUAGE_NAME_GENERATOR_WORD_HPP

/* Wildcat: NameGeneratorWord
#include <Game/Language/NameGeneratorWord.hpp> */

/**
	Class that generates random names. Works off of a basic vowel/consonant alternation principle, with minor
	modifications. The names should generally be pronouncable. It still needs tweaks to make aesthetically-pleasing
	names, but it's good enough for now.
	This version makes use of the Word class to allow for additional functionality such as mutation.
*/

#include <string>
#include <Math/Random/RandomLehmer.hpp>

#include <Game/Language/Word.cpp>

class NameGeneratorWord
{
		bool useVowel=true;
		
		static const char VOWELS[];
		static const size_t NUM_VOWELS;
		
		static const char CONSONANTS[];
		static const size_t NUM_CONSONANTS;
		
		static const std::string DOUBLE_CONSONANTS[];
		static const size_t NUM_DOUBLE_CONSONANTS;

		RandomLehmer random;
		Word name;

	public:
		NameGeneratorWord();
		
		void seed(RandomLehmer& rng);
		
		Word generate
		(unsigned char minLength = 3, unsigned char maxLength = 8, bool capitalise = true);

		void addSingleVowel();
		void addSingleConsonant();
		void addDoubleConsonant();
		void addSound();
		int generateNameLength(unsigned short int minLength=3, unsigned short int maxLength=8);

		std::string toString();
};

#endif // WILDCAT_LANGUAGE_NAME_GENERATOR_WORD_HPP
