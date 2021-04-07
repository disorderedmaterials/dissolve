// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <string_view>

// System Functions
class DissolveSys
{
    /*
     * Conversion Functions
     */
    public:
    // Convert string to boolean
    static bool stob(std::string_view s);
    // Convert boolean to string representation ("True" or "False")
    static std::string_view btoa(bool b);
    // Convert boolean to string representation ("On" or "Off")
    static std::string_view onOff(bool b);

    /*
     * String Functions
     */
    private:
    // Wildcard match the supplied strings
    static bool wildCardMatch(std::string_view::const_iterator wild, std::string_view::const_iterator wildEnd,
                              std::string_view::const_iterator s2, std::string_view::const_iterator s2End, bool caseSensitive);

    public:
    // Perform case-(in)sensitive string comparison
    static bool sameString(const std::string_view s1, const std::string_view s2, bool caseSensitive = false);
    // Perform case-(in)sensitive, wildcard-enabled string comparison
    static bool sameWildString(const std::string_view wild, const std::string_view s2, bool caseSensitive = false);
    // Get characters before first occurrence of designated character, or an empty string if the character does not exist
    static std::string_view beforeChar(const std::string_view s, char c);
    // Get characters before first occurrence of any of the supplied characters, or an empty string if none are present
    static std::string_view beforeChar(const std::string_view s, std::string_view chars);
    // Get characters after first occurrence of designated character, or an empty string if the character does not exist
    static std::string_view afterChar(const std::string_view s, char c);
    // Get characters after first occurrence of any of the supplied characters, or an empty string if none are present
    static std::string_view afterChar(const std::string_view s, std::string_view chars);
    // Get characters before last occurrence of designated character, or an empty string if the character does not exist
    static std::string_view beforeLastChar(const std::string_view s, char c);
    // Get characters after last occurrence of designated character, or an empty string if the character does not exist
    static std::string_view afterLastChar(const std::string_view s, char c);
    // Return if the target string starts with the specified characters
    static bool startsWith(std::string_view target, std::string_view beginning);
    // Return if the target string ends with the specified characters
    static bool endsWith(std::string_view target, std::string_view ending);
    // Remove comments from supplied string
    static void removeComments(std::string &s);
    // Return nice version of string (no spaces, slashes etc.)
    static std::string niceName(std::string_view original);
    // Return whether the supplied string is a number
    static bool isNumber(std::string_view text);
    // Return whether the supplied string is a number, and also whether it is floating-point
    static bool isNumber(std::string_view text, bool &isFloatingPoint);
    // Replace all occurrences of search string with replace string
    static std::string replace(const std::string_view source, const std::string_view search, const std::string_view replace);

    /*
     * Files
     */
    public:
    // Return whether specified file exists
    static bool fileExists(std::string_view filename);

    /*
     * Time Functions
     */
    public:
    // Return string of current time / date
    static std::string currentTimeAndDate();
};
