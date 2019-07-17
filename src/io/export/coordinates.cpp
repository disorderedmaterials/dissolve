/*
	*** Export - Coordinates
	*** src/io/export/coordinates.cpp
	Copyright T. Youngs 2012-2019

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

#include "io/export/coordinates.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "data/atomicmass.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Coordinate Type Keywords
const char* CoordinateExportFormatKeywords[] = { "xyz", "dlpoly" };
const char* NiceCoordinateExportFormatKeywords[] = { "XYZ Coordinates", "DL_POLY CONFIG File" };

// Return number of available formats
int CoordinateExportFileFormat::nFormats() const
{
	return CoordinateExportFileFormat::nCoordinateExportFormats;
}

// Return formats array
const char** CoordinateExportFileFormat::formats() const
{
	return CoordinateExportFormatKeywords;
}

// Return nice formats array
const char** CoordinateExportFileFormat::niceFormats() const
{
	return NiceCoordinateExportFormatKeywords;
}

// Return current format as CoordinateExportFormat
CoordinateExportFileFormat::CoordinateExportFormat CoordinateExportFileFormat::coordinateFormat() const
{
	return (CoordinateExportFileFormat::CoordinateExportFormat) format_;
}

// Constructor
CoordinateExportFileFormat::CoordinateExportFileFormat(const char* filename, CoordinateExportFormat format) : FileAndFormat(filename, format)
{
}

/*
 * Export Functions
 */

// Export coordinates as XYZ
bool CoordinateExportFileFormat::exportXYZ(LineParser& parser, Configuration* cfg)
{
	// Export number of atoms and title
	if (!parser.writeLineF("%i\n", cfg->nAtoms())) return false;
	if (!parser.writeLineF("%s @ %i\n", cfg->name(), cfg->contentsVersion())) return false;
	
	// Export Atoms
	for (int n=0; n<cfg->nAtoms(); ++n)
	{
		Atom* i = cfg->atom(n);
		if (!parser.writeLineF("%-3s   %15.9f  %15.9f  %15.9f\n", i->element()->symbol(), i->r().x, i->r().y, i->r().z)) return false;
	}

	return true;
}

// Export coordinates as CONFIG
bool CoordinateExportFileFormat::exportDLPOLY(LineParser& parser, Configuration* cfg)
{
	// Export title
	if (!parser.writeLineF("%s @ %i\n", cfg->name(), cfg->contentsVersion())) return false;

	// Export keytrj and imcon
	if (cfg->box()->type() == Box::NonPeriodicBoxType) if (!parser.writeLineF("%10i%10i\n", 0, 0)) return false;
	else if (cfg->box()->type() == Box::CubicBoxType) if (!parser.writeLineF("%10i%10i\n", 0, 1)) return false;
	else if (cfg->box()->type() == Box::OrthorhombicBoxType) if (!parser.writeLineF("%10i%10i\n", 0, 2)) return false;
	else parser.writeLineF("%10i%10i\n", 0, 3);
	
	// Export Cell
	if (cfg->box()->type() != Box::NonPeriodicBoxType)
	{
		Matrix3 axes = cfg->box()->axes();
		if (!parser.writeLineF("%20.12f%20.12f%20.12f\n", axes[0], axes[1], axes[2])) return false;
		if (!parser.writeLineF("%20.12f%20.12f%20.12f\n", axes[3], axes[4], axes[5])) return false;
		if (!parser.writeLineF("%20.12f%20.12f%20.12f\n", axes[6], axes[7], axes[8])) return false;
	}

	// Export Atoms
	for (int n=0; n<cfg->nAtoms(); ++n)
	{
		Atom* i = cfg->atom(n);
		if (!parser.writeLineF("%-6s%10i%20.10f\n%20.12f%20.12f%20.12f\n", cfg->usedAtomType(i->localTypeIndex())->name(), n+1, AtomicMass::mass(i->element()), i->r().x, i->r().y, i->r().z)) return false;
	}

	return true;
}

// Export coordinates using current filename and format
bool CoordinateExportFileFormat::exportData(Configuration* cfg)
{
	// Open the file
	LineParser parser;
	if (!parser.openOutput(filename_))
	{
		parser.closeFiles();
		return false;
	}

	// Write data
	bool result = false;
	if (coordinateFormat() == CoordinateExportFileFormat::XYZCoordinates) result = exportXYZ(parser, cfg);
	else if (coordinateFormat() == CoordinateExportFileFormat::DLPOLYCoordinates) result = exportDLPOLY(parser, cfg);
	else Messenger::error("Unrecognised coordinate format.\nKnown formats are: %s.\n", CoordinateExportFileFormat().formats());

	return result;
}
