/*
	*** Export PairPotentials Module - Processing
	*** src/modules/export_pairpotentials/process.cpp
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

#include "modules/export_pairpotentials/exportpp.h"
#include "main/dissolve.h"
#include "classes/atom.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "base/sysfunc.h"
#include "base/lineparser.h"

// Run main processing
bool ExportPairPotentialsModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	if (!pairPotentialFormat_.hasValidFileAndFormat()) return Messenger::error("No valid file/format set for pair potential export.\n");

	// Only the pool master saves the data
	if (procPool.isMaster())
	{
		// Store the current (root) pair potential filename
		CharString rootPPName = pairPotentialFormat_.filename();

		for (PairPotential* pp = dissolve.pairPotentials().first(); pp != NULL; pp = pp->next())
		{
			Messenger::print("Export: Writing pair potential file (%s) for %s-%s...\n", pairPotentialFormat_.description(), pp->atomTypeNameI(), pp->atomTypeNameJ());

			// Generate filename
			pairPotentialFormat_.setFilename(CharString("%s-%s-%s.pp", rootPPName.get(), pp->atomTypeNameI(), pp->atomTypeNameJ()));

			// Append pair potential
			if (!pairPotentialFormat_.exportData(pp))
			{
				Messenger::print("Export: Failed to export pair potential file '%s'.\n", pairPotentialFormat_.filename());
				pairPotentialFormat_.setFilename(rootPPName);
				procPool.decideFalse();
				return false;
			}

			procPool.decideTrue();
		}

		// Revert root name in FileAndFormat
		pairPotentialFormat_.setFilename(rootPPName);
	}
	else if (!procPool.decision()) return false; 

	return true;
}
