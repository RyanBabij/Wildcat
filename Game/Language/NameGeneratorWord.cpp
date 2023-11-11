#pragma once
#ifndef WILDCAT_LANGUAGE_NAME_GENERATOR_WORD_CPP
#define WILDCAT_LANGUAGE_NAME_GENERATOR_WORD_CPP

/* Wildcat: NameGeneratorWord
#include <Game/Language/NameGeneratorWord.cpp> */

/**
	Implementation of NameGeneratorWord.hpp
*/

#include <Game/Language/NameGeneratorWord.hpp>
#include <Math/Random/RandomLehmer.hpp>

#include <string>
#include <cctype> // For toupper()

NameGeneratorWord::NameGeneratorWord()
{
	random.seed();
}

void NameGeneratorWord::seed(RandomLehmer& rng)
{
	random.seed(rng);
}

Word NameGeneratorWord::generate(unsigned char minLength, unsigned char maxLength, bool capitalise)
{
	name.clear();
	//name.reserve(minLength);
	unsigned int nameLength = generateNameLength(minLength,maxLength);

	if(random.rand8(2)==0) { useVowel=false; }
	
	for (size_t i=0;i<nameLength;++i)
	{
		addSound();
		useVowel=!useVowel;
	}
	
	// if (capitalise && !name.empty())
	// {
		// name[0] = std::toupper(name[0]);
	// }
	
	//name.shrink_to_fit();
	return name;
}

void NameGeneratorWord::addSingleVowel()
{
	// //Evenly weighted vowels.
   name += LanguageTools::VOWELS[random.rand8(LanguageTools::NUM_VOWELS)];
}

void NameGeneratorWord::addSingleConsonant()
{
	name += LanguageTools::CONSONANTS[random.rand8(LanguageTools::NUM_CONSONANTS)];
}

void NameGeneratorWord::addDoubleConsonant()
{
	//chshphrhwhckstllqussnzldhnggghfrrxrm
	name += LanguageTools::DOUBLE_CONSONANTS[random.rand8(LanguageTools::NUM_DOUBLE_CONSONANTS)];
}

void NameGeneratorWord::addSound()
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
int NameGeneratorWord::generateNameLength(unsigned short int minLength, unsigned short int maxLength)
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

std::string NameGeneratorWord::toString()
{
	return name.toString();
}

#endif // WILDCAT_LANGUAGE_NAME_GENERATOR_WORD_CPP
