/*
	*** Import Module - Forces Functions
	*** src/modules/import/functions_forces.cpp
	Copyright T. Youngs 2012-2018

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

#include "modules/import/import.h"
#include "base/lineparser.h"

// Read forces in specified format
bool ImportModule::readForces(ForceImportFileFormat::ForceImportFormat format, LineParser& parser, Array<double>& fx, Array<double>& fy, Array<double>& fz)
{
	// Check supplied format
	if (format == ForceImportFileFormat::XYZForces) return readSimpleForces(parser, fx, fy, fz);
	else if (format == ForceImportFileFormat::DLPOLYForces) return readDLPOLYForces(parser, fx, fy, fz);

	Messenger::error("Unrecognised force format - '%s'.\nKnown formats are: %s.\n", format, ForceImportFileFormat().formats());
	return false;
}

// Read simple forces from specified file
bool ImportModule::readSimpleForces(LineParser& parser, Array<double>& fx, Array<double>& fy, Array<double>& fz)
{
	/*
	 * Read force information through the specified line parser.
	 * Assumed format is as follows:
	 * 
	 * Line 1:    nAtoms
	 * Line 2:    <atom id>    <fx>    <fy>    <fz>
	 *   ...
	 */

	// Read in number of atoms and initiliase arrays
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	int nAtoms = parser.argi(0);
	Messenger::print(" --> Expecting forces for %i atoms.\n", nAtoms);
	fx.initialise(nAtoms);
	fy.initialise(nAtoms);
	fz.initialise(nAtoms);

	for (int n=0; n<nAtoms; ++n)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		fx[n] = parser.argd(0);
		fy[n] = parser.argd(1);
		fz[n] = parser.argd(2);
	}

	return true;
}

// Read DL_POLY forces from specified file
bool ImportModule::readDLPOLYForces(LineParser& parser, Array<double>& fx, Array<double>& fy, Array<double>& fz)
{
	/*
	 * Read DL_POLY force information through the specified line parser.
	 * We assume CONFIG or REVCON format:
	 * 
	 * Line 1:    Title
	 * Line 2:    keytrj   imcon    natoms    []
	 * Line 3-5:  cell matrix (if imcon > 0)
	 * Line 6:    atomtype        id
	 * Line 7:    rx   ry   rz
	 * Line 8:    vx   vy   vz      if (keytrj > 0)
	 * Line 9:    fx   fy   fz	if (keytrj > 1)
	 *   ...
	 */

	Messenger::print(" --> Reading forces in DL_POLY (CONFIG/REVCON) format...\n");
	// Skip title
	if (parser.skipLines(1) != LineParser::Success) return false;

	// Read in keytrj, imcon, and number of atoms, and initiliase arrays
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;

	int keytrj = parser.argi(0);
	int imcon = parser.argi(1);
	int nAtoms = parser.argi(2);
	if (keytrj != 2)
	{
		Messenger::error("File does not contain forces (keytrj = %i).\n", keytrj);
		return false;
	}
	Messenger::print(" --> Expecting forces for %i atoms (DLPOLY keytrj=%i, imcon=%i).\n", nAtoms, keytrj, imcon);
	fx.initialise(nAtoms);
	fy.initialise(nAtoms);
	fz.initialise(nAtoms);

	// Skip cell information if given
	if (imcon > 0) parser.skipLines(3);

	// Loop over atoms
	for (int n=0; n<nAtoms; ++n)
	{
		// Skip atomname, position and velocity lines
		if (parser.skipLines(3) != LineParser::Success) return false;
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		fx[n] = parser.argd(0);
		fy[n] = parser.argd(1);
		fz[n] = parser.argd(2);
	}

	return true;
}
