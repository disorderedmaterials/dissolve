// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "io/import/data3d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

Data3DImportFileFormat::Data3DImportFileFormat(Data3DImportFileFormat::Data3DImportFormat format) : FileAndFormat(format)
{
    setUpKeywords();
}
Data3DImportFileFormat::Data3DImportFileFormat(std::string_view filename, Data3DImportFileFormat::Data3DImportFormat format)
    : FileAndFormat(filename, format)
{
    setUpKeywords();
}

Data3DImportFileFormat::~Data3DImportFileFormat() {}

/*
 * Keyword Options
 */

// Set up keywords for the format
void Data3DImportFileFormat::setUpKeywords() {}

/*
 * Format Access
 */

// Return enum options for Data3DImportFormat
EnumOptions<Data3DImportFileFormat::Data3DImportFormat> Data3DImportFileFormat::data3DImportFormats()
{
    return EnumOptions<Data3DImportFileFormat::Data3DImportFormat>(
        "Data3DImportFileFormat", {{Data3DImportFileFormat::CartesianData3D, "cartesian", "Cartesian X,Y,Z,f(x,y,z) data"}});
}

// Return number of available formats
int Data3DImportFileFormat::nFormats() const { return Data3DImportFileFormat::nData3DImportFormats; }

// Return format keyword for supplied index
std::string Data3DImportFileFormat::formatKeyword(int id) const { return data3DImportFormats().keywordByIndex(id); }

// Return description string for supplied index
std::string Data3DImportFileFormat::formatDescription(int id) const { return data3DImportFormats().descriptionByIndex(id); }

// Return current format as Data3DImportFormat
Data3DImportFileFormat::Data3DImportFormat Data3DImportFileFormat::data3DFormat() const
{
    return (Data3DImportFileFormat::Data3DImportFormat)format_;
}

/*
 * Import / Write
 */

// Parse additional argument
bool Data3DImportFileFormat::parseArgument(std::string_view arg)
{
    // Split arg into parts before and after the '='
    std::string_view key = DissolveSys::beforeChar(arg, '=');
    std::string_view value = DissolveSys::afterChar(arg, '=');
    if (key == "template")
        templateSourceObjectTag_ = value;
    else
        return false;

    return true;
}

// Return whether this file/format has any additional arguments to write
bool Data3DImportFileFormat::hasAdditionalArguments() const { return true; }

// Return additional arguments as string
std::string Data3DImportFileFormat::additionalArguments() const
{
    std::string args;

    args.clear();
    if (!templateSourceObjectTag_.empty())
        args += fmt::format("template='{}'", templateSourceObjectTag_);

    return args;
}

/*
 * Import Functions
 */

// Import Data3D using current filename and format
bool Data3DImportFileFormat::importData(Data3D &data, ProcessPool *procPool)
{
    // Open file and check that we're OK to proceed importing from it
    LineParser parser(procPool);
    if ((!parser.openInput(filename_)) || (!parser.isFileGoodForReading()))
        return Messenger::error("Couldn't open file '{}' for loading Data3D data.\n", filename_);

    // Import the data
    auto result = importData(parser, data);

    parser.closeFiles();

    return result;
}

// Import Data3D using supplied parser and current format
bool Data3DImportFileFormat::importData(LineParser &parser, Data3D &data)
{
    // Import the data
    auto result = false;
    switch (data3DFormat())
    {
        default:
            Messenger::error("Don't know how to load Data3D in format '{}'.\n", formatKeyword(data3DFormat()));
    }

    return result;
}
