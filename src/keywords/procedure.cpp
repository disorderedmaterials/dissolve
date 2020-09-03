/*
    *** Keyword - Procedure
    *** src/keywords/procedure.cpp
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

#include "keywords/procedure.h"
#include "base/lineparser.h"
#include "classes/configuration.h"
#include "classes/species.h"

ProcedureKeyword::ProcedureKeyword(Procedure &procedure) : KeywordData<Procedure &>(KeywordBase::ProcedureData, procedure) {}

ProcedureKeyword::~ProcedureKeyword() {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int ProcedureKeyword::minArguments() const { return 0; }

// Return maximum number of arguments accepted
int ProcedureKeyword::maxArguments() const { return 0; }

// Parse arguments from supplied LineParser, starting at given argument offset
bool ProcedureKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    if (!data_.read(parser, coreData))
        return false;

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool ProcedureKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    // Write the keyword name as the start of the data
    if (!parser.writeLineF("{}{}\n", prefix, name()))
        return false;

    // Increase the indent
    std::string newPrefix = fmt::format("{}  ", prefix);

    // Write the node data
    if (!data_.write(parser, newPrefix))
        return false;

    // Write the end keyword (based on our name)
    if (!parser.writeLineF("{}End{}\n", prefix, name()))
        return false;

    return true;
}
