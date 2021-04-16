// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/fileandformat.h"
#include "keywords/types.h"

// Forward Declarations
class Data1D;
class ProcessPool;

// Data1D Import Formats
class Data1DImportFileFormat : public FileAndFormat
{
    public:
    // Data1D Formats
    enum Data1DImportFormat
    {
        XYData1D,
        HistogramData1D,
        GudrunMintData1D,
        nData1DImportFormats
    };
    Data1DImportFileFormat(Data1DImportFormat format = XYData1D);
    Data1DImportFileFormat(std::string_view filename, Data1DImportFormat format = XYData1D);
    ~Data1DImportFileFormat();

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
    // Return enum options for Data1DImportFormat
    static EnumOptions<Data1DImportFileFormat::Data1DImportFormat> data1DImportFormats();

    public:
    // Return number of available formats
    int nFormats() const;
    // Return format keyword for supplied index
    std::string formatKeyword(int id) const;
    // Return description string for supplied index
    std::string formatDescription(int id) const;
    // Return current format as Data1DImportFormat
    Data1DImportFormat data1DFormat() const;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const { return true; }

    /*
     * Import Functions
     */
    private:
    // Import simple XY data using specified parser
    bool importXY(LineParser &parser, Data1D &data);
    // Import simple histogram data using specified parser
    bool importHistogram(LineParser &parser, Data1D &data);
    // Import Gudrun merged interference cross-section (mint) data using specified parser
    bool importGudrunMint(LineParser &parser, Data1D &data);

    public:
    // Import Data1D using current filename and format
    bool importData(Data1D &data, ProcessPool *procPool = nullptr);
    // Import Data1D using supplied parser and current format
    bool importData(LineParser &parser, Data1D &data);
};
