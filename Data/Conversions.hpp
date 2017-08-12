#ifndef DATA_CONVERSIONS_HPP
#define DATA_CONVERSIONS_HPP

class Conversions
{
	public:
	// RyanCode ///////////////////////////////////
	static unsigned int asciiToRyanCode(unsigned int ascii)
	{
			//number
			if(ascii>=0x30 && ascii <= 0x39)
			{
				return ascii-0x30;
			}
			//capital letter
			else if(ascii>=0x41 && ascii<= 0x5A)
			{
				return ascii-0x37;
			}
			//lower case
			//else if(ascii
			//{
			//}
			else
			{
				return 0x25;
			}
	}
	/////////////////////////////////////////////
};

#endif
