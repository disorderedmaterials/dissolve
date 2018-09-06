/*
	*** CalculateRDF Module - Processing
	*** src/modules/calculate/rdf/process.cpp
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

#include "modules/calculate/rdf/calculaterdf.h"
#include "main/dissolve.h"
#include "base/sysfunc.h"

// Run set-up stage
bool CalculateRDFModule::setUp(Dissolve& dissolve, ProcessPool& procPool)
{
	// Reconstruct the Analyser
// 	analyser_.clear();

	// Get relevant Module options
	SpeciesSite* originSite = KeywordListHelper<SpeciesSite*>::retrieve(keywords_, "OriginSite", NULL);
	SpeciesSite* otherSite = KeywordListHelper<SpeciesSite*>::retrieve(keywords_, "OtherSite", NULL);
	if (!originSite) return Messenger::error("Origin site is not defined.\n");
	if (!otherSite) return Messenger::error("Other (surrounding) site is not defined.\n");
}

// Run main processing
bool CalculateRDFModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	/*
	 * This is a XXX routine.
	 * XXX
	 */

	// Check for zero Configuration targets
	if (targetConfigurations_.nItems() == 0)
	{
		Messenger::warn("No Configuration targets for Module.\n");
		return true;
	}

	// Loop over target Configurations
	for (RefListItem<Configuration,bool>* ri = targetConfigurations_.first(); ri != NULL; ri = ri->next)
	{
		// Grab Configuration pointer
		Configuration* cfg = ri->item;

		// Set up process pool - must do this to ensure we are using all available processes
		procPool.assignProcessesToGroups(cfg->processPool());

		// MODULE CODE
	}

	return false;
}

