/*
	Keyboard.hpp
	#include <Device/Keyboard/Keyboard.hpp>
	
	Class to portably keep track of the keyboard state.
	
	NOTE: THIS THING IS STILL INCOMPLETE AND KINDA DODGY.
*/

#pragma once

#include <string>

class Keyboard
{
    static const int NUMBER_OF_KEYS=1000; //SET TO A SAFE VALUE FOR NOW, UNTIL THE CLASS IS COMPLETE.
    static const int SPECIAL_KEY_MODIFIER =127;
    bool keyMap[NUMBER_OF_KEYS];
    public:
	
	char lastKey;
	int numKeysDown;
	
	bool keyWasPressed;
	bool keyWasUnpressed;
	
	//Keep a history of keypresses.

		// YEAH I DON'T REALLY GET THIS EITHER. APPARENTLY CTRL COMBOS GET THEIR OWN NUMBER. THERE SEEMS TO BE NO CASE DISTINCTION EITHER, MAKING OVERRIDES DIFFICULT. BUT IN FUTURE I MIGHT OVERRIDE THIS BEHAVIOUR. ANOTHER ANNOYANCE IS THAT THERE IS NO DISTINCTION BETWEEN LEFT CTRL AND RIGHT CTRL. MAYBE THERE IS SOME KIND OF STANDARD SAYING THAT THERE SHOULD BE NO DISTINCTION.

	static const int CTRL_A = 1;
	static const int CTRL_B = 2;
	static const int CTRL_C = 3;
	static const int CTRL_D = 4;
	static const int CTRL_Q = 17;
	static const int CTRL_U = 21;

    static const int A=65;
    static const int B=66;
    static const int C=67;
    static const int D=68;
    static const int E=69;
    static const int F=70;
    static const int G=71;
    static const int H=72;
    static const int I=73;
    static const int J=74;
    static const int K=75;
    static const int L=76;
    static const int M=77;
    static const int N=78;
    static const int O=79;
    static const int P=80;
    static const int Q=81;
    static const int R=82;
    static const int S=83;
    static const int T=84;
    static const int U=85;
    static const int V=86;
    static const int W=87;
    static const int X=88;
    static const int Y=89;
    static const int Z=90;

    static const int a=97;
    static const int b=98;
    static const int c=99;
    static const int d=100;
    static const int e=101;
    static const int f=102;
    static const int g=103;
    static const int h=104;
    static const int i=105;
    static const int j=106;
    static const int k=107;
    static const int l=108;
    static const int m=109;
    static const int n=110;
    static const int o=111;
    static const int p=112;
    static const int q=113;
    static const int r=114;
    static const int s=115;
    static const int t=116;
    static const int u=117;
    static const int v=118;
    static const int w=119;
    static const int x=120;
    static const int y=121;
    static const int z=122;

    static const int ZERO=48;
    static const int ONE=49;
    static const int TWO=50;
    static const int THREE=51;
    static const int FOUR=52;
    static const int FIVE=53;
    static const int SIX=54;
    static const int SEVEN=55;
    static const int EIGHT=56;
    static const int NINE=57;

    static const int GRAVE_ACCENT=96;
    static const int TILDE=126;
    static const int EXCLAMATION=33;
    static const int AT=64;
    static const int HASH=35;
    static const int NUMBER_SIGN=35;
    static const int DOLLAR=36;
    static const int PERCENT=37;
    static const int CARET=94;
    static const int AMPERSAND=38;
    static const int ASTERISK=42;
    static const int LEFT_BRACKET=40;
    static const int RIGHT_BRACKET=41;
    static const int HYPHEN=45;
    static const int MINUS=45;
    static const int EQUALS=61;
    static const int BACKSLASH=92;
    static const int UNDERSCORE=95;
    static const int PLUS=43;
    static const int PIPE=124;
    static const int LEFT_SQUARE_BRACKET=91;
    static const int RIGHT_SQUARE_BRACKET=93;
    static const int LEFT_BRACE=123;
    static const int RIGHT_BRACE=125;
    static const int COMMA=44;
    static const int PERIOD=46;
    static const int LEFT_CHEVRON=60;
    static const int RIGHT_CHEVRON=62;
    static const int SOLIDUS=47;
    static const int SLASH=47;
    static const int QUESTION_MARK=63;
    static const int QUERY=63;
    static const int SEMICOLON=59;
    static const int APPOSTROPHE=39;
    static const int COLON=58;
    static const int QUOTATION=34;

