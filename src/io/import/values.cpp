// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "io/import/values.h"
#include "base/lineparser.h"
#include "templates/optionalref.h"

ValueImportFileFormat::ValueImportFileFormat(std::string_view filename, ValueImportFileFormat::ValueImportFormat format)
    : FileAndFormat(formats_, filename)
{
    formats_ = EnumOptions<ValueImportFileFormat::ValueImportFormat>(
        "ValueImportFileFormat",
        {{ValueImportFileFormat::ValueImportFormat::Simple, "values", "Number of values, followed by one value per line"}},
        format);
}

/*
 * Import Functions
 */

// Import simple value data using specified parser
bool ValueImportFileFormat::importSimple(LineParser &parser, std::vector<double> &data)
{
    data.clear();

    // Read in number of values to expect
    if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
        return false;
    auto nValues = parser.argi(0);
    data.resize(nValues);

    // Read in values
    for (auto &v : data)
    {
        if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success)
            return false;
        v = parser.argd(0);
    }

    return true;
}

// Import values using current filename and format
bool ValueImportFileFormat::importData(std::vector<double> &data, LineParser &currentParser, ProcessPool *procPool)
{
    // If the filename is simply '@' then we read from the current parser - otherwise open a new file / parser
    auto readFromCurrent = filename_ == "@";
    LineParser fileParser(procPool);
    LineParser &parser = readFromCurrent ? currentParser : fileParser;

    if (!readFromCurrent && ((!parser.openInput(filename_)) || (!parser.isFileGoodForReading())))
        return Messenger::error("Couldn't open file '{}' for loading Data1D data.\n", filename_);

    // Import the data
    auto result = false;
    switch (formats_.enumeration())
    {
        case (ValueImportFormat::Simple):
            result = importSimple(parser, data);
            break;
        default:
            throw(std::runtime_error(fmt::format("Value format '{}' import has not been implemented.\n", formats_.keyword())));
    }

    if (!readFromCurrent)
        fileParser.closeFiles();

    return result;
}
