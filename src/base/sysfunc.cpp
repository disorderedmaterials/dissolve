/*
    *** System Functions
    *** src/base/sysfunc.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "base/sysfunc.h"
#include "base/messenger.h"
#include <ctime>
#include <ctype.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

using namespace std;

/*
 * Conversion Functions
 */

// Convert string to boolean
bool DissolveSys::stob(std::string_view s)
{
    if (sameString(s, "true") || sameString(s, "on") || sameString(s, "yes") || sameString(s, "1"))
        return true;
    return false;
}

// Convert boolean to string representation ("True" or "False")
std::string_view DissolveSys::btoa(bool b) { return (b ? "True" : "False"); }

// Convert boolean to string representation ("On" or "Off")
std::string_view DissolveSys::onOff(bool b) { return (b ? "On" : "Off"); }

/*
 * String Functions
 */

// Wildcard match the supplied strings
bool DissolveSys::wildCardMatch(std::string_view::const_iterator wild, std::string_view::const_iterator wildEnd,
                                std::string_view::const_iterator s2, std::string_view::const_iterator s2End, bool caseSensitive)
{
    // If we reach at the end of both strings, the match was a success
    if ((wild == wildEnd) && (s2 == s2End))
        return true;

    // Handle wildcards '*' and '?'
    if (*wild == '*')
    {
        /*
         * If this is not the last character in the string and there are no more characters in s2, return false.
         * Otherwise, we try to match the next character in 'wild' from the current character in s2, then the current
         * character in wild with the next character in s2. This allows the '*' to match as many characters as it can,
         * but always checks for absolute character matching between the current characters.
         */
        if (((wild + 1) != wildEnd) && (s2 == s2End))
            return false;
        else
            return wildCardMatch(wild + 1, wildEnd, s2, s2End, caseSensitive) ||
                   wildCardMatch(wild, wildEnd, s2 + 1, s2End, caseSensitive);
    }
    else
    {
        // Wildcard '?' matches any single character, so increase both character positions and continue
        if (*wild == '?')
            return wildCardMatch(wild + 1, wildEnd, s2 + 1, s2End, caseSensitive);
    }

    // Not a wildcard, so do absolute character matching
    if (caseSensitive)
    {
        if (std::tolower(*wild) == std::tolower(*s2))
            return wildCardMatch(wild + 1, wildEnd, s2 + 1, s2End, caseSensitive);
    }
    else if (*wild == *s2)
        return wildCardMatch(wild + 1, wildEnd, s2 + 1, s2End, caseSensitive);

    // Characters do not match, so return false
    return false;
}

// Perform case-(in)sensitive string comparison
bool DissolveSys::sameString(const std::string_view s1, const std::string_view s2, bool caseSensitive)
{
    // Check string lengths
    if (s1.length() != s2.length())
        return false;

    if (caseSensitive)
        return s1 == s2;

    return std::equal(s1.cbegin(), s1.cend(), s2.cbegin(), [](auto &l, auto &r) { return std::toupper(l) == std::toupper(r); });
}

// Perform case-(in)sensitive, wildcard-enabled string comparison
bool DissolveSys::sameWildString(const std::string_view wild, const std::string_view s2, bool caseSensitive)
{
    return wildCardMatch(wild.cbegin(), wild.cend(), s2.cbegin(), s2.cend(), caseSensitive);
}

// Get characters before first occurrence of designated character, or an empty string if the character does not exist
std::string_view DissolveSys::beforeChar(const std::string_view s, char c)
{
    std::size_t pos = s.find(c);
    if (pos == std::string::npos)
        return "";

    return s.substr(0, pos);
}

// Get characters before first occurrence of any of the supplied characters, or an empty string if none are present
std::string_view DissolveSys::beforeChar(const std::string_view s, std::string_view chars)
{
    for (auto n = 0; n < s.length(); ++n)
        if (chars.find(s[n]) != std::string::npos)
            return s.substr(0, n);

    return "";
}

// Get characters after first occurrence of designated character, or an empty string if the character does not exist
std::string_view DissolveSys::afterChar(const std::string_view s, char c)
{
    std::size_t pos = s.find(c);
    if ((pos == std::string::npos) || (pos == s.length() - 1))
        return "";

    return s.substr(pos + 1);
}

