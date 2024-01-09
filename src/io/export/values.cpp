// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "io/export/values.h"
#include "base/lineParser.h"
#include "templates/optionalRef.h"

ValueExportFileFormat::ValueExportFileFormat(std::string_view filename, ValueExportFileFormat::ValueExportFormat format)
    : FileAndFormat(formats_, filename, (int)format)
{
    formats_ = EnumOptions<ValueExportFileFormat::ValueExportFormat>(
        "ValueExportFileFormat",
        {{ValueExportFileFormat::ValueExportFormat::Simple, "values", "Number of values, followed by one value per line"}});
}

/*
 * Export Functions
 */

// Export simple value data using specified parser
bool ValueExportFileFormat::exportSimple(LineParser &parser, const std::vector<double> &data)
{
    // Write number of values
    if (!parser.writeLineF("{}\n", data.size()))
        return false;

    // Write values
    for (auto &v : data)
        if (!parser.writeLineF("{}\n", v))
            return false;

    return true;
}

// Export values using current filename and format
bool ValueExportFileFormat::exportData(const std::vector<double> &data, LineParser &currentParser, ProcessPool *procPool)
{
    // Check the format
    if (!formatIndex_)
        return Messenger::error("No format set for ValueExportFileFormat so can't export.\n");

    // If the filename is simply '@' then we write to the current parser - otherwise open a new file / parser
    auto writeToCurrent = filename_ == "@";
    LineParser fileParser(procPool);
    LineParser &parser = writeToCurrent ? currentParser : fileParser;

    if (!writeToCurrent && ((!parser.openOutput(filename_)) || (!parser.isFileGoodForWriting())))
        return Messenger::error("Couldn't open file '{}' for writing value data.\n", filename_);

    // Export the data
    auto result = false;
    switch (formats_.enumerationByIndex(*formatIndex_))
    {
        case (ValueExportFormat::Simple):
            result = exportSimple(parser, data);
            break;
        default:
            throw(std::runtime_error(
                fmt::format("Value format '{}' export has not been implemented.\n", formats_.keywordByIndex(*formatIndex_))));
    }

    return result;
}
