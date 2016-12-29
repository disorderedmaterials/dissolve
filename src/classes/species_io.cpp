/*
	*** Species XYZ Import
	*** src/classes/species_xyz.cpp
	Copyright T. Youngs 2012-2016

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

// Load Species from file
bool Species::load(const char* filename)
{
	// Grab extension from filename
	CharString ext = DUQSys::lowerCase(DUQSys::afterLastChar(filename, '.'));

	if (ext == "xyz") return loadFromXYZ(filename);
	else Messenger::print("Can't load - unknown extension for file '%s'.\n", filename);

	return false;
}

// Load Species information from XYZ file
bool Species::loadFromXYZ(const char* filename)
{
	Messenger::print("Loading XYZ data from file '%s'\n", filename);
	
	// Open the specified file...
	LineParser parser;
	parser.openInput(filename);
	if (!parser.isFileGoodForReading())
	{
		Messenger::error("Couldn't open XYZ file '%s'.\n", filename);
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
			Messenger::error("Couldn't read Atom %i from file '%s'\n", n+1, filename);
			return false;
		}
		el = PeriodicTable::find(parser.argc(0));
		if (el == -1) el = 0;
		SpeciesAtom* i = addAtom(el, parser.argd(1), parser.argd(2),parser.argd(3));
		if (parser.hasArg(4)) i->setCharge(parser.argd(4));
	}

	Messenger::print("Succesfully loaded XYZ data from file '%s'.\n", filename);
	parser.closeFiles();
	return true;
}
