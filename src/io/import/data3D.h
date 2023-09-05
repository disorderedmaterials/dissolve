// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "io/import/dataBase.h"

// Forward Declarations
class Data3D;
class ProcessPool;

// Data3D Import Formats
class Data3DImportFileFormat : public DataImportFileFormat
{
    public:
    // Available Data3D formats
    enum class Data3DImportFormat
    {
        PDens
    };
    Data3DImportFileFormat(std::string_view filename = "", Data3DImportFormat format = Data3DImportFormat::PDens);
    ~Data3DImportFileFormat() override = default;

    /*
     * Keyword Options
     */
    private:
    // Set up keywords for the format
    void setUpKeywords() override;

    /*
     * Formats
     */
    private:
    // Format enum options
    EnumOptions<Data3DImportFileFormat::Data3DImportFormat> formats_;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const override { return true; }

    /*
     * Data Import
     */
    private:
    // Import pdens data from supplied parser
    bool importPDens(LineParser &parser, Data3D &data);

    public:
    // Import Data3D using current filename and format
    bool importData(Data3D &data, const ProcessPool *procPool = nullptr);
    // Import Data3D using supplied parser and current format
    bool importData(LineParser &parser, Data3D &data);
};
