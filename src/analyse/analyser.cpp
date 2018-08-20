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

// Run analysis for specified Configuration
bool Analyser::execute(Configuration* cfg)
{
	// Clear the 
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool Analyser::read(LineParser& parser)
{
	// Make sure the context stack is clear to begin with
	contextStack_.clear();

	return rootSequence_.read(parser, contextStack_);
}

// Write structure to specified LineParser
bool Analyser::write(LineParser& parser, const char* prefix)
{
	return rootSequence_.write(parser, prefix);
}
