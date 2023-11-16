// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "io/export/dlPolyControl.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/speciesAtom.h"
#include "data/atomicMasses.h"

DlPolyControlExportFileFormat::DlPolyControlExportFileFormat(std::string_view filename, DlPolyControlExportFormat format)
    : FileAndFormat(formats_, filename, (int)format)
{
    formats_ = EnumOptions<DlPolyControlExportFileFormat::DlPolyControlExportFormat>(
        "CoordinateExportFileFormat", {{DlPolyControlExportFormat::DLPOLY, "dlpoly", "DL_POLY CONFIG File"}});
}


// Export DlPolyControl as CONTROL
bool DlPolyControlExportFileFormat::exportDLPOLY(LineParser &parser, Configuration *cfg, bool capForces, double capForcesAt, std::optional<double> cutoffDistance, double fixedTimestep, std::optional<int> energyFrequency, int nSteps, std::optional<int> outputFrequency, bool randomVelocities, std::optional<int> trajectoryFrequency)
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
    if (capForces && !parser.writeLineF("equilibration_force_cap {}\n", capForcesAt))
        return false;
    if (!parser.writeLineF("time_run {} steps\n", nSteps))
        return false;
    if (!parser.writeLineF("{}\n", fixedTimestep))
        return false;
    if (!parser.writeLineF("{}\n", randomVelocities))
        return false;
    if (!parser.writeLineF("{}\n", trajectoryFrequency.value()))
        return false;

    return true;
}

// Export DlPolyControl using current filename and format
bool DlPolyControlExportFileFormat::exportData(Configuration *cfg, bool capForces, double capForcesAt, std::optional<double> cutoffDistance, double fixedTimestep, std::optional<int> energyFrequency, int nSteps, std::optional<int> outputFrequency, bool randomVelocities, std::optional<int> trajectoryFrequency)
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
                                  fixedTimestep,
                                  energyFrequency,
                                  nSteps,
                                  outputFrequency,
                                  randomVelocities,
                                  trajectoryFrequency);
            break;
        default:
            throw(std::runtime_error(fmt::format("DlPolyControl format '{}' export has not been implemented.\n",
                                                 formats_.keywordByIndex(*formatIndex_))));
    }

    return true;
}
