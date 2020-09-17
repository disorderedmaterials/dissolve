/*
    *** Export - Forces
    *** src/io/export/forces.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "io/export/forces.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/speciesatom.h"
#include "data/atomicmass.h"

ForceExportFileFormat::ForceExportFileFormat(std::string_view filename, ForceExportFormat format)
    : FileAndFormat(filename, format)
{
}

/*
 * Format Access
 */

// Return enum options for ForceExportFormat
EnumOptions<ForceExportFileFormat::ForceExportFormat> ForceExportFileFormat::forceExportFormats()
{
    static EnumOptionsList ForceExportFormats =
        EnumOptionsList() << EnumOption(ForceExportFileFormat::SimpleForces, "simple", "Simple Free-Formatted Forces");

    static EnumOptions<ForceExportFileFormat::ForceExportFormat> options("ForceExportFileFormat", ForceExportFormats);

    return options;
}

// Return number of available formats
int ForceExportFileFormat::nFormats() const { return ForceExportFileFormat::nForceExportFormats; }

// Return format keyword for supplied index
std::string_view ForceExportFileFormat::formatKeyword(int id) const { return forceExportFormats().keywordByIndex(id); }

// Return description string for supplied index
std::string_view ForceExportFileFormat::formatDescription(int id) const { return forceExportFormats().descriptionByIndex(id); }

// Return current format as ForceExportFormat
ForceExportFileFormat::ForceExportFormat ForceExportFileFormat::forceFormat() const
{
    return (ForceExportFileFormat::ForceExportFormat)format_;
}

/*
 * Export Functions
 */

// Export simple forces
bool ForceExportFileFormat::exportSimple(LineParser &parser, const Array<double> &fx, const Array<double> &fy,
                                         const Array<double> &fz)
{
    if (!parser.writeLine("# Atom        FX            FY            FZ"))
        return false;

    if (!parser.writeLineF("{}\n", fx.nItems()))
        return false;

    for (auto n = 0; n < fx.nItems(); ++n)
        if (!parser.writeLineF("  {:10d}  {:15.8e}  {:15.8e}  {:15.8e}\n", n + 1, fx.constAt(n), fy.constAt(n), fz.constAt(n)))
            return false;

    return true;
}

// Export forces using current filename and format
bool ForceExportFileFormat::exportData(const Array<double> &fx, const Array<double> &fy, const Array<double> &fz)
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
    if (forceFormat() == ForceExportFileFormat::SimpleForces)
        result = exportSimple(parser, fx, fy, fz);
    else
    {
        Messenger::error("Unrecognised force format.\nKnown formats are:\n");
        printAvailableFormats();
    }

    return result;
}
