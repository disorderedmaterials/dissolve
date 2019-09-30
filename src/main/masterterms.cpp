/*
	*** Dissolve - Master Ternms
	*** src/main/masterterms.cpp
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

#include "main/dissolve.h"
#include "classes/speciesangle.h"
#include "classes/speciesbond.h"
#include "classes/speciestorsion.h"

// Return list of master Bond parameters
const List<MasterIntra>& Dissolve::masterBonds() const
{
	return coreData_.masterBonds();
}

// Return list of master Angle parameters
const List<MasterIntra>& Dissolve::masterAngles() const
{
	return coreData_.masterAngles();
}

// Return list of master Torsion parameters
const List<MasterIntra>& Dissolve::masterTorsions() const
{
	return coreData_.masterTorsions();
}

// Check and print MasterTerm usage
void Dissolve::checkMasterTermUsage() const
{
	Array<int> bondCounts(coreData_.masterBonds().nItems());
	Array<int> angleCounts(coreData_.masterAngles().nItems());
	Array<int> torsionCounts(coreData_.masterTorsions().nItems());
	bondCounts = 0;
	angleCounts = 0;
	torsionCounts = 0;

	// Loop over Configurations and count term usage
	int termIndex;
	ListIterator<Configuration> configIterator(coreData_.constConfigurations());
	while (Configuration* cfg = configIterator.iterate())
	{
		// Bonds
		DynamicArrayIterator<Bond> bondIterator(cfg->bonds());
		while (Bond* b = bondIterator.iterate())
		{
			if (!b->speciesBond()->masterParameters()) continue;
			termIndex = coreData_.masterBonds().indexOf(b->speciesBond()->masterParameters());
			++bondCounts[termIndex];
		}

		// Angles
		DynamicArrayIterator<Angle> angleIterator(cfg->angles());
		while (Angle* a = angleIterator.iterate())
		{
			if (!a->speciesAngle()->masterParameters()) continue;
			termIndex = coreData_.masterAngles().indexOf(a->speciesAngle()->masterParameters());
			++angleCounts[termIndex];
		}

		// Torsions
		DynamicArrayIterator<Torsion> torsionIterator(cfg->torsions());
		while (Torsion* t = torsionIterator.iterate())
		{
			if (!t->speciesTorsion()->masterParameters()) continue;
			termIndex = coreData_.masterTorsions().indexOf(t->speciesTorsion()->masterParameters());
			++torsionCounts[termIndex];
		}
	}

	if (coreData_.nMasterBonds() > 0)
	{
		Messenger::print("\n  Bond Masters:\n");

		Messenger::print("\n     Name        Usage Count\n");
		Messenger::print("    --------------------------------------------------------------------------\n");
		int n = 0;
		for (MasterIntra* b = coreData_.masterBonds().first(); b != NULL; b = b->next(), ++n) Messenger::print("     %-10s   %i", b->name(), bondCounts[n]);
	}

	if (coreData_.nMasterAngles() > 0)
	{
		Messenger::print("\n  Angle Masters:\n");

		Messenger::print("\n     Name        Usage Count\n");
		Messenger::print("    --------------------------------------------------------------------------\n");
		int n = 0;
		for (MasterIntra* a = coreData_.masterAngles().first(); a != NULL; a = a->next(), ++n) Messenger::print("     %-10s   %i", a->name(), angleCounts[n]);
	}

	if (coreData_.nMasterTorsions() > 0)
	{
		Messenger::print("\n  Torsion Masters:\n");

		Messenger::print("\n     Name        Usage Count\n");
		Messenger::print("    --------------------------------------------------------------------------\n");
		int n = 0;
		for (MasterIntra* t = coreData_.masterTorsions().first(); t != NULL; t = t->next(), ++n) Messenger::print("     %-10s   %i", t->name(), torsionCounts[n]);
	}
}
