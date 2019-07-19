/*
	*** Analyse Module - Core
	*** src/modules/analyse/core.cpp
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

#include "modules/analyse/analyse.h"

/*
 * Constructor / Destructor
 */

// Constructor
AnalyseModule::AnalyseModule() : Module(), analyser_(ProcedureNode::AnalysisContext)
{
	// Set up variables / control parameters
	setUpKeywords();

	// Set the End* keyword for the Analyser
	analyser_.setEndKeyword("EndAnalyser");
}

// Destructor
AnalyseModule::~AnalyseModule()
{
}

/*
 * Instances
 */

// Create instance of this module
Module* AnalyseModule::createInstance() const
{
	return new AnalyseModule;
}
