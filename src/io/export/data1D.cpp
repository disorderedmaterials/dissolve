// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "io/export/data1D.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "math/sampledData1D.h"
#include "templates/algorithms.h"

Data1DExportFileFormat::Data1DExportFileFormat(std::string_view filename, Data1DExportFormat format)
    : FileAndFormat(formats_, filename, (int)format)
{
    formats_ = EnumOptions<Data1DExportFileFormat::Data1DExportFormat>(
        "Data1DExportFileFormat", {{Data1DExportFormat::XY, "xy", "Simple XY data (x = bin centres)"}});
}

/*
 * Export Functions
 */

// Export Data1D as simple XY (or XYE) data
bool Data1DExportFileFormat::exportXY(LineParser &parser, const std::vector<double> &xAxis, const std::vector<double> &values,
                                      OptionalReferenceWrapper<const std::vector<double>> errors)
{
    if (errors)
    {
        for (auto &&[x, value, error] : zip(xAxis, values, errors->get()))
            if (!parser.writeLineF("{:16.10e}  {:16.10e}  {:16.10e}\n", x, value, error))
                return false;
    }
    else
        for (auto &&[x, value] : zip(xAxis, values))
            if (!parser.writeLineF("{:16.10e}  {:16.10e}\n", x, value))
                return false;

    return true;
}

// Export Data1D using current filename and format
bool Data1DExportFileFormat::exportData(const Data1DBase &data)
{
    // Check the format
    if (!formatIndex_)
        return Messenger::error("No format set for Data1DExportFileFormat so can't export.\n");

    // Open the file
    LineParser parser;
    if (!parser.openOutput(filename_))
    {
        parser.closeFiles();
        return false;
    }

    // Write data
    auto result = false;
    switch (formats_.enumerationByIndex(*formatIndex_))
    {
        case (Data1DExportFormat::XY):
            if (data.valuesHaveErrors())
                result = exportXY(parser, data.xAxis(), data.values(), data.errors());
            else
                result = exportXY(parser, data.xAxis(), data.values());
            break;
        default:
            throw(std::runtime_error(
                fmt::format("Data1D format '{}' export has not been implemented.\n", formats_.keywordByIndex(*formatIndex_))));
    }

    return result;
}