    static const int ESCAPE=27;
    static const int ESC=27;
    static const int SPACEBAR=32;
    static const int SPACE=32;
    static const int ENTER=13;
    static const int TAB=9;
    static const int BACKSPACE=8;
    static const int DEL=127; //DELETE seems to be reserved

    static const int F1=128;
    static const int F2=129;
    static const int F3=130;
    static const int F4=131;
    static const int F5=132;
    static const int F6=133;
    static const int F7=134;
    static const int F8=135;
    static const int F9=136;
    static const int F10=137;
    static const int F11=138;
    static const int F12=139;

		// KEYBOARD ARROW KEYS
    static const int LEFT=227;
    static const int UP=228;
    static const int RIGHT=229;
    static const int DOWN=230;

    static const int PAGE_UP=231;
    static const int PAGE_DOWN=232;
    static const int HOME=233;
    static const int END=234;
    static const int INSERT=235;
	 
	 static const int LEFT_SHIFT = 239;
	 static const int RIGHT_SHIFT = 240;

	static const int LEFT_CTRL = 241;
	static const int RIGHT_CTRL = 242;
	 
	 
	 

    Keyboard()
    {
      lastKey=0;
      numKeysDown=0;
      keyWasPressed=false;
      keyWasUnpressed=false;
      clearAll();
    }
	void init()
  {
    clearAll();
	}
	void clearAll()
	{
    for(int index=NUMBER_OF_KEYS-1;index>=0;--index)
    { keyMap[index]=false; }
		numKeysDown=0;
		keyWasPressed=false;
		keyWasUnpressed=false;
	}
  void keyDown(const int keyVal)
  {
    if(keyMap[keyVal]==false)
    {
      numKeysDown++;
      keyMap[keyVal]=true;
      lastKey=keyVal;
    }
    keyWasPressed=true;
  }
  void keyUp(const int keyVal)
  {
    if(keyMap[keyVal]==true)
    {
      numKeysDown--;
      keyMap[keyVal]=false;
    }
  }
  void unpress (const int keyVal) { keyUp(keyVal); }
  void unPress (const int keyVal) { keyUp(keyVal); }
  
  
  void specialKeyDown(const int keyVal)
  {
      keyMap[keyVal+SPECIAL_KEY_MODIFIER]=true;
  }
  void specialKeyUp(const int keyVal)
  {
      keyMap[keyVal+SPECIAL_KEY_MODIFIER]=false;
  }
  bool isPressed(const int keyVal)
  {
      return(keyMap[keyVal]);
  }
    /*void printKey(int keyVal)
    {
		std::cout<<"KEYBOARD::printkey Not functional\n";
    }
	std::string keyName(const int keyVal)
	{
		return "";
	}*/
	
	int isNumberPressed()
	{
			if(isPressed(Keyboard::ZERO))
			{ return 0; }
			if(isPressed(Keyboard::ONE))
			{ return 1; }
			if(isPressed(Keyboard::TWO))
			{ return 2; }
			if(isPressed(Keyboard::THREE))
			{ return 3; }
			if(isPressed(Keyboard::FOUR))
			{ return 4; }
			if(isPressed(Keyboard::FIVE))
			{ return 5; }
			if(isPressed(Keyboard::SIX))
			{ return 6; }
			if(isPressed(Keyboard::SEVEN))
			{ return 7; }
			if(isPressed(Keyboard::EIGHT))
			{ return 8; }
			if(isPressed(Keyboard::NINE))
			{ return 9; }
			return -1;
	}
	
	bool isAlphaNumeric(const int key)
	{
		//std::cout<<"Key: "<<(int)key<<".\n";
		if ( (key>=65&&key<=90) || (key>=97&& key<=122) || (key>=48&& key<=57) )
		{ return true; }
		return false;
	}
	
};


