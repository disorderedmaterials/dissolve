// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "keywords/function1d.h"
#include "base/lineparser.h"
#include "templates/algorithms.h"

Function1DKeyword::Function1DKeyword(Functions::Function1DWrapper &data, int functionProperties)
    : KeywordBase(typeid(this)), data_(data), functionProperties_(functionProperties)
{
}

/*
 * Data
 */

// Return reference to data
Functions::Function1DWrapper &Function1DKeyword::data() { return data_; }
const Functions::Function1DWrapper &Function1DKeyword::data() const { return data_; }

// Return requested function properties
int Function1DKeyword::functionProperties() const { return functionProperties_; }

/*
 * Arguments
 */

// Return maximum number of arguments accepted
std::optional<int> Function1DKeyword::maxArguments() const { return std::nullopt; }

// Deserialise from supplied LineParser, starting at given argument offset
bool Function1DKeyword::deserialise(LineParser &parser, int startArg, const CoreData &coreData)
{
    // Check function name
    if (!Functions::function1D().isValid(parser.argsv(startArg)))
        return Functions::function1D().errorAndPrintValid(parser.argsv(startArg));
    auto func = Functions::function1D().enumeration(parser.argsv(startArg));

    // Check properties
    if (!Functions::validFunction1DProperties(func, functionProperties_))
        return Messenger::error(
            "1D function type {} can't be set in keyword '{}' as it does not have the correct properties.\n",
            Functions::function1D().keyword(func), name());

    // Get parameters if they were given
    auto params = parser.hasArg(startArg + 1) ? parser.argvd(startArg + 1) : std::vector<double>();

    return data_.setFunctionAndParameters(func, params);
}

// Serialise data to specified LineParser
bool Function1DKeyword::serialise(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
{
    return parser.writeLineF("{}{}  '{}'  {}\n", prefix, keywordName, Functions::function1D().keyword(data_.type()),
                             joinStrings(data_.parameters(), "  "));
}
