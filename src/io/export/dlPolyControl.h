// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "io/fileAndFormat.h"

// Forward Declarations
class Configuration;

// Coordinate Export Formats
class DlPolyControlExportFileFormat : public FileAndFormat
{
    public:
    // Available DlPolyControl formats
    enum class DlPolyControlExportFormat
    {
        DLPOLY
    };
    DlPolyControlExportFileFormat(std::string_view filename = "", DlPolyControlExportFormat format = DlPolyControlExportFormat::DLPOLY);
    ~DlPolyControlExportFileFormat() override = default;

    /*
     * Formats
     */
    private:
    // Format enum options
    EnumOptions<DlPolyControlExportFileFormat::DlPolyControlExportFormat> formats_;

    /*
     * Filename / Basename
     */
    public:
    // Return whether the file must exist
    bool fileMustExist() const override { return false; }

    /*
     * Export Functions
     */
    private:
    // Export Configuration as DL_POLY CONTROL
    bool exportDLPOLY(LineParser &parser, Configuration *cfg, bool capForces, double capForcesAt, std::optional<double> cutoffDistance, double padding, std::string ensemble, std::string ensembleMethod, double ensembleThermostatCoupling, bool timestepVariable, double fixedTimestep, std::optional<int> energyFrequency, int nSteps, std::optional<int> outputFrequency, bool randomVelocities, std::optional<int> trajectoryFrequency, std::string trajectoryKey, std::string coulMethod, double coulPrecision);

    public:
    // Export Configuration using current filename and format
    bool exportData(Configuration *cfg, bool capForces, double capForcesAt, std::optional<double> cutoffDistance, double padding, std::string ensemble, std::string ensembleMethod, double ensembleThermostatCoupling, bool timestepVariable, double fixedTimestep, std::optional<int> energyFrequency, int nSteps, std::optional<int> outputFrequency, bool randomVelocities, std::optional<int> trajectoryFrequency, std::string trajectoryKey, std::string coulMethod, double coulPrecision);

};


