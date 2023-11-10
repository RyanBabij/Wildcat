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
	name="";
	//Name length... Using a normal distribution.
	unsigned char length=random.multiRoll8(3,4);
	if(length<minLength) { length=minLength; }
	if(length>maxLength) { length=maxLength; }

	bool sound=SOFT;
	if(random.rand8(2)==0) { sound=HARD; }

	unsigned char length2=0;
	while(length2<length)
	{
		addSound(sound);
		sound=!sound;
		length2++;
	}
	if ( capitalise==true )
	{
		name[0]=toupper(name[0]);
	}
	return name;
}

std::string NameGenerator::generateName()
{
	std::string _name="";
	//Starting sound... Biased towards starting with a consonant.
	bool startingSound = true;
	if (random.rand8(2) == 0)
	{
		startingSound = false;
	}

	//male or female name?

	//Find how long the name should be.
	unsigned char nameLength = getNameLength();
	for (unsigned char a = 0; a < nameLength; a++)
	{
		if (startingSound)
		{
			_name += consonants(a);
		}
		if (!startingSound)
		{
			_name += vowels();
		}
		//Alternate between vowels and consonants
		startingSound = !startingSound;
	}
	return _name;
}

void NameGenerator::addSingleVowel()
{
	//Evenly weighted vowels.
	unsigned char chance=random.rand8(6);
	if (chance == 0)
	{ name+="a"; }
	else if (chance == 1)
	{ name+="e"; }
	else if (chance == 2)
	{ name+="i"; }
	else if (chance == 3)
	{ name+="o"; }
	else if (chance == 4)
	{ name+="u"; }
	else if (chance == 5)
	{ name+="s"; }
	else if (chance == 6)
	{ name+="y"; }
}

void NameGenerator::addSingleConsonant()
{
	unsigned char chance=random.rand8(17);
	if     (chance==0) { name+="b"; }
	else if(chance==1) { name+="c"; }
	else if(chance==2) { name+="d"; }
	else if(chance==3) { name+="f"; }
	else if(chance==4) { name+="g"; }
	else if(chance==5) { name+="h"; }
	else if(chance==6) { name+="j"; }
	else if(chance==7) { name+="k"; }
	else if(chance==8) { name+="l"; }
	else if(chance==9) { name+="m"; }
	else if(chance==10) { name+="n"; }
	else if(chance==11) { name+="p"; }
	else if(chance==12) { name+="r"; }
	else if(chance==13) { name+="s"; }
	else if(chance==14) { name+="t"; }
	else if(chance==15) { name+="v"; }
	else if(chance==16) { name+="w"; }
	else if(chance==17) { name+="y"; }
}

void NameGenerator::addDoubleConsonant()
{
	//chshphrhwhckstllqussnzldhnggghfrrxrm
	unsigned char chance=random.rand8(18);

	//ch
	if(chance==0)
	{ name+="ch"; }
	//sh
	else if(chance==1)
	{ name+="sh"; }
	//ph
	else if(chance==2)
	{ name+="ph"; }
	//rh
	else if(chance==3)
	{ name+="rh"; }
	//wh
	else if(chance==4)
	{ name+="wh"; }
	//ck
	else if(chance==5)
	{ name+="ck"; }
	//st
	else if(chance==6)
	{ name+="st"; }
	//ll
	else if(chance==7)
	{ name+="ll"; }
	//qu
	else if(chance==8)
	{ name+="qu"; }
	//ss
	else if(chance==9)
	{ name+="ss"; }
	//nz
	else if(chance==10)
	{ name+="nz"; }
	//ld
	else if(chance==11)
	{ name+="ld"; }
	//hn
	else if(chance==12)
	{ name+="hn"; }
	//gg
	else if(chance==13)
	{ name+="gg"; }
	//gh
	else if(chance==14)
	{ name+="gh";}
	//fr
	else if(chance==15)
	{ name+="fr"; }
	//rx
	else if(chance==16)
	{ name+="rx"; }
	//rm
	else if(chance==17)
	{ name+="rm"; }
}

void NameGenerator::addSound(bool sound)
{
	if(sound==SOFT)
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

int NameGenerator::getNameLength()
{
	unsigned char nameLength = 3;

	/* Shift bellcurve over by 1. */
	nameLength+=random.rand8(1);

	for (unsigned char i=0;i<5;++i)
	{
		if(random.flip())
		{ ++nameLength;
		}
		else
		{
			return nameLength;
		}
	}
	return nameLength;
}

std::string NameGenerator::consonants(unsigned short int a)
{
	char consonantList[] = "bcdfghjklmnprstvwxz";
	char doubleConsonantList[] = "chshphrhwhckstllqussnzldhnggghfrrxrm";
	std::string nameSegment = "";
	//if (random.randInt(20) == 0)
	if (0)
	{
		//Double consonant
		unsigned char consonant = makeEven(random.rand8(sizeof(doubleConsonantList) - 2));
		//Rerolls
		if (doubleConsonantList[consonant] == 'g' && doubleConsonantList[consonant+1] == 'h' && a == 0)
		{
			consonant = makeEven(random.rand8(sizeof(consonantList)-2));
		}
		nameSegment = doubleConsonantList[consonant];
		nameSegment += doubleConsonantList[consonant+1];
	}
	else
	{
		unsigned char consonant = random.rand8(sizeof(consonantList)-2);
		//Reroll some single consonants

		if (consonantList[consonant] == 'x' || consonantList[consonant] == 'z' || consonantList[consonant] == 'w' ||(consonantList[consonant] == 'j' && a == 0))
		{
			/* 022-172. Removed 1/16 chance of keeping. */
			//if (random.randInt(16) != 0)
			//{
				consonant = random.rand8(sizeof(consonantList)-2);
			//}
		}
		nameSegment = consonantList[consonant];
	}
	return nameSegment;
}

int NameGenerator::makeEven(int number)
{
	if (number % 2 != 0)
	{
		number--;
	}
	return number;
}

std::string NameGenerator::vowels()
{
	char vowelList[] = "aeiouy";
	char doubleVowelList[] = "ooeeaeoeyaeiiaieyoyu";
	std::string nameSegment = "";
	if (random.rand8(26) == 0)
	{
		//Double vowels
		unsigned char vowel = makeEven(random.rand8(sizeof(doubleVowelList)-2));
		nameSegment = doubleVowelList[vowel];
		nameSegment += doubleVowelList[vowel+1];
	}
	else
	{
		unsigned char vowel = random.rand8(sizeof(vowelList)-2);
		if (vowelList[vowel] == 'y')
		{
			if (nameSegment == "")
			{
				if (random.rand8(20) != 0)
				{
					vowel = random.rand8(sizeof(vowelList)-2);
				}
			}
			else
			{
				if (random.rand8(3) != 0)
				{
					vowel = random.rand8(sizeof(vowelList)-2);
				}
			}
		}
		nameSegment = vowelList[vowel];
	}
	return nameSegment;
}

#endif // WILDCAT_LANGUAGE_NAME_GENERATOR_CPP

