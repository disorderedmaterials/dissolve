/*
	*** Import Module - Coordinates Functions
	*** src/modules/import/functions_coordinates.cpp
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

#include "modules/import/import.h"
#include "base/lineparser.h"

// Read coordinates in specified format
bool ImportModule::readCoordinates(CoordinateImportFileFormat::CoordinateImportFormat format, LineParser& parser, Array< Vec3<double> >& r)
{
	// Check supplied format
	if (format == CoordinateImportFileFormat::XYZCoordinates) return readXYZCoordinates(parser, r);
	else if (format == CoordinateImportFileFormat::DLPOLYCoordinates) return readDLPOLYCoordinates(parser, r);
	else if (format == CoordinateImportFileFormat::EPSRCoordinates) return readEPSRCoordinates(parser, r);

	Messenger::error("Don't know how to load coordinates in format '%s'.\n", CoordinateImportFileFormat().format(format));

	return false;
}

// Read xyz coordinates from specified file
bool ImportModule::readXYZCoordinates(LineParser& parser, Array< Vec3<double> >& r)
{
	Messenger::print(" --> Reading coordinates in xyz format...\n");

	// Read in natoms
	if (parser.getArgsDelim(LineParser::SkipBlanks) != LineParser::Success) return false;
	int nAtoms = parser.argi(0);

	// Skip title
	if (parser.skipLines(1) != LineParser::Success) return false;

	Messenger::print(" --> Expecting coordinates for %i atoms.\n", nAtoms);
	r.clear();
	for (int n=0; n<nAtoms; ++n)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		r.add(parser.arg3d(1));
	}

	return true;
}

// Read DL_POLY coordinates from specified file
bool ImportModule::readDLPOLYCoordinates(LineParser& parser, Array< Vec3<double> >& r)
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

	Messenger::print(" --> Reading coordinates in DL_POLY (CONFIG/REVCON) format...\n");
	// Skip title
	if (parser.skipLines(1) != LineParser::Success) return false;

	// Read in keytrj, imcon, and number of atoms, and initiliase arrays
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;

	int keytrj = parser.argi(0);
	int imcon = parser.argi(1);
	int nAtoms = parser.hasArg(2) ? parser.argi(2) : 0;
	Messenger::print(" --> Expecting coordinates for %i atoms (DLPOLY keytrj=%i, imcon=%i).\n", nAtoms, keytrj, imcon);
	r.clear();

	// Skip cell information if given
	if (imcon > 0) parser.skipLines(3);

	// Loop over atoms (either a specified number, or until we reach the end of the file
	int atomCount = 0;
	while (!parser.eofOrBlank())
	{
		// Skip atomname line, get the positions, then skip velocity and force lines if necessary
		if (parser.skipLines(1) != LineParser::Success) return false;
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		r.add(parser.arg3d(0));
		if (parser.skipLines(keytrj) != LineParser::Success) return false;
		++atomCount;
		if ((nAtoms > 0) && (atomCount == nAtoms)) break;
	}

	return true;
}

// Read EPSR ATO coordinates from specified file
bool ImportModule::readEPSRCoordinates(LineParser& parser, Array< Vec3<double> >& r)
{
	// File header:
	// Either  1   : nmols, box length, temperature   (for cubic systems)
	//    or   2   : nmols,   temperature             (for non-cubic systems)
	// followed by : A, B, C
	//             : phib, thetac, phic
	if (parser.getArgsDelim(LineParser::SkipBlanks) != LineParser::Success) return false;
	int nMols = parser.argi(0);
	if (parser.nArgs() == 3)
	{
		double boxSize = parser.argd(1);
		Messenger::print("File has a cubic cell (side length %f Angstroms)", boxSize);
// 		targetModel()->setCell(Vec3<double>(boxSize,boxSize,boxSize), Vec3<double>(90,90,90));
	}
	else
	{
		Messenger::print("File has a full cell specification");
		Vec3<double> lengths, angles;
		if (parser.getArgsDelim(LineParser::SkipBlanks) != LineParser::Success) return false;
		lengths = parser.arg3d(0);
		if (parser.getArgsDelim(LineParser::SkipBlanks) != LineParser::Success) return false;
		angles = parser.arg3d(0);

		// angles.x = phib = angle between a and b (== gamma)
		// angles.y = thetac = angle deviation from cartesian z (== 90-beta)  ?? CHECK
		// angles.z = phic = angle deviation from cartesian z (== 90-alpha)  ?? CHECK
// 		targetModel()->setCell(lengths, Vec3<double>(90-angles.z, 90-angles.y, angles.x));
	}

	// 2 : step sizes etc. **IGNORED**
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;

	// Molecule/atom specifications are in the form:
	// n  : natoms, comx, comy, comz, phix, phiy, phiz
	// n+1: atom name 1
	// n+2: x,y,z (offsets from com)
	// n+3: nrestraints, res1, res2... (number of distance restraints, 5 per line)
	// n+4: ...resN-1, resN
	// n+5: nrot (number of defined molecular rotations)
	// n+6: atom1, atom2 (bonds of rotation 'axis')
	// n+7: list of headgroup atoms that are rotated
	int atomOffset = 0;
	int nAtoms, nRestraints, currentArg;
	Vec3<double> com, delta;
	for (int m=0; m<nMols; m++)
	{
		Messenger::printVerbose("Reading molecule %i from EPSR ato file...\n", m+1);

		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		nAtoms = parser.argi(0);
		com = parser.arg3d(1);

		for (int n=0; n<nAtoms; n++)
		{
			// Atom name
			if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;

			// Atom coordinates (specified as offset from com)
			if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
			delta = parser.arg3d(0);

			// Add a new atom position to our list
			r.add(com+delta);

			// Read in number of restraints line
			if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
			nRestraints = parser.argi(0);
			currentArg = 1;
			while (nRestraints > 0)
			{
				// Look at next available argument - if none, read another line in
				if (currentArg >= parser.nArgs())
				{
					if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
					currentArg = 0;
				}
				//partnerId = parser.argi(currentArg) - 1;
				currentArg += 2;

				// Create new bond between these atoms (only if the partnerId is less than the current atom index)  **IGNORED**
// 				if (partnerId < n) targetModel()->bondAtoms(atomOffset+n, atomOffset+partnerId, Bond::Single);

				--nRestraints;
			}
		}

		// Discard molecular rotations and dihedrals
		// There are 14 atoms per line - first line contains number of atoms followed by (up to) 13 indices
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		int nRotations = parser.argi(0);
		while (nRotations > 0)
		{
			// Read line to find out which type of definition this is...
			if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;

			// Skip axis line
			if (parser.skipLines(1) != LineParser::Success) return false;

			// If a DIHedral, we expect an integer which defines the number of constraints, and thus the number of lines to skip before the main
			if (DissolveSys::sameString(parser.argc(0), "DIH"))
			{
				if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
				if (parser.skipLines(parser.argi(0)) != LineParser::Success) return false;
			}

			// Finally, read in number of atoms affected by rotation and calculate next number of lines to discard
			if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
			if (parser.skipLines(parser.argi(0)/14) != LineParser::Success) return false;

			--nRotations;
		}

		atomOffset += nAtoms;
	}

	// Atomtype specifications follow, but we have the coordinates and so are done
	return true;
}

