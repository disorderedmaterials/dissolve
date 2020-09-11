/*
    *** Import - Forces
    *** src/io/import/forces.cpp
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

#include "io/import/forces.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "keywords/double.h"

ForceImportFileFormat::ForceImportFileFormat(ForceImportFileFormat::ForceImportFormat format) : FileAndFormat(format)
{
    setUpKeywords();
}
ForceImportFileFormat::ForceImportFileFormat(std::string_view filename, ForceImportFileFormat::ForceImportFormat format)
    : FileAndFormat(filename, format)
{
    setUpKeywords();
}

ForceImportFileFormat::~ForceImportFileFormat() {}

/*
 * Keyword Options
 */

// Set up keywords for the format
void ForceImportFileFormat::setUpKeywords()
{
    keywords_.add("Conversion", new DoubleKeyword(1.0), "Factor", "Factor to multiply forces by (for unit conversion etc.)");
}

/*
 * Format Access
 */

// Return enum options for ForceImportFormat
EnumOptions<ForceImportFileFormat::ForceImportFormat> &ForceImportFileFormat::forceImportFormats()
{
    static EnumOptionsList ForceImportFileFormats =
        EnumOptionsList() << EnumOption(ForceImportFileFormat::DLPOLYForces, "dlpoly", "DL_POLY Config File Forces")
                          << EnumOption(ForceImportFileFormat::MoscitoForces, "moscito", "Moscito Structure File Forces")
                          << EnumOption(ForceImportFileFormat::SimpleForces, "simple", "Simple Free-Formatted Forces");

    static EnumOptions<ForceImportFileFormat::ForceImportFormat> options("ForceImportFileFormat", ForceImportFileFormats);

    return options;
}

// Return number of available formats
int ForceImportFileFormat::nFormats() const { return ForceImportFileFormat::nForceImportFormats; }

// Return format keyword for supplied index
std::string_view ForceImportFileFormat::formatKeyword(int id) const { return forceImportFormats().keywordByIndex(id); }

// Return description string for supplied index
std::string_view ForceImportFileFormat::formatDescription(int id) const { return forceImportFormats().descriptionByIndex(id); }

// Return current format as ForceImportFormat
ForceImportFileFormat::ForceImportFormat ForceImportFileFormat::forceFormat() const
{
    return (ForceImportFileFormat::ForceImportFormat)format_;
}

/*
 * Import Functions
 */

// Read forces using current filename and format
bool ForceImportFileFormat::importData(Array<double> &fx, Array<double> &fy, Array<double> &fz, ProcessPool *procPool)
{
    // Open file and check that we're OK to proceed importing from it
    LineParser parser(procPool);
    if ((!parser.openInput(filename_)) || (!parser.isFileGoodForReading()))
        return Messenger::error("Couldn't open file '{}' for loading forces data.\n", filename_);

    // Import the data
    auto result = importData(parser, fx, fy, fz);

    parser.closeFiles();

    return result;
}

// Import forces using supplied parser and current format
bool ForceImportFileFormat::importData(LineParser &parser, Array<double> &fx, Array<double> &fy, Array<double> &fz)
{
    // Import the data
    auto result = false;
    switch (forceFormat())
    {
        case (ForceImportFileFormat::DLPOLYForces):
            result = importDLPOLY(parser, fx, fy, fz);
            break;
        case (ForceImportFileFormat::MoscitoForces):
            result = importMoscito(parser, fx, fy, fz);
            break;
        case (ForceImportFileFormat::SimpleForces):
            result = importSimple(parser, fx, fy, fz);
            break;
        default:
            Messenger::error("Don't know how to load forces in format '{}'.\n", formatKeyword(forceFormat()));
    }

    // Apply factor to data
    auto factor = keywords_.asDouble("Factor");
    fx *= factor;
    fy *= factor;
    fz *= factor;

    return result;
}
