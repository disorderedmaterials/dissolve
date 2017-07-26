/*
	*** Export Module - Functions
	*** src/modules/export/functions.cpp
	Copyright T. Youngs 2012-2017

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

#include "modules/export/export.h"
#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "base/ptable.h"

// Write Configuration as XYZ
bool ExportModule::writeConfigurationXYZ(LineParser& parser, Configuration* cfg, const char* header)
{
	// Write number of atoms and title
	if (!parser.writeLineF("%i\n", cfg->nAtoms())) return false;
	if (!parser.writeLineF("%s\n", header)) return false;
	
	// Write Atoms
	for (int n=0; n<cfg->nAtoms(); ++n)
	{
		Atom* i = cfg->atom(n);
		if (!parser.writeLineF("%-3s   %15.9f  %15.9f  %15.9f\n", PeriodicTable::element(i->element()).symbol(), i->r().x, i->r().y, i->r().z)) return false;
	}

	return true;
}

// Write Configuration as CONFIG
bool ExportModule::writeConfigurationDLPOLY(LineParser& parser, Configuration* cfg, const char* header)
{
	// Write title
	parser.writeLineF("%s\n", header);

	// Write keytrj and imcon
	if (cfg->box()->type() == Box::NonPeriodicBox) parser.writeLineF("%10i%10i\n", 0, 0);
	else if (cfg->box()->type() == Box::CubicBox) parser.writeLineF("%10i%10i\n", 0, 1);
	else if (cfg->box()->type() == Box::OrthorhombicBox) parser.writeLineF("%10i%10i\n", 0, 2);
	else parser.writeLineF("%10i%10i\n", 0, 3);
	
	// Write Cell
	if (cfg->box()->type() != Box::NonPeriodicBox)
	{
		Matrix3 axes = cfg->box()->axes();
		parser.writeLineF("%20.12f%20.12f%20.12f\n", axes[0], axes[1], axes[2]);
		parser.writeLineF("%20.12f%20.12f%20.12f\n", axes[3], axes[4], axes[5]);
		parser.writeLineF("%20.12f%20.12f%20.12f\n", axes[6], axes[7], axes[8]);
	}

	// Write Atoms
	for (int n=0; n<cfg->nAtoms(); ++n)
	{
		Atom* i = cfg->atom(n);
		parser.writeLineF("%-6s%10i%20.10f\n%20.12f%20.12f%20.12f\n", cfg->type(i->localTypeIndex())->name(), n+1, PeriodicTable::element(i->element()).isotope(0)->atomicWeight(), i->r().x, i->r().y, i->r().z);
	}

	return true;
}
