#pragma once
#ifndef WILDCAT_DATA_DEBUG_VERBOSE_CPP
#define WILDCAT_DATA_DEBUG_VERBOSE_CPP
// Path: #include <Debug/Verbose/Verbose.hpp>
// Has documentation

#include <string>

int VerboseDepth;

#ifdef VERBOSE
#include <iostream>

inline void VerbosePrintDepth()
{
	for(int i=0;i<VerboseDepth;++i)
	{ std::cout<<" "; }
}
inline void Verbose(const std::string output, const int level=1)
{
	VerbosePrintDepth();
	if(VERBOSE>=level)
	{ std::cout<<"VERBOSE:"<<level<<":"<<output; }
}
inline void VerboseSimple(const std::string output, const int level=1)
{
	VerbosePrintDepth();
	if(VERBOSE>=level)
	{ std::cout<<output; }
}
inline void VerboseSimple2(const std::string output, const int level=1)
{
	if(VERBOSE>=level)
	{ std::cout<<output; }
}

inline void VerboseInc(const std::string output, const int level=1)
{
	VerbosePrintDepth();
	if(VERBOSE>=level)
	{ std::cout<<"VERBOSE:"<<level<<":"<<output; }
	++VerboseDepth;
}
inline void VerboseDec(const std::string output, const int level=1)
{
	--VerboseDepth;
	VerbosePrintDepth();
	if(VERBOSE>=level)
	{ std::cout<<"VERBOSE:"<<level<<":"<<output; }
}

#else
inline void Verbose(const std::string, const int)
{}
inline void VerboseInc(const std::string, const int)
{}
inline void VerboseDec(const std::string, const int)
{}
inline void VerboseSimple(const std::string, const int)
{}
inline void VerboseSimple2(const std::string, const int)
{}
inline void VerbosePrintDepth()
{}
#endif

#endif