/*
	*** Species XYZ Import
	*** src/lib/classes/species_xyz.cpp
	Copyright T. Youngs 2012-2014

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "classes/species.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include <string.h>

/*!
 * \brief Load Species from file
 */
bool Species::load(const char* fileName)
{
	// Grab extension from fileName
	Dnchar ext = lowerCase(afterLastChar(fileName, '.'));

	if (ext == "xyz") return loadFromXYZ(fileName);
	else msg.print("Can't load - unknown extension for file '%s'.\n", fileName);

	return false;
}

/*!
 * \brief Load Species information from XYZ file
 * \details Load atomic data from specified XYZ file.
 */
bool Species::loadFromXYZ(const char* fileName)
{
	msg.print("Loading XYZ data from file '%s'\n", fileName);
	
	// Open the specified file...
	LineParser parser;
	parser.openInput(fileName);
	if (!parser.isFileGoodForReading())
	{
		msg.error("Couldn't open XYZ file '%s'.\n", fileName);
		return false;
	}

	// Clear any existing data
	clear();

	// Simple format - first line is number of atoms, next line is title, then follow atoms/coordinates, one atom per line
	parser.getArgsDelim(LineParser::Defaults);
	int nAtoms = parser.argi(0);
	parser.readNextLine(LineParser::Defaults);
	name_ = parser.line();
	int el, success;
	for (int n=0; n<nAtoms; ++n)
	{
		success = parser.getArgsDelim(LineParser::Defaults);
		if (success != 0)
		{
			parser.closeFiles();
			msg.error("Couldn't read Atom %i from file '%s'\n", n+1, fileName);
			return false;
		}
		el = PeriodicTable::find(parser.argc(0));
		if (el == -1) el = 0;
		SpeciesAtom* i = addAtom(el, parser.argd(1), parser.argd(2),parser.argd(3));
		if (parser.hasArg(4)) i->setCharge(parser.argd(4));
	}

	msg.print("Succesfully loaded XYZ data from file '%s'.\n", fileName);
	parser.closeFiles();
	return true;
}
