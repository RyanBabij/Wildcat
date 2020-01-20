#pragma once
#ifndef WILDCAT_NAMEGEN_NAMEGEN2_HPP
#define WILDCAT_NAMEGEN_NAMEGEN2_HPP

/*
	#include <NameGen/NameGen.hpp> Created: <0223355932. Updated: 0223355932. Component of GaroSoft WildCat game engine.
	This class generates names from a hardcoded list of first and last names.
*/

#include <string>
#include <Math/Random/GlobalRandom.hpp>
#include <Container/Vector/Vector.hpp>

class NameList
{
	public:
	
	std::string cultureName;
	
	Vector <std::string> vMaleFirstName;
	Vector <std::string> vFemaleFirstName;
	Vector <std::string> vLastName;
	
	NameList(std::string _cultureName = "?")
	{
		cultureName=_cultureName;
	}
	// First names
	void addMaleFirstName(std::string _firstName)
	{
		vMaleFirstName.push(_firstName);
	}
	void addFemaleFirstName(std::string _firstName)
	{
		vFemaleFirstName.push(_firstName);
	}
	
	void addSurname (std::string _surname)
	{
		vLastName.push(_surname);
	}
	
	std::string getMaleFirstName()
	{
		return vMaleFirstName(Random::randomInt(vMaleFirstName.size()-1));
	}
	std::string getFemaleFirstName()
	{
		return vFemaleFirstName(Random::randomInt(vFemaleFirstName.size()-1));
	}
	std::string getSurname()
	{
		return vLastName(Random::randomInt(vLastName.size()-1));
	}
};

void NameGen_Init()
{
	NameList nameList01 ("Australia");

	nameList01.addMaleFirstName("Oliver");
	nameList01.addMaleFirstName("Noah");
	nameList01.addMaleFirstName("William");
	nameList01.addMaleFirstName("Lachlan");
	nameList01.addMaleFirstName("Ethan");
	nameList01.addMaleFirstName("Jack");
	nameList01.addMaleFirstName("Lucas");
	nameList01.addMaleFirstName("Charlie");
	nameList01.addMaleFirstName("Joshua");
	nameList01.addMaleFirstName("Thomas");
	nameList01.addMaleFirstName("Max");
	nameList01.addMaleFirstName("Riley");
	nameList01.addMaleFirstName("Jacob");
	nameList01.addMaleFirstName("Aiden");
	nameList01.addMaleFirstName("Liam");
	nameList01.addMaleFirstName("Jackson");
	nameList01.addMaleFirstName("Cooper");
	nameList01.addMaleFirstName("Alexander");
	nameList01.addMaleFirstName("Xavier");
	nameList01.addMaleFirstName("Benjamin");

	nameList01.addFemaleFirstName("Chloe");
	nameList01.addFemaleFirstName("Sophie");
	nameList01.addFemaleFirstName("Ruby");
	nameList01.addFemaleFirstName("Charlotte");
	nameList01.addFemaleFirstName("Olivia");
	nameList01.addFemaleFirstName("Lily");
	nameList01.addFemaleFirstName("Ava");
	nameList01.addFemaleFirstName("Ella");
	nameList01.addFemaleFirstName("Emily");
	nameList01.addFemaleFirstName("Mia");
	nameList01.addFemaleFirstName("Sienna");
	nameList01.addFemaleFirstName("Amelia");
	nameList01.addFemaleFirstName("Isabella");
	nameList01.addFemaleFirstName("Lucy");
	nameList01.addFemaleFirstName("Zoe");
	nameList01.addFemaleFirstName("Grace");
	nameList01.addFemaleFirstName("Isabelle");
	nameList01.addFemaleFirstName("Maddison");
	nameList01.addFemaleFirstName("Emma");
	nameList01.addFemaleFirstName("Charlie");
	nameList01.addFemaleFirstName("Hannah");
	
	nameList01.addSurname("Smith");
	nameList01.addSurname("Jones");
	nameList01.addSurname("Williams");
	nameList01.addSurname("Brown");
	nameList01.addSurname("Wilson");
	nameList01.addSurname("Taylor");
	nameList01.addSurname("Nguyen");
	nameList01.addSurname("Johnson");
	nameList01.addSurname("Martin");
	nameList01.addSurname("White");
	

	NameList nameList02 ("China");
	
	nameList02.addMaleFirstName("Aiguo");
	nameList02.addMaleFirstName("An");
	nameList02.addMaleFirstName("Angúo");
	nameList02.addMaleFirstName("Bai");
	nameList02.addMaleFirstName("Bingwen");
	nameList02.addMaleFirstName("Bo");
	nameList02.addMaleFirstName("Bohai");
	nameList02.addMaleFirstName("Bojing");
	nameList02.addMaleFirstName("Bolin");
	nameList02.addMaleFirstName("Boqin");
	nameList02.addMaleFirstName("Changpu");
	nameList02.addMaleFirstName("Chanming");
	nameList02.addMaleFirstName("Chao");
	nameList02.addMaleFirstName("Chaoxiang");
	nameList02.addMaleFirstName("Chen");
	nameList02.addMaleFirstName("Cheng");
	nameList02.addMaleFirstName("Chenglei");
	nameList02.addMaleFirstName("Cheung");
	nameList02.addMaleFirstName("Chi");
	nameList02.addMaleFirstName("Chongan");
	nameList02.addMaleFirstName("Chongkun");
	nameList02.addMaleFirstName("Chonglin");
	nameList02.addMaleFirstName("Chuanli");
	nameList02.addMaleFirstName("Chung");
	nameList02.addMaleFirstName("Délì");
	nameList02.addMaleFirstName("Déshì");
	nameList02.addMaleFirstName("Déwei");
	nameList02.addMaleFirstName("Da");
	nameList02.addMaleFirstName("Dai");
	nameList02.addMaleFirstName("Delun");
	nameList02.addMaleFirstName("Deming");
	nameList02.addMaleFirstName("Dingbang");
	nameList02.addMaleFirstName("Dingxiang");
	nameList02.addMaleFirstName("Dong");
	nameList02.addMaleFirstName("Donghai");
	nameList02.addMaleFirstName("Duyi");
	nameList02.addMaleFirstName("Enlai");
	nameList02.addMaleFirstName("Fa");
	nameList02.addMaleFirstName("Fai");
	nameList02.addMaleFirstName("Fang");
	nameList02.addMaleFirstName("Feng");
	nameList02.addMaleFirstName("Fengge");
	nameList02.addMaleFirstName("Fu");
	nameList02.addMaleFirstName("Fuhua");
	nameList02.addMaleFirstName("Gan");
	nameList02.addMaleFirstName("Gang");
	
	
}



// class NameGen
// {

	// std::string name;
	
	// public:
	
	// NameGen()
	// {
		// name = "";
	// }
	
	// std::string generate(std::string strCulture="?")
	// {
		// name = "";
	
		// return name;
	// }
	
// };
	
#endif