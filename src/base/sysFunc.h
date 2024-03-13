// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <algorithm>
#include <fmt/core.h>
#include <optional>
#include <string_view>
#include <vector>

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
    // Get characters after first occurrence of the supplied string, or an empty string if it is not present
    static std::string_view afterString(const std::string_view s, std::string_view searchString);
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
    // Returns 's' if value is plural
    static std::string plural(int value);
    // Return whether the supplied string is a number
    static bool isNumber(std::string_view text);
    // Return whether the supplied string is a number, and also whether it is floating-point
    static bool isNumber(std::string_view text, bool &isFloatingPoint);
    // Replace all occurrences of search string with replace string
    static std::string replace(const std::string_view source, const std::string_view search, const std::string_view replace);
    // Split a string over a delimiter, returning a vector of elements
    static std::vector<std::string> splitString(std::string_view str, std::string_view delim = " ");
    // Split a string over a delimiter, returning a vector of converted double values
    static std::vector<double> splitStringToDoubles(std::string_view str, std::string_view delim = ", ");
    // Double any of the supplied characters in the string
    static std::string doubleChars(const std::string_view s, const std::string_view charsToDouble);
    // Return unique name for object
    template <class Range, class NameFunction>
    static std::string uniqueName(std::string_view baseName, const Range &objects, NameFunction nameFunction)
    {
        // Ensure our base string is valid and set the starting unique name
        std::string base = baseName.empty() ? "UnnamedObject" : std::string(baseName);
        std::string uniqueName{base};

        // Iterate until we find an unused name
        auto suffix = 0;
        while (std::find_if(objects.begin(), objects.end(),
                            [nameFunction, &uniqueName](const auto &object) {
                                return !nameFunction(object).empty() &&
                                       DissolveSys::sameString(nameFunction(object), uniqueName);
                            }) != objects.end())
            uniqueName = fmt::format("{}{:02d}", base, ++suffix);

        return uniqueName;
    }
    // Return string with all occurrences of specified character stripped out
    static std::string strip(const std::string_view s, char toStrip = ' ');

    /*
     * String Vectors
     */
    public:
    // Return nth string from the given vector
    static std::string stringAt(const std::vector<std::string> &ss, int n);
    // Return index of string in the given vector
    static std::optional<int> indexOf(const std::vector<std::string> &ss, std::string_view s);

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
