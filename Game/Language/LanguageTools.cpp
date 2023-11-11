#pragma once
#ifndef WILDCAT_LANGUAGE_LANGUAGE_TOOLS_CPP
#define WILDCAT_LANGUAGE_LANGUAGE_TOOLS_CPP

/* Wildcat: LanguageTools
#include <Game/Language/LanguageTools.cpp> */

#include "LanguageTools.hpp"

namespace LanguageTools
{
    // Initialize the static constant array
    const char VOWELS[] = {'a', 'e', 'i', 'o', 'u', 's', 'y'};
    const size_t NUM_VOWELS = sizeof(VOWELS) / sizeof(VOWELS[0]);

    // Initialize the static constant array of consonants
    const char CONSONANTS[] = {'b', 'c', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 'm', 'n', 'p', 'r', 's', 't', 'v', 'w', 'y'};
    const size_t NUM_CONSONANTS = sizeof(CONSONANTS) / sizeof(CONSONANTS[0]);

    // Initialize the static constant array of double consonants
    const std::string DOUBLE_CONSONANTS[] = {"ch", "sh", "ph", "rh", "wh", "ck", "st", "ll", "qu", "ss", "nz", "ld", "hn", "gg", "gh", "fr", "rx", "rm"};
    const size_t NUM_DOUBLE_CONSONANTS = sizeof(DOUBLE_CONSONANTS) / sizeof(DOUBLE_CONSONANTS[0]);
}

#endif // WILDCAT_LANGUAGE_LANGUAGE_TOOLS_CPP
