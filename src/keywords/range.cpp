/*
    *** Keyword - Double Range
    *** src/keywords/range.cpp
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

#include "keywords/range.h"
#include "base/lineparser.h"

RangeKeyword::RangeKeyword(Range value, Vec3Labels::LabelType labelType) : KeywordData<Range>(KeywordBase::RangeData, value)
{
    labelType_ = labelType;
}

RangeKeyword::~RangeKeyword() {}

/*
 * Label Type
 */

// Label type to display in GUI
Vec3Labels::LabelType RangeKeyword::labelType() const { return labelType_; }

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int RangeKeyword::minArguments() const { return 2; }

// Return maximum number of arguments accepted
int RangeKeyword::maxArguments() const { return 2; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool RangeKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    if (parser.hasArg(startArg + 1))
    {
        setData(Range(parser.argd(startArg), parser.argd(startArg + 1)));

        return true;
    }

    return false;
}

// Write keyword data to specified LineParser
bool RangeKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    return parser.writeLineF("{}{}  {:12.6e}  {:12.6e}\n", prefix, keywordName, data_.minimum(), data_.maximum());
}
