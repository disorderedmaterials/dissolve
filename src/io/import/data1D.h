// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "io/import/dataBase.h"

// Forward Declarations
class Data1D;
class ProcessPool;

// Data1D Import Formats
class Data1DImportFileFormat : public DataImportFileFormat
{
    public:
    // Data1D Formats
    enum class Data1DImportFormat
    {
        XY,
        Histogram,
        GudrunMint
    };
    Data1DImportFileFormat(std::string_view filename = "", Data1DImportFormat format = Data1DImportFormat::XY, int xColumn = 1,
                           int yColumn = 2, int errorColumn = 0);
    ~Data1DImportFileFormat() override = default;

    /*
     * Keyword Options
     */
    private:
    // X value from which to calculate average Y and remove
    std::optional<double> removeAverageFromX_;
    // Minimum x value to permit when reading data
    std::optional<double> xMin_;
    // Maximum x value to permit when reading data
    std::optional<double> xMax_;
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
    void setUpKeywords() override;

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
    bool importData(Data1D &data, const ProcessPool *procPool = nullptr);
    // Import Data1D using supplied parser and current format
    bool importData(LineParser &parser, Data1D &data);
};