// Get characters after first occurrence of any of the supplied characters, or an empty string if none are present
std::string_view DissolveSys::afterChar(const std::string_view s, std::string_view chars)
{
    for (auto n = 0; n < s.length(); ++n)
        if (chars.find(s[n]) != std::string::npos)
            return s.substr(n + 1);

    return "";
}

// Get characters before last occurrence of designated character, or an empty string if the character does not exist
std::string_view DissolveSys::beforeLastChar(const std::string_view s, char c)
{
    std::size_t pos = s.rfind(c);
    if (pos == std::string::npos)
        return "";

    return s.substr(0, pos);
}

// Get characters after last occurrence of designated character, or an empty string if the character does not exist
std::string_view DissolveSys::afterLastChar(const std::string_view s, char c)
{
    std::size_t pos = s.rfind(c);
    if ((pos == std::string::npos) || (pos == s.length() - 1))
        return "";

    return s.substr(pos + 1);
}

// Return if the target string ends with the specified characters
bool DissolveSys::endsWith(std::string_view target, std::string_view ending)
{
    if (ending.size() > target.size())
        return false;

    return std::equal(ending.rbegin(), ending.rend(), target.rbegin());
}

// Remove comments from supplied string
void DissolveSys::removeComments(std::string &s)
{
    auto quoteChar = '\0';
    auto escaped = false;
    for (auto n = 0; n < s.length(); ++n)
    {
        const auto c = s[n];

        // Remember current quoting info...
        if (c == '"')
        {
            if (quoteChar == '\0')
                quoteChar = '"';
            else if (quoteChar == '"')
                quoteChar = '\0';
        }
        if (c == '\'')
        {
            if (quoteChar == '\0')
                quoteChar = '\'';
            else if (quoteChar == '\'')
                quoteChar = '\0';
        }
        if ((c == '#') && (!escaped) && (quoteChar == '\0'))
        {
            s = s.substr(0, n);
            return;
        }

        // Set "escaped" status
        escaped = c == '\\';
    }
}

// Return nice version of string (no spaces, slashes etc.)
std::string DissolveSys::niceName(std::string_view original)
{
    std::string s{original};

    std::replace_if(s.begin(), s.end(), [](auto &c) { return " /\\#*$"sv.find(c) != std::string::npos; }, '_');

    return s;
}

/*
 * Number Detection
 */

// Return whether the supplied string is a number
bool DissolveSys::isNumber(std::string_view text)
{
    bool isFloatingPoint;
    return isNumber(text, isFloatingPoint);
}

// Return whether the supplied string is a number, and also whether it is floating-point
bool DissolveSys::isNumber(std::string_view text, bool &isFloatingPoint)
{
    // Assume integer to start with
    isFloatingPoint = false;

    auto exponentIndex = -1;

    const auto length = text.size();
    for (int n = 0; n < length; ++n)
    {
        char c = text[n];
        switch (text[n])
        {
            // Decimal point
            case ('.'):
                isFloatingPoint = true;
                break;
            // Negate or plus
            case ('-'):
            case ('+'):
                // Only allow as first character or immediately following an exponent
                if (n != (exponentIndex + 1))
                    return false;
                break;
            // Exponentiation
            case ('e'):
            case ('E'):
                // Can't be first character
                if (n == 0)
                    return false;

                // Can't have more than one
                if (exponentIndex > 0)
                    return false;

                // Store position
                exponentIndex = n;
                break;
            default:
                // If not a digit, return false
                if (!isdigit(c))
                    return false;
                break;
        }
    }

    return true;
}

/*
 * Files
 */

// Return whether file exists
bool DissolveSys::fileExists(std::string_view filename)
{
    fstream f(std::string(filename), ios::in);
    if (f.is_open())
    {
        f.close();
        return true;
    }
    else
        return false;
}

/*
 * Time Functions
 */

// Return string of current time / date
std::string DissolveSys::currentTimeAndDate()
{
    auto currentTime = time(nullptr);
    auto *local = localtime(&currentTime);

    std::ostringstream timeStream;
    timeStream << std::put_time(local, "%H:%M:%S on %d-%m-%Y");
    return timeStream.str();
}
