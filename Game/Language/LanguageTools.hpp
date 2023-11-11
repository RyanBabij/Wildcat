#pragma once
#ifndef WILDCAT_LANGUAGE_LANGUAGE_TOOLS_HPP
#define WILDCAT_LANGUAGE_LANGUAGE_TOOLS_HPP

/* Wildcat: LanguageTools
#include <Game/Language/LanguageTools.hpp> */

/**
	Library for miscellaneous common functions involving languages.
*/

#include <string>
#include <cstddef> // For size_t

namespace LanguageTools {
    extern const char VOWELS[];
    extern const size_t NUM_VOWELS;

    extern const char CONSONANTS[];
    extern const size_t NUM_CONSONANTS;

    extern const std::string DOUBLE_CONSONANTS[];
    extern const size_t NUM_DOUBLE_CONSONANTS;
}

#endif // WILDCAT_LANGUAGE_LANGUAGE_TOOLS_HPP
