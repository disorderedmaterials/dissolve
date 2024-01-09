// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "io/export/coordinates.h"
#include "base/lineParser.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/speciesAtom.h"
#include "data/atomicMasses.h"

CoordinateExportFileFormat::CoordinateExportFileFormat(std::string_view filename, CoordinateExportFormat format)
    : FileAndFormat(formats_, filename, (int)format)
{
    formats_ = EnumOptions<CoordinateExportFileFormat::CoordinateExportFormat>(
        "CoordinateExportFileFormat", {{CoordinateExportFormat::XYZ, "xyz", "Simple XYZ Coordinates"},
                                       {CoordinateExportFormat::DLPOLY, "dlpoly", "DL_POLY CONFIG File"}});
}

/*
 * Export Functions
 */

// Export coordinates as XYZ
bool CoordinateExportFileFormat::exportXYZ(LineParser &parser, Configuration *cfg)
{
    // Export number of atoms and title
    if (!parser.writeLineF("{}\n", cfg->nAtoms()))
        return false;
    if (!parser.writeLineF("{} @ {}\n", cfg->name(), cfg->contentsVersion()))
        return false;

    // Export Atoms
    for (const auto &i : cfg->atoms())
        if (!parser.writeLineF("{:<3}   {:15.9f}  {:15.9f}  {:15.9f}\n", Elements::symbol(i.speciesAtom()->Z()), i.r().x,
                               i.r().y, i.r().z))
            return false;

    return true;
}

// Export coordinates as CONFIG
bool CoordinateExportFileFormat::exportDLPOLY(LineParser &parser, Configuration *cfg)
{
    // Export title
    if (!parser.writeLineF("{} @ {}\n", cfg->name(), cfg->contentsVersion()))
        return false;

    // Export keytrj and imcon
    if (cfg->box()->type() == Box::BoxType::NonPeriodic)
    {
        if (!parser.writeLineF("{:10d}{:10d}\n", 0, 0))
            return false;
    }
    else if (cfg->box()->type() == Box::BoxType::Cubic)
    {
        if (!parser.writeLineF("{:10d}{:10d}\n", 0, 1))
            return false;
    }
    else if (cfg->box()->type() == Box::BoxType::Orthorhombic)
    {
        if (!parser.writeLineF("{:10d}{:10d}\n", 0, 2))
            return false;
    }
    else
        parser.writeLineF("{:10d}{:10d}\n", 0, 3);

    // Export Cell
    if (cfg->box()->type() != Box::BoxType::NonPeriodic)
    {
        Matrix3 axes = cfg->box()->axes();
        if (!parser.writeLineF("{:20.12f}{:20.12f}{:20.12f}\n", axes[0], axes[1], axes[2]))
            return false;
        if (!parser.writeLineF("{:20.12f}{:20.12f}{:20.12f}\n", axes[3], axes[4], axes[5]))
            return false;
        if (!parser.writeLineF("{:20.12f}{:20.12f}{:20.12f}\n", axes[6], axes[7], axes[8]))
            return false;
    }

    // Export Atoms
    const auto &atomTypes = cfg->atomTypes();
    auto n = 0;
    for (const auto &i : cfg->atoms())
        if (!parser.writeLineF("{:<6}{:10d}{:20.10f}\n{:20.12f}{:20.12f}{:20.12f}\n",
                               atomTypes[i.localTypeIndex()].atomTypeName(), n++ + 1, AtomicMass::mass(i.speciesAtom()->Z()),
                               i.r().x, i.r().y, i.r().z))
            return false;

    return true;
}

// Export coordinates using current filename and format
bool CoordinateExportFileFormat::exportData(Configuration *cfg)
{
    // Check the format
    if (!formatIndex_)
        return Messenger::error("No format set for CoordinateExportFileFormat so can't export.\n");

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
        case (CoordinateExportFormat::XYZ):
            result = exportXYZ(parser, cfg);
            break;
        case (CoordinateExportFormat::DLPOLY):
            result = exportDLPOLY(parser, cfg);
            break;
        default:
            throw(std::runtime_error(fmt::format("Coordinates format '{}' export has not been implemented.\n",
                                                 formats_.keywordByIndex(*formatIndex_))));
    }

    return result;
}
