// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "io/fileandformat.h"
#include "templates/array.h"

// Forward Declarations
class ProcessPool;

// Forces Import Formats
class ForceImportFileFormat : public FileAndFormat
{
    public:
    // Forces Formats
    enum ForceImportFormat
    {
        DLPOLYForces,
        MoscitoForces,
        SimpleForces,
        nForceImportFormats
    };
    ForceImportFileFormat(ForceImportFormat format = SimpleForces);
    ForceImportFileFormat(std::string_view filename, ForceImportFormat format = SimpleForces);
    ~ForceImportFileFormat();

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
    // Return enum options for ForceImportFormat
    static EnumOptions<ForceImportFileFormat::ForceImportFormat> forceImportFormats();

    public:
    // Return number of available formats
    int nFormats() const;
    // Return format keyword for supplied index
    std::string formatKeyword(int id) const;
    // Return description string for supplied index
    std::string formatDescription(int id) const;
    // Return current format as ForceImportFormat
    ForceImportFormat forceFormat() const;

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
    // Import DL_POLY forces through specified parser
    bool importDLPOLY(LineParser &parser, std::vector<Vec3<double>> &f);
    // Import Moscito forces through specified parser
    bool importMoscito(LineParser &parser, std::vector<Vec3<double>> &f);
    // Import simple formatted forces through specified parser
    bool importSimple(LineParser &parser, std::vector<Vec3<double>> &f);

    public:
    // Import forces using current filename and format
    bool importData(std::vector<Vec3<double>> &f, ProcessPool *procPool = nullptr);
    // Import forces using supplied parser and current format
    bool importData(LineParser &parser, std::vector<Vec3<double>> &f);
};
