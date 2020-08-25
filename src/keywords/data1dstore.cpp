/*
    *** Keyword - Data1D Store
    *** src/keywords/data1dstore.cpp
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

#include "keywords/data1dstore.h"
#include "base/lineparser.h"
#include "classes/data1dstore.h"
#include "io/import/data1d.h"

Data1DStoreKeyword::Data1DStoreKeyword(Data1DStore &data1DStore)
    : KeywordData<Data1DStore &>(KeywordBase::Data1DStoreData, data1DStore)
{
}

Data1DStoreKeyword::~Data1DStoreKeyword() {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int Data1DStoreKeyword::minArguments() const
{
    // Must have reference data name and format as a minimum
    return 2;
}

// Return maximum number of arguments accepted
int Data1DStoreKeyword::maxArguments() const
{
    // Filename, name of data, and other args
    return 99;
}

// Parse arguments from supplied LineParser, starting at given argument offset
bool Data1DStoreKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    Messenger::print("Reading test data '{}' from file '{}' (format={})...\n", parser.argsv(startArg),
                     parser.argsv(startArg + 2), parser.argsv(startArg + 1));

    if (!data_.addData(parser.argsv(startArg), parser, startArg + 1, fmt::format("End{}", name()), coreData))
        return Messenger::error("Failed to add data.\n");

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool Data1DStoreKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    // Loop over list of one-dimensional data
    RefDataListIterator<Data1D, Data1DImportFileFormat> dataIterator(data_.dataReferences());
    while (Data1D *data = dataIterator.iterate())
    {
        Data1DImportFileFormat &ff = dataIterator.currentData();
        if (!ff.writeFilenameAndFormat(parser, fmt::format("{}{}  '{}'  ", prefix, keywordName, data->name())))
            return false;
        if (!ff.writeBlock(parser, fmt::format("{}  ", prefix)))
            return false;
        if (!parser.writeLineF("{}End{}\n", prefix, name()))
            return false;
    }

    return true;
}
