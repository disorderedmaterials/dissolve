// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "io/export/forces.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/speciesatom.h"
#include "data/atomicmasses.h"

ForceExportFileFormat::ForceExportFileFormat(std::string_view filename, ForceExportFormat format)
    : FileAndFormat(formats_, filename)
{
    formats_ = EnumOptions<ForceExportFileFormat::ForceExportFormat>(
        "ForceExportFileFormat", {{ForceExportFormat::Simple, "simple", "Simple Free-Formatted Forces"}}, format);
}

/*
 * Export Functions
 */

// Export simple forces
bool ForceExportFileFormat::exportSimple(LineParser &parser, const std::vector<Vec3<double>> &f)
{
    if (!parser.writeLine("# Atom        FX            FY            FZ"))
        return false;

    if (!parser.writeLineF("{}\n", f.size()))
        return false;

    auto count = 1;
    for (auto &fxyz : f)
        if (!parser.writeLineF("  {:10d}  {:15.8e}  {:15.8e}  {:15.8e}\n", count++, fxyz.x, fxyz.y, fxyz.z))
            return false;

    return true;
}

// Export forces using current filename and format
bool ForceExportFileFormat::exportData(const std::vector<Vec3<double>> &f)
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
    if (formats_.enumeration() == ForceExportFormat::Simple)
        result = exportSimple(parser, f);
    else
    {
        Messenger::error("Unrecognised force format.\nKnown formats are:\n");
        printAvailableFormats();
    }

    return result;
}
