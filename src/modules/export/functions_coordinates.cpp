/*
	*** Export Module - Coordinate Functions
	*** src/modules/export/functions_coordinates.cpp
	Copyright T. Youngs 2012-2019

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.Coordinate

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "modules/export/export.h"
#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "data/atomicmass.h"
#include "base/lineparser.h"

// Write coordinates in specified format
bool ExportModule::writeCoordinates(CoordinateExportFileFormat::CoordinateExportFormat format, LineParser& parser, Configuration* cfg)
{
	// Check supplied format
	if (format == CoordinateExportFileFormat::XYZCoordinates) return writeXYZCoordinates(parser, cfg);
	else if (format == CoordinateExportFileFormat::DLPOLYCoordinates) return writeDLPOLYCoordinates(parser, cfg);

	Messenger::error("Unrecognised coordinate format - '%s'.\nKnown formats are: %s.\n", format, CoordinateExportFileFormat().formats());
	return false;
}

// Write Configuration in specified format
bool ExportModule::writeCoordinates(CoordinateExportFileFormat::CoordinateExportFormat format, const char* filename, Configuration* cfg)
{
	// Open the file
	LineParser parser;
	if (!parser.openOutput(filename))
	{
		parser.closeFiles();
		return false;
	}

	bool result = writeCoordinates(format, parser, cfg);

	parser.closeFiles();

	return result;
}

// Write coordinates as XYZ
bool ExportModule::writeXYZCoordinates(LineParser& parser, Configuration* cfg)
{
	// Write number of atoms and title
	if (!parser.writeLineF("%i\n", cfg->nAtoms())) return false;
	if (!parser.writeLineF("%s @ %i\n", cfg->name(), cfg->contentsVersion())) return false;
	
	// Write Atoms
	for (int n=0; n<cfg->nAtoms(); ++n)
	{
		Atom* i = cfg->atom(n);
		if (!parser.writeLineF("%-3s   %15.9f  %15.9f  %15.9f\n", i->element()->symbol(), i->r().x, i->r().y, i->r().z)) return false;
	}

	return true;
}

// Write coordinates as CONFIG
bool ExportModule::writeDLPOLYCoordinates(LineParser& parser, Configuration* cfg)
{
	// Write title
	if (!parser.writeLineF("%s @ %i\n", cfg->name(), cfg->contentsVersion())) return false;

	// Write keytrj and imcon
	if (cfg->box()->type() == Box::NonPeriodicBoxType) if (!parser.writeLineF("%10i%10i\n", 0, 0)) return false;
	else if (cfg->box()->type() == Box::CubicBoxType) if (!parser.writeLineF("%10i%10i\n", 0, 1)) return false;
	else if (cfg->box()->type() == Box::OrthorhombicBoxType) if (!parser.writeLineF("%10i%10i\n", 0, 2)) return false;
	else parser.writeLineF("%10i%10i\n", 0, 3);
	
	// Write Cell
	if (cfg->box()->type() != Box::NonPeriodicBoxType)
	{
		Matrix3 axes = cfg->box()->axes();
		if (!parser.writeLineF("%20.12f%20.12f%20.12f\n", axes[0], axes[1], axes[2])) return false;
		if (!parser.writeLineF("%20.12f%20.12f%20.12f\n", axes[3], axes[4], axes[5])) return false;
		if (!parser.writeLineF("%20.12f%20.12f%20.12f\n", axes[6], axes[7], axes[8])) return false;
	}

	// Write Atoms
	for (int n=0; n<cfg->nAtoms(); ++n)
	{
		Atom* i = cfg->atom(n);
		if (!parser.writeLineF("%-6s%10i%20.10f\n%20.12f%20.12f%20.12f\n", cfg->usedAtomType(i->localTypeIndex())->name(), n+1, AtomicMass::mass(i->element()), i->r().x, i->r().y, i->r().z)) return false;
	}

	return true;
}
