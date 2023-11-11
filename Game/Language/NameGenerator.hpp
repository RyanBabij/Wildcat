#pragma once
#ifndef WILDCAT_LANGUAGE_NAME_GENERATOR_HPP
#define WILDCAT_LANGUAGE_NAME_GENERATOR_HPP

/* Wildcat: NameGenerator
#include <Game/Language/NameGenerator.hpp> */

/**
	Class that generates random names. Works off of a basic vowel/consonant alternation principle, with minor
	modifications. The names should generally be pronouncable. It still needs tweaks to make aesthetically-pleasing
	names, but it's good enough for now.
*/

#include <string>
#include <Math/Random/RandomLehmer.hpp>

class NameGenerator
{
		static const unsigned char SOFT = 0;
		static const unsigned char HARD = 1;
		
		static const char VOWELS[];
		static const size_t NUM_VOWELS;
		
		static const char CONSONANTS[];
		static const size_t NUM_CONSONANTS;
		
		static const std::string DOUBLE_CONSONANTS[];
		static const size_t NUM_DOUBLE_CONSONANTS;

		RandomLehmer random;
		std::string name;

	public:
		NameGenerator();
		
		void seed(RandomLehmer& rng);
		
		std::string generate
		(unsigned char minLength = 3, unsigned char maxLength = 8, bool capitalise = true);

		void addSingleVowel();
		void addSingleConsonant();
		void addDoubleConsonant();
		void addSound(bool sound);
		int generateNameLength(unsigned short int minLength=3, unsigned short int maxLength=8);
		std::string toString();
};

#endif // WILDCAT_LANGUAGE_NAME_GENERATOR_HPP
