/*
	*** Analyser
	*** src/analyse/analyser.cpp
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

#include "analyse/analyser.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
Analyser::Analyser() : rootSequence_("EndAnalyser")
{
}

// Destructor
Analyser::~Analyser()
{
}

/*
 * Execute
 */

// Run analysis for specified Configuration, storing / retrieving generated data from supplied list
bool Analyser::execute(ProcessPool& procPool, Configuration* cfg, const char* dataPrefix, GenericList& targetList)
{
	// Check that the Configuration has changed before we do any more analysis on it
	RefListItem<Configuration,int>* ri = configurationPoints_.contains(cfg);
	if (ri)
	{
		// A Configuration we've processed before - check the index
		if (cfg->coordinateIndex() == ri->data)
		{
			Messenger::warn("Refusing to analyse Configuration '%s' since it has not changed.\n", cfg->name());
			return true;
		}
		else ri->data = cfg->coordinateIndex();
	}
	else configurationPoints_.add(cfg, cfg->coordinateIndex());

	// Prepare the nodes
	if (!rootSequence_.prepare(cfg, dataPrefix, targetList)) return Messenger::error("Failed to prepare analysis sequence for execution.\n");

	// Execute the root sequence
	bool result = rootSequence_.execute(procPool, cfg, dataPrefix, targetList);

	return result;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool Analyser::read(LineParser& parser)
{
	SiteContextStack contextStack;
	return rootSequence_.read(parser, contextStack);
}

// Write structure to specified LineParser
bool Analyser::write(LineParser& parser, const char* prefix)
{
	return rootSequence_.write(parser, prefix);
}
