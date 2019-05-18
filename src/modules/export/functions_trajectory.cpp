/*
	*** Export Module - Trajectory Functions
	*** src/modules/export/functions_trajectory.cpp
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

#include "modules/export/export.h"
#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "data/atomicmass.h"
#include "base/lineparser.h"

// Append trajectory in specified format to file
bool ExportModule::writeTrajectory(TrajectoryExportFileFormat::TrajectoryExportFormat format, const char* filename, Configuration* cfg)
{
	// Make an initial check to see if the specified file exists
	bool fileExists = DissolveSys::fileExists(filename);

	// Open the specified file for appending
	LineParser parser;
	if (!parser.appendOutput(filename))
	{
		parser.closeFiles();
		return false;
	}

	bool result;

	// Write header?
	if (!fileExists)
	{
		// if (format == OneThatNeedsAHeader) result = writeAHeader(parser, cfg);
	}
	
	// Write frame in supplied format
	if (format == TrajectoryExportFileFormat::XYZTrajectory) result = writeXYZTrajectory(parser, cfg);
	else result = Messenger::error("Unrecognised trajectory format - '%s'.\nKnown formats are: %s.\n", format, TrajectoryExportFileFormat().formats());

	parser.closeFiles();

	return result;
}

// Append XYZ frame to trajectory
bool ExportModule::writeXYZTrajectory(LineParser& parser, Configuration* cfg)
{
	// Write number of atoms and title
	if (!parser.writeLineF("%i\n", cfg->nAtoms())) return false;
	if (!parser.writeLineF("%s @ %i\n", cfg->name(), cfg->coordinateIndex())) return false;
	
	// Write Atoms
	for (int n=0; n<cfg->nAtoms(); ++n)
	{
		Atom* i = cfg->atom(n);
		if (!parser.writeLineF("%-3s   %15.9f  %15.9f  %15.9f\n", i->element()->symbol(), i->r().x, i->r().y, i->r().z)) return false;
	}

	return true;
}
