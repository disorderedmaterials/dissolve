/*
    *** Keyword - Vector of Integer/Double Tuples
    *** src/keywords/vector_intdouble.h
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

#pragma once

#include "keywords/data.h"
#include <optional>
#include <tuple>
#include <vector>

typedef std::vector<std::tuple<std::vector<int>, std::vector<double>>> IntegerDoubleVectorKeywordData;

// Keyword with list of Tuples of Vectors
class IntegerDoubleVectorKeyword : public KeywordData<IntegerDoubleVectorKeywordData &>
{
    public:
    IntegerDoubleVectorKeyword(IntegerDoubleVectorKeywordData &data, int nRequiredIntegers,
                               std::optional<int> nRequiredValues = std::nullopt);
    ~IntegerDoubleVectorKeyword();

    /*
     * Data
     */
    private:
    // Number of required integers
    int nRequiredIntegers_;
    // Number of required values (or, if not specified, one or more)
    std::optional<int> nRequiredValues_;

    /*
     * Arguments
     */
    public:
    // Return minimum number of arguments accepted
    int minArguments() const;
    // Return maximum number of arguments accepted
    int maxArguments() const;
    // Parse arguments from supplied LineParser, starting at given argument offset
    bool read(LineParser &parser, int startArg, CoreData &coreData);
    // Write keyword data to specified LineParser
    bool write(LineParser &parser, std::string_view keywordName, std::string_view prefix);
};
