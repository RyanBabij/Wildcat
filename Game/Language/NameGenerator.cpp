#pragma once
#ifndef WILDCAT_LANGUAGE_NAME_GENERATOR_CPP
#define WILDCAT_LANGUAGE_NAME_GENERATOR_CPP

/* Wildcat: NameGenerator
#include <Game/Language/NameGenerator.cpp> */

/**
	Implementation of NameGenerator.hpp
*/

#include <Game/Language/NameGenerator.hpp>
#include <Math/Random/RandomLehmer.hpp>

#include <string>
#include <cctype> // For toupper()

// Initialize the static constant array
const char NameGenerator::VOWELS[] = {'a', 'e', 'i', 'o', 'u', 's', 'y'};
const size_t NameGenerator::NUM_VOWELS = sizeof(NameGenerator::VOWELS) / sizeof(NameGenerator::VOWELS[0]);

// Initialize the static constant array of consonants
const char NameGenerator::CONSONANTS[] =
{'b','c','d','f','g','h','j','k','l','m','n','p','r','s','t','v','w','y'};
const size_t NameGenerator::NUM_CONSONANTS =
sizeof(NameGenerator::CONSONANTS)/sizeof(NameGenerator::CONSONANTS[0]);

const std::string NameGenerator::DOUBLE_CONSONANTS[] =
{"ch", "sh", "ph", "rh", "wh", "ck", "st", "ll", "qu", "ss", "nz", "ld", "hn", "gg", "gh", "fr", "rx", "rm"};

const size_t NameGenerator::NUM_DOUBLE_CONSONANTS =
sizeof(NameGenerator::DOUBLE_CONSONANTS) / sizeof(NameGenerator::DOUBLE_CONSONANTS[0]);


NameGenerator::NameGenerator()
{
	random.seed();
}

void NameGenerator::seed(RandomLehmer& rng)
{
	random.seed(rng);
}

std::string NameGenerator::generate(unsigned char minLength, unsigned char maxLength, bool capitalise)
{
	name.clear();
	name.reserve(minLength);
	unsigned int nameLength = generateNameLength(minLength,maxLength);

	if(random.rand8(2)==0) { useVowel=false; }
	
	for (size_t i=0;i<nameLength;++i)
	{
		addSound();
		useVowel=!useVowel;
	}
	
	if (capitalise && !name.empty())
	{
		name[0] = std::toupper(name[0]);
	}
	
	name.shrink_to_fit();
	return name;
}

void NameGenerator::addSingleVowel()
{
	// //Evenly weighted vowels.
   name += VOWELS[random.rand8(NUM_VOWELS)];
}

void NameGenerator::addSingleConsonant()
{
	name += CONSONANTS[random.rand8(NUM_CONSONANTS)];
}

void NameGenerator::addDoubleConsonant()
{
	//chshphrhwhckstllqussnzldhnggghfrrxrm
	name += DOUBLE_CONSONANTS[random.rand8(NUM_DOUBLE_CONSONANTS)];
}

void NameGenerator::addSound()
{
	if(useVowel)
	{
		addSingleVowel();
	}
	else
	{
		//addSingleConsonant();
		if(random.rand8(20)==0)
		{
			addDoubleConsonant();
		}
		else
		{
			addSingleConsonant();
		}
	}
}

// Generate a name length as a normal distribution between min and max.
int NameGenerator::generateNameLength(unsigned short int minLength, unsigned short int maxLength)
{
	if (minLength>maxLength)
	{ return 0; }
	
	const unsigned short int nameVariance = maxLength-minLength;
	unsigned char nameLength = minLength;

	for (unsigned char i=0;i<nameVariance;++i)
	{
		if (random.flip())
		{
			++nameLength;
		}
	}
	return nameLength;
}

#endif // WILDCAT_LANGUAGE_NAME_GENERATOR_CPP

