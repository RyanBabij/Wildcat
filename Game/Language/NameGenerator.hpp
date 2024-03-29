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
		bool useVowel=true;

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
		void addSound();
		int generateNameLength(unsigned short int minLength=3, unsigned short int maxLength=8);
		std::string toString();
};

#endif // WILDCAT_LANGUAGE_NAME_GENERATOR_HPP
