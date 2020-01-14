/*
	*** Import - XYZ Forces
	*** src/io/import/forces_xyz.cpp
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

// Read XYZ forces from specified file
bool ForceImportFileFormat::importXYZ(LineParser& parser, Array<double>& fx, Array<double>& fy, Array<double>& fz)
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
