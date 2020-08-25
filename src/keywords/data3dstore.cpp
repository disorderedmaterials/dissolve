/*
    *** Keyword - Data3D Store
    *** src/keywords/data3dstore.cpp
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

#include "keywords/data3dstore.h"
#include "base/lineparser.h"

Data3DStoreKeyword::Data3DStoreKeyword(Data3DStore &data3DStore)
    : KeywordData<Data3DStore &>(KeywordBase::Data3DStoreData, data3DStore)
{
}

Data3DStoreKeyword::~Data3DStoreKeyword() {}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int Data3DStoreKeyword::minArguments() const
{
    // Must have reference data name and format as a minimum
    return 2;
}

// Return maximum number of arguments accepted
int Data3DStoreKeyword::maxArguments() const
{
    // Filename, name of data, and other args
    return 99;
}

// Parse arguments from supplied LineParser, starting at given argument offset
bool Data3DStoreKeyword::read(LineParser &parser, int startArg, CoreData &coreData)
{
    Messenger::print("Reading test data '{}' from file '{}' (format={})...\n", parser.argsv(startArg),
                     parser.argsv(startArg + 2), parser.argsv(startArg + 1));

    if (!data_.addData(parser.argsv(startArg), parser, startArg + 1, fmt::format("End{}", name()), coreData))
        return Messenger::error("Failed to add data.\n");

    set_ = true;

    return true;
}

// Write keyword data to specified LineParser
bool Data3DStoreKeyword::write(LineParser &parser, std::string_view keywordName, std::string_view prefix)
{
    // Loop over list of one-dimensional data
    RefDataListIterator<Data3D, Data3DImportFileFormat> dataIterator(data_.dataReferences());
    while (Data3D *data = dataIterator.iterate())
    {
        Data3DImportFileFormat &ff = dataIterator.currentData();
        if (!ff.writeFilenameAndFormat(parser, fmt::format("{}{}  '{}'  ", prefix, keywordName, data->name())))
            return false;
        if (!ff.writeBlock(parser, fmt::format("{}  ", prefix)))
            return false;
        if (!parser.writeLineF("{}End{}\n", prefix, name()))
            return false;
    }

    return true;
}
