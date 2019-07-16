/*
	*** Export - Trajectory
	*** src/io/export/trajectory.cpp
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

#include "io/export/trajectory.h"
#include "classes/configuration.h"
#include "data/elements.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Trajectory Type Keywords
const char* TrajectoryExportFormatKeywords[] = { "xyz" };
const char* NiceTrajectoryExportFormatKeywords[] = { "XYZ Trajectory" };

// Return number of available formats
int TrajectoryExportFileFormat::nFormats() const
{
	return TrajectoryExportFileFormat::nTrajectoryExportFormats;
}

// Return formats array
const char** TrajectoryExportFileFormat::formats() const
{
	return TrajectoryExportFormatKeywords;
}

// Return nice formats array
const char** TrajectoryExportFileFormat::niceFormats() const
{
	return NiceTrajectoryExportFormatKeywords;
}

// Return current format as TrajectoryExportFormat
TrajectoryExportFileFormat::TrajectoryExportFormat TrajectoryExportFileFormat::trajectoryFormat() const
{
	return (TrajectoryExportFileFormat::TrajectoryExportFormat) format_;
}

// Constructor
TrajectoryExportFileFormat::TrajectoryExportFileFormat(const char* filename, TrajectoryExportFormat format) : FileAndFormat(filename, format)
{
}

/*
 * Export Functions
 */

// Append XYZ frame to trajectory
bool TrajectoryExportFileFormat::exportXYZ(LineParser& parser, Configuration* cfg)
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

// Append trajectory using current filename and format
bool TrajectoryExportFileFormat::exportData(Configuration* cfg)
{
	// Make an initial check to see if the specified file exists
	bool fileExists = DissolveSys::fileExists(filename_);

	// Open the specified file for appending
	LineParser parser;
	if (!parser.appendOutput(filename_))
	{
		parser.closeFiles();
		return false;
	}

	bool headerResult = false;

	// Write header?
	if (!fileExists)
	{
		// if (format == OneThatNeedsAHeader) headerResult = writeAHeader(parser, cfg);
		// else result = Messenger::error("Unrecognised trajectory format so can't write header.\nKnown formats are: %s.\n", TrajectoryExportFileFormat().formats());
	}
	if (!headerResult) return false;

	// Append frame in supplied format
	bool frameResult = false;
	if (trajectoryFormat() == TrajectoryExportFileFormat::XYZTrajectory) frameResult = exportXYZ(parser, cfg);
	else Messenger::error("Unrecognised trajectory format.\nKnown formats are: %s.\n", TrajectoryExportFileFormat().formats());

	parser.closeFiles();

	return frameResult;
}
