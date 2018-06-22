#pragma once
#ifndef WILDCAT_NAMEGEN_NAMEGEN_HPP
#define WILDCAT_NAMEGEN_NAMEGEN_HPP

/*
	#include <NameGen/NameGen.hpp> Created: <022-171. Updated: 0223355932. Component of GaroSoft WildCat game engine.
	Class that generates random names. Works off of a basic vowel/consonant alternation principle, with minor modifications. The names are typically pronouncable, but I don't consider this good enough for release yet. However, it is still pretty decent.
*/

#include <string>
#include <Math/Random/GlobalRandom.hpp>


class NameGen
{
	static const unsigned char SOFT=0;
	static const unsigned char HARD=1;
	//MTRand rnd;
	
	RandomNonStatic random;

	std::string name;

	public:

	NameGen()
	{
		random.seed();
	}

	std::string generate(const int minLength=1, const int maxLength=100, bool capitalise=true)
	{
		name="";
		//Name length... Using a normal distribution.
		//int length= (int)rnd.randNorm(7,2);
		//int length=(int)random.randNorm(7,1.5);
		int length=(int)random.multiRoll(3,4);
		if(length<minLength) { length=minLength; }
		if(length>maxLength) { length=maxLength; }

		bool sound=SOFT;
		if(random.randInt(2)==0) { sound=HARD; }

		int length2=0;
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

	std::string generateName()
	{
		std::string _name="";
		//Starting sound... Biased towards starting with a consonant.
		bool startingSound = true;
		if (random.randInt(2) == 0)
		{
			startingSound = false;
		}


		//male or female name?

		//Find how long the name should be.
		unsigned short int nameLength = getNameLength();
		for (unsigned short int a = 0; a < nameLength; a++)
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

	void addSingleVowel()
	{
		//Evenly weighted vowels.
		unsigned int chance=random.randInt(6);
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
	void addSingleConsonant()
	{
		unsigned int chance=random.randInt(17);
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


// 		unsigned int chance=random.randInt(20);
// 		if(chance==0) { name+="b"; }
// 		else if(chance==1) { name+="c"; }
// 		else if(chance==2) { name+="d"; }
// 		else if(chance==3) { name+="f"; }
// 		else if(chance==4) { name+="g"; }
// 		else if(chance==5) { name+="h"; }
// 		else if(chance==6) { name+="j"; }
// 		else if(chance==7) { name+="k"; }
// 		else if(chance==8) { name+="l"; }
// 		else if(chance==9) { name+="m"; }
// 		else if(chance==10) { name+="n"; }
// 		else if(chance==11) { name+="p"; }
// 		else if(chance==12) { name+="qu"; }
// 		else if(chance==13) { name+="r"; }
// 		else if(chance==14) { name+="s"; }
// 		else if(chance==15) { name+="t"; }
// 		else if(chance==16) { name+="v"; }
// 		else if(chance==17) { name+="w"; }
// 		else if(chance==18) { name+="x"; }
// 		else if(chance==19) { name+="y"; }
// 		else if(chance==20) { name+="z"; }
	}
	void addDoubleConsonant()
	{
		//chshphrhwhckstllqussnzldhnggghfrrxrm
		unsigned int chance=random.randInt(18);

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

	/* letters that can go after a:
		a
		b
		c
		d
		e
		f
		 etc....

		 just do this for all 26 letters.


	*/

	void addSound(bool sound)
	{
//std::cout<<"as\n";
		if(sound==SOFT)
		{
			addSingleVowel();
		}
		else
		{
			//addSingleConsonant();
			if(random.randInt(20)==0)
			//if(0)
			{
				addDoubleConsonant();
			}
			else
			{
				addSingleConsonant();
			}
		}
	}




	int getNameLength()
	{
	    //wanna do an almost infinite bellcurve on this.
	    /*unsigned short int nameLength=3;
	    while(rnd.randInt(3)!=0)
	    {
	        nameLength++;
	    }
	    return nameLength;*/



		/* 022-172 NOTE: Changed name length algo. This seems to approximate a kind of bell curve, which seems to work well. */
			//unsigned short int nameLength = random.randInt(3) + 3;
		unsigned short int nameLength = 3;

		/* Shift bellcurve over by 1. */
		nameLength+=random.randomInt(1);

		for (int i=0;i<5;++i)
		{
			int extend = random.randInt(1);
			if(extend==0)
			{ ++nameLength;
			}
			else
			{
				return nameLength;
			}
		}


		// unsigned short int modifier = random.randInt(110);
		// if (modifier <= 40)
		// {
			// nameLength += random.randInt(2);
		// }
		// if (modifier >= 41 && modifier <= 45)
		// {
			// nameLength += random.randInt(4);
		// }
		// if (modifier == 46)
		// {
			// nameLength += random.randInt(8);
		// }
		// if (modifier == 100)
		// {
			// nameLength -= random.randInt(2);
		// }
		// if (modifier == 101)
		// {
			// nameLength --;
		// }
		return nameLength;

	}
	std::string consonants(unsigned short int a)
	{
		char consonantList[] = "bcdfghjklmnprstvwxz";
		char doubleConsonantList[] = "chshphrhwhckstllqussnzldhnggghfrrxrm";
		std::string nameSegment = "";
		//if (random.randInt(20) == 0)
		if (0)
		{
			//Double consonant
			unsigned char consonant = makeEven(random.randInt(sizeof(doubleConsonantList) - 2));
			//Rerolls
			if (doubleConsonantList[consonant] == 'g' && doubleConsonantList[consonant+1] == 'h' && a == 0)
			{
				consonant = makeEven(random.randInt(sizeof(consonantList)-2));
			}
			nameSegment = doubleConsonantList[consonant];
			nameSegment += doubleConsonantList[consonant+1];
		}

		else
		{
			unsigned char consonant = random.randInt(sizeof(consonantList)-2);
			//Reroll some single consonants


			if (consonantList[consonant] == 'x' || consonantList[consonant] == 'z' || consonantList[consonant] == 'w' ||(consonantList[consonant] == 'j' && a == 0))
			{
				/* 022-172. Removed 1/16 chance of keeping. */
				//if (random.randInt(16) != 0)
				//{
					consonant = random.randInt(sizeof(consonantList)-2);
				//}
			}
			nameSegment = consonantList[consonant];
		}
		return nameSegment;
	}
	int makeEven(int number)
	{
		if (number % 2 != 0)
		{
			number--;
		}
		return number;
	}
	//std::string vowels (unsigned short int a)
	std::string vowels ()
	{
        char vowelList[] = "aeiouy";
		char doubleVowelList[] = "ooeeaeoeyaeiiaieyoyu";
		std::string nameSegment = "";
		if (random.randInt(26) == 0)
		{
			//Double vowels
			unsigned char vowel = makeEven(random.randInt(sizeof(doubleVowelList)-2));
			nameSegment = doubleVowelList[vowel];
			nameSegment += doubleVowelList[vowel+1];
		}
		else
		{
			unsigned char vowel = random.randInt(sizeof(vowelList)-2);
			if (vowelList[vowel] == 'y')
			{
				if (nameSegment == "")
				{
					if (random.randInt(20) != 0)
					{
						vowel = random.randInt(sizeof(vowelList)-2);
					}
				}
				else
				{
					if (random.randInt(3) != 0)
					{
						vowel = random.randInt(sizeof(vowelList)-2);
					}
				}
			}
			nameSegment = vowelList[vowel];
		}
		return nameSegment;
	}
};

#endif

