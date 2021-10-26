// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/fileandformat.h"

// Forward Declarations
class Data1D;
class ProcessPool;

// Data1D Import Formats
class Data1DImportFileFormat : public FileAndFormat
{
    public:
    // Data1D Formats
    enum class Data1DImportFormat
    {
        XY,
        Histogram,
        GudrunMint
    };
    explicit Data1DImportFileFormat(std::string_view filename = "", Data1DImportFormat format = Data1DImportFormat::XY);
    ~Data1DImportFileFormat() override = default;

    /*
     * Keyword Options
     */
    private:
    // X value from which to calculate average Y and remove
    double removeAverageFromX_{-1.0};
    // Minimum x value to permit when reading data
    double xMin_{0.0};
    // Maximum x value to permit when reading data
    double xMax_{0.0};
    // Column index to use for x values
    int xColumn_{1};
    // Column index to use for y values
    int yColumn_{2};
    // Column index to use for error values
    int errorColumn_{0};
    // Remove a number of points from the start of the data
    int nPointsToRemove_{0};

    private:
    // Set up keywords for the format
    void setUpKeywords();

    /*
     * Formats
     */
    private:
    // Format enum options
    EnumOptions<Data1DImportFileFormat::Data1DImportFormat> formats_;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const override { return true; }

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
