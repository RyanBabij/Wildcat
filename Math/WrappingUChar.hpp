#pragma once
#ifndef WILDCAT_MATH_WRAPPING_UCHAR_HPP
#define WILDCAT_MATH_WRAPPING_UCHAR_HPP

/* Wildcat: WrappingUChar
	#include <Math/WrappingUChar.hpp>

	WrappingUChar is a utility class that represents an unsigned char value that wraps around at 0 and 255.
	It provides functionality to calculate distances considering the wrapping behavior and supports
	arithmetic and comparison operations with both WrappingUChar and unsigned char types.
*/

class WrappingUChar
{
public:
	WrappingUChar(unsigned char _value = 0);
	WrappingUChar(const WrappingUChar& _other);

	void setValue(unsigned char _newValue);
	unsigned char getValue() const;

	int distanceTo(const WrappingUChar& _other) const;
	int distanceTo(unsigned char _other) const;

	WrappingUChar operator+(const WrappingUChar& _other) const;
	WrappingUChar operator+(unsigned char _other) const;
	WrappingUChar& operator+=(const WrappingUChar& _other);
	WrappingUChar& operator+=(unsigned char _other);

	WrappingUChar& operator=(const WrappingUChar& _other);
	WrappingUChar& operator=(unsigned char _other);

	bool operator==(const WrappingUChar& _other) const;
	bool operator==(unsigned char _other) const;
	
	friend std::ostream& operator<<(std::ostream& os, const WrappingUChar& c);
	
	bool isCloser(const WrappingUChar& wrap1, const WrappingUChar& wrap2) const;

private:
	unsigned char value;

	int calculateDistance(unsigned char _other) const;
};

#endif // WILDCAT_MATH_WRAPPING_UCHAR_HPP
