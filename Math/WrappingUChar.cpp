#pragma once
#ifndef WILDCAT_MATH_WRAPPING_UCHAR_CPP
#define WILDCAT_MATH_WRAPPING_UCHAR_CPP

/* Wildcat: WrappingUChar
		#include <Math/WrappingUChar.cpp>
	Implementation of WrappingUChar.hpp
	
*/


#include "WrappingUChar.hpp"
#include <cmath>

WrappingUChar::WrappingUChar(unsigned char _value) : value(_value) {}

void WrappingUChar::setValue(unsigned char _newValue)
{
	value = _newValue;
}

unsigned char WrappingUChar::getValue() const
{
	return value;
}

int WrappingUChar::distanceTo(const WrappingUChar& _other) const
{
	return calculateDistance(_other.value);
}

int WrappingUChar::distanceTo(unsigned char _other) const
{
	return calculateDistance(_other);
}

WrappingUChar WrappingUChar::operator+(const WrappingUChar& _other) const
{
	return WrappingUChar((value + _other.value) % 256);
}

WrappingUChar WrappingUChar::operator+(unsigned char _other) const
{
	return WrappingUChar((value + _other) % 256);
}

WrappingUChar& WrappingUChar::operator+=(const WrappingUChar& _other)
{
	value = (value + _other.value) % 256;
	return *this;
}

WrappingUChar& WrappingUChar::operator+=(unsigned char _other)
{
	value = (value + _other) % 256;
	return *this;
}

WrappingUChar& WrappingUChar::operator=(const WrappingUChar& _other)
{
	value = _other.value;
	return *this;
}

WrappingUChar& WrappingUChar::operator=(unsigned char _other)
{
	value = _other;
	return *this;
}

bool WrappingUChar::operator==(const WrappingUChar& _other) const
{
	return value == _other.value;
}

bool WrappingUChar::operator==(unsigned char _other) const
{
	return value == _other;
}

// Helper function for calculating distance
int WrappingUChar::calculateDistance(unsigned char _other) const
{
	const unsigned char directDistance = _other - value;
	const unsigned char reverseDistance = value - _other;

	// Return the smaller of the two distances
	return (directDistance <= reverseDistance) ? directDistance : reverseDistance;
}

#endif // WILDCAT_MATH_WRAPPING_UCHAR_CPP
