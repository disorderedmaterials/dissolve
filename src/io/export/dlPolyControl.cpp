// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "io/export/dlPolyControl.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/configuration.h"

DlPolyControlExportFileFormat::DlPolyControlExportFileFormat(std::string_view filename, DlPolyControlExportFormat format)
    : FileAndFormat(formats_, filename, (int)format)
{
    formats_ = EnumOptions<DlPolyControlExportFileFormat::DlPolyControlExportFormat>(
        "ControlExportFileFormat", {{DlPolyControlExportFormat::DLPOLY, "dlpoly", "DL_POLY CONFIG File"}});
}


// Export DlPolyControl as CONTROL
bool DlPolyControlExportFileFormat::exportDLPOLY(LineParser &parser, Configuration *cfg, bool capForces, double capForcesAt, std::optional<double> cutoffDistance, std::string ensemble, double ensembleThermostatCoupling, bool timestepVariable, double fixedTimestep, std::optional<int> energyFrequency, int nSteps, std::optional<int> outputFrequency, bool randomVelocities, std::optional<int> trajectoryFrequency, std::string trajectoryKey, std::string coulMethod, double coulPrecision)
{
    // Export title
    if (!parser.writeLineF("title {} @ {}\n\n", cfg->name(), cfg->contentsVersion()))
        return false;
    if (!parser.writeLineF("io_file_config CONFIG\n"))
        return false;
    if (!parser.writeLineF("io_file_field FIELD\n"))
        return false;
    if (!parser.writeLineF("io_file_statis STATIS\n"))
        return false;
    if (!parser.writeLineF("io_file_revive REVIVE\n"))
        return false;
    if (!parser.writeLineF("io_file_revcon REVCON\n"))
        return false;
    if (!parser.writeLineF("temperature {} K\n", cfg->temperature()))
        return false;
    if (!parser.writeLineF("print_frequency {} steps\n", energyFrequency.value()))
        return false;
    if (!parser.writeLineF("stats_frequency {} steps\n", outputFrequency.value()))
        return false;
    if (!parser.writeLineF("cutoff {} ang\n", cutoffDistance.value()))
        return false;
    if (!parser.writeLineF("ensemble {}\n", ensemble))
        return false;
    if (!parser.writeLineF("ensemble_method hoover\n", cutoffDistance.value()))
        return false;
    if (!parser.writeLineF("ensemble_thermostat_coupling  {} ps\n", ensembleThermostatCoupling))
        return false;
    if (capForces && !parser.writeLineF("equilibration_force_cap {} k_b.temp/ang\n", capForcesAt))
        return false;
    if (!parser.writeLineF("time_run {} steps\n", nSteps))
        return false;
    if (timestepVariable)
    {
        if (!parser.writeLineF("timestep_variable ON\n"))
            return false;
    }
    if (!parser.writeLineF("timestep {} internal_t\n", fixedTimestep))
        return false;
    if (trajectoryFrequency.value_or(0) > 0)
    {
        if (!parser.writeLineF("traj_calculate ON\n"))
            return false;
        if (!parser.writeLineF("traj_interval {}\n", trajectoryFrequency.value()))
            return false;
        if (!parser.writeLineF("traj_key {}\n", trajectoryKey))
            return false;
    }
    if (!parser.writeLineF("coul_method {}\n", coulMethod))
        return false;
    if (!parser.writeLineF("coul_precision {}\n", coulPrecision))
        return false;
    if (!parser.writeLineF("vdw_mix_method Lorentz-Berthelot\n"))
        return false;

    return true;
}

// Export DlPolyControl using current filename and format
bool DlPolyControlExportFileFormat::exportData(Configuration *cfg, bool capForces, double capForcesAt, std::optional<double> cutoffDistance, std::string ensemble, double ensembleThermostatCoupling, bool timestepVariable, double fixedTimestep, std::optional<int> energyFrequency, int nSteps, std::optional<int> outputFrequency, bool randomVelocities, std::optional<int> trajectoryFrequency, std::string trajectoryKey, std::string coulMethod, double coulPrecision)
{
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
        case (DlPolyControlExportFormat::DLPOLY):
            result = exportDLPOLY(parser, 
                                  cfg,                                              
                                  capForces,
                                  capForcesAt,
                                  cutoffDistance,
                                  ensemble,
                                  ensembleThermostatCoupling,
                                  timestepVariable,
                                  fixedTimestep,
                                  energyFrequency,
                                  nSteps,
                                  outputFrequency,
                                  randomVelocities,
                                  trajectoryFrequency,
                                  trajectoryKey,
                                  coulMethod,
                                  coulPrecision);
            break;
        default:
            throw(std::runtime_error(fmt::format("DlPolyControl format '{}' export has not been implemented.\n",
                                                 formats_.keywordByIndex(*formatIndex_))));
    }

    return true;
}
