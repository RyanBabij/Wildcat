#pragma once
#ifndef WILDCAT_NAMEGENERATOR_NAME_GENERATOR_HPP
#define WILDCAT_NAMEGENERATOR_NAME_GENERATOR_HPP

#include <string>
#include <Math/Random/RandomLehmer.hpp>

class NameGenerator
{
	static const unsigned char SOFT = 0;
	static const unsigned char HARD = 1;

	RandomLehmer random;
	std::string name;

public:
	NameGenerator();
	
	void seed(RandomLehmer& rng);
	
	std::string generate
	(unsigned char minLength = 1, unsigned char maxLength = 100, bool capitalise = true);
	
	std::string generateName();
	void addSingleVowel();
	void addSingleConsonant();
	void addDoubleConsonant();
	void addSound(bool sound);
	int getNameLength();
	std::string consonants(unsigned short int a);
	int makeEven(int number);
	std::string vowels();
};

#endif // WILDCAT_NAMEGENERATOR_NAME_GENERATOR_HPP
