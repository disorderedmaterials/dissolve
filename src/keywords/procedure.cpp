// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
bool ProcedureKeyword::read(LineParser &parser, int startArg, const CoreData &coreData)
{
    if (!data_.deserialise(parser, coreData))
        return false;

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool ProcedureKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix) const
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
