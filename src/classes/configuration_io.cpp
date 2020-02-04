/*
	*** Configuration I/O
	*** src/classes/configuration_io.cpp
	Copyright T. Youngs 2012-2020

	This file is part of Configuration.

	Configuration is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Configuration is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Configuration.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "classes/configuration.h"
#include "classes/box.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Write through specified LineParser
bool Configuration::write(LineParser& parser) const
{
	int molId;

	if (!parser.writeLineF("'%s'  %i  # nMolecules\n", name(), molecules_.nItems())) return false;

	// Write unit cell (box) lengths and angles
	Vec3<double> lengths = box()->axisLengths();
	Vec3<double> angles = box()->axisAngles();
	if (!parser.writeLineF("%12e %12e %12e  %f  %s\n", lengths.x, lengths.y, lengths.z, requestedSizeFactor_, DissolveSys::btoa(box()->type() == Box::NonPeriodicBoxType))) return false;
	if (!parser.writeLineF("%12e %12e %12e\n", angles.x, angles.y, angles.z)) return false;

	// Write total number of Molecules
	if (!parser.writeLineF("%i\n", molecules_.nItems())) return false;

	// Write Molecule types - write sequential Molecules with same type as single line
	int moleculeCount = 0;
	const Species* lastType = NULL;
	for (int n=0; n<molecules_.nItems(); ++n)
	{
		// If the last Molecule's Species is the same as this one, increment counter and move on
		if (lastType == molecules_.constValue(n)->species())
		{
			++moleculeCount;
			continue;
		}

		// Species is different between this molecule and the last - write this info, and reset the counter
		if (lastType && (!parser.writeLineF("%i  '%s'\n", moleculeCount, lastType->name()))) return false;
		moleculeCount = 1;
		lastType = molecules_.constValue(n)->species();
	}
	// Write final molecule count / type
	if ((moleculeCount > 0) && (!parser.writeLineF("%i  '%s'\n", moleculeCount, lastType->name()))) return false;

	// Write all Atoms - for each write index and coordinates
	if (!parser.writeLineF("%i  # nAtoms\n", atoms_.nItems())) return false;
	for (int n=0; n<atoms_.nItems(); ++n)
	{
		const Atom* i = atoms_.constValue(n);
		if (!parser.writeLineF("%i %e %e %e\n", i->molecule()->arrayIndex(), i->x(), i->y(), i->z())) return false;
	}

	return true;
}

// Read through specified LineParser
bool Configuration::read(LineParser& parser, const List<Species>& availableSpecies, double pairPotentialRange)
{
	// Clear current contents of Configuration
	empty();

	// Read configuration name and nMolecules
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	setName(parser.argc(0));

	/*
	 * Read box definition
	 * Lengths, along with atomic coordinates, reflect the specified size factor (if present).
	 * Create box with unscaled lengths - they will be scaled according to the size factor at the end of the routine.
	 */
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	Vec3<double> scaledLengths = parser.arg3d(0);
	requestedSizeFactor_ = (parser.hasArg(3) ? parser.argd(3) : 1.0);
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;

	appliedSizeFactor_ = requestedSizeFactor_;
	Vec3<double> lengths  = scaledLengths / appliedSizeFactor_;
	Vec3<double> angles = parser.arg3d(0);
	if (!createBox(lengths, angles)) return false;

	// Read total number of Molecules to expect
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	const int expectedNMols = parser.argi(0);

	// Read Species types for Molecules
	int nMolsRead = 0;
	Species* sp = NULL;
	while (nMolsRead < expectedNMols)
	{
		// Read line containing number of molecules and Species name
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		for (sp = availableSpecies.first(); sp != NULL; sp = sp->next()) if (DissolveSys::sameString(sp->name(), parser.argc(1))) break;
		if (!sp) return Messenger::error("Unrecognised Species '%s' found in Configuration '%s' in restart file.\n", parser.argc(1), name());

		// Set Species pointers for this range of Molecules
		int nMols = parser.argi(0);
		for (int n=0; n<nMols; ++n) addMolecule(sp);

		// Increase our counter
		nMolsRead += parser.argi(0);
	}

	// Read in Atoms
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	int nAtoms = parser.argi(0);
	for (int n=0; n<nAtoms; ++n)
	{
		// Each line contains molecule ID and coordinates only
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;

		atom(n)->setCoordinates(parser.arg3d(1));
	}

	// Finalise used AtomType list
	usedAtomTypes_.finalise();

	// Set-up Cells for the Box
	cells_.generate(box_, requestedCellDivisionLength_, pairPotentialRange);

	// Scale box and cells according to the applied size factor
	scaleBox(appliedSizeFactor_);
	
	// Update Cell locations for Atoms
	updateCellContents();

	return true;
}
