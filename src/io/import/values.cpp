// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "io/import/values.h"
#include "base/lineParser.h"
#include "templates/optionalRef.h"

ValueImportFileFormat::ValueImportFileFormat(std::string_view filename, ValueImportFileFormat::ValueImportFormat format)
    : FileAndFormat(formats_, filename, (int)format)
{
    formats_ = EnumOptions<ValueImportFileFormat::ValueImportFormat>(
        "ValueImportFileFormat",
        {{ValueImportFileFormat::ValueImportFormat::Simple, "values", "Number of values, followed by one value per line"}});
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
bool ValueImportFileFormat::importData(std::vector<double> &data, LineParser &currentParser, const ProcessPool *procPool)
{
    // Check the format
    if (!formatIndex_)
        return Messenger::error("No format set for ValueImportFileFormat so can't import.\n");

    // If the filename is simply '@' then we read from the current parser - otherwise open a new file / parser
    auto readFromCurrent = filename_ == "@";
    LineParser fileParser(procPool);
    LineParser &parser = readFromCurrent ? currentParser : fileParser;

    if (!readFromCurrent && ((!parser.openInput(filename_)) || (!parser.isFileGoodForReading())))
        return Messenger::error("Couldn't open file '{}' for loading value data.\n", filename_);

    // Import the data
    auto result = false;
    switch (formats_.enumerationByIndex(*formatIndex_))
    {
        case (ValueImportFormat::Simple):
            result = importSimple(parser, data);
            break;
        default:
            throw(std::runtime_error(
                fmt::format("Value format '{}' import has not been implemented.\n", formats_.keywordByIndex(*formatIndex_))));
    }

    if (!readFromCurrent)
        fileParser.closeFiles();

    return result;
}
