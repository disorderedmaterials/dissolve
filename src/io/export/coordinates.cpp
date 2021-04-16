// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "io/export/coordinates.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/speciesatom.h"
#include "data/atomicmasses.h"

CoordinateExportFileFormat::CoordinateExportFileFormat(std::string_view filename, CoordinateExportFormat format)
    : FileAndFormat(filename, format)
{
}

/*
 * Format Access
 */

// Return enum options for CoordinateExportFormat
EnumOptions<CoordinateExportFileFormat::CoordinateExportFormat> CoordinateExportFileFormat::coordinateExportFormats()
{
    return EnumOptions<CoordinateExportFileFormat::CoordinateExportFormat>(
        "CoordinateExportFileFormat", {{CoordinateExportFileFormat::XYZCoordinates, "xyz", "Simple XYZ Coordinates"},
                                       {CoordinateExportFileFormat::DLPOLYCoordinates, "dlpoly", "DL_POLY CONFIG File"}});
}

// Return number of available formats
int CoordinateExportFileFormat::nFormats() const { return CoordinateExportFileFormat::nCoordinateExportFormats; }

// Return format keyword for supplied index
std::string CoordinateExportFileFormat::formatKeyword(int id) const { return coordinateExportFormats().keywordByIndex(id); }

// Return description string for supplied index
std::string CoordinateExportFileFormat::formatDescription(int id) const
{
    return coordinateExportFormats().descriptionByIndex(id);
}

// Return current format as CoordinateExportFormat
CoordinateExportFileFormat::CoordinateExportFormat CoordinateExportFileFormat::coordinateFormat() const
{
    return (CoordinateExportFileFormat::CoordinateExportFormat)format_;
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
    for (auto i : cfg->atoms())
        if (!parser.writeLineF("{:<3}   {:15.9f}  {:15.9f}  {:15.9f}\n", Elements::symbol(i->speciesAtom()->Z()), i->r().x,
                               i->r().y, i->r().z))
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
    if (cfg->box()->type() == Box::NonPeriodicBoxType)
    {
        if (!parser.writeLineF("{:10d}{:10d}\n", 0, 0))
            return false;
    }
    else if (cfg->box()->type() == Box::CubicBoxType)
    {
        if (!parser.writeLineF("{:10d}{:10d}\n", 0, 1))
            return false;
    }
    else if (cfg->box()->type() == Box::OrthorhombicBoxType)
    {
        if (!parser.writeLineF("{:10d}{:10d}\n", 0, 2))
            return false;
    }
    else
        parser.writeLineF("{:10d}{:10d}\n", 0, 3);

    // Export Cell
    if (cfg->box()->type() != Box::NonPeriodicBoxType)
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
    auto n = 0;
    for (auto i : cfg->atoms())
        if (!parser.writeLineF("{:<6}{:10d}{:20.10f}\n{:20.12f}{:20.12f}{:20.12f}\n",
                               cfg->usedAtomType(i->localTypeIndex())->name(), n++ + 1, AtomicMass::mass(i->speciesAtom()->Z()),
                               i->r().x, i->r().y, i->r().z))
            return false;

    return true;
}

// Export coordinates using current filename and format
bool CoordinateExportFileFormat::exportData(Configuration *cfg)
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
    if (coordinateFormat() == CoordinateExportFileFormat::XYZCoordinates)
        result = exportXYZ(parser, cfg);
    else if (coordinateFormat() == CoordinateExportFileFormat::DLPOLYCoordinates)
        result = exportDLPOLY(parser, cfg);
    else
    {
        Messenger::error("Unrecognised coordinate format.\nKnown formats are:\n");
        printAvailableFormats();
    }

    return result;
}
