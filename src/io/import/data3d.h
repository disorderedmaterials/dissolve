// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/fileandformat.h"

// Forward Declarations
class Data3D;
class ProcessPool;

// Data3D Import Formats
class Data3DImportFileFormat : public FileAndFormat
{
    public:
    // Available Data3D formats
    enum Data3DImportFormat
    {
        CartesianData3D,
        nData3DImportFormats
    };
    Data3DImportFileFormat(Data3DImportFormat format = CartesianData3D);
    Data3DImportFileFormat(std::string_view filename, Data3DImportFormat format = CartesianData3D);
    ~Data3DImportFileFormat();
    Data3DImportFileFormat(const Data3DImportFileFormat &source);
    void operator=(const Data3DImportFileFormat &source);

    /*
     * Keyword Options
     */
    private:
    // Set up keywords for the format
    void setUpKeywords();

    /*
     * Format Access
     */
    private:
    // Return enum options for Data3DImportFormat
    static EnumOptions<Data3DImportFileFormat::Data3DImportFormat> data3DImportFormats();

    public:
    // Return number of available formats
    int nFormats() const;
    // Return format keyword for supplied index
    std::string formatKeyword(int id) const;
    // Return description string for supplied index
    std::string formatDescription(int id) const;
    // Return current format as Data3DImportFormat
    Data3DImportFormat data3DFormat() const;

    /*
     * Templating
     */
    private:
    // Object tag of Data3D upon which to template arrays before importing
    std::string templateSourceObjectTag_;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const { return true; }

    /*
     * Import / Write
     */
    protected:
    // Parse additional argument
    bool parseArgument(std::string_view arg);
    // Return whether this file/format has any additional arguments to write
    bool hasAdditionalArguments() const;
    // Return additional arguments as string
    std::string additionalArguments() const;

    /*
     * Data Import
     */
    private:
    // Import cartesian data from supplied parser
    bool importCartesian(LineParser &parser, Data3D &data);

    public:
    // Import Data3D using current filename and format
    bool importData(Data3D &data, ProcessPool *procPool = nullptr);
    // Import Data3D using supplied parser and current format
    bool importData(LineParser &parser, Data3D &data);
};
