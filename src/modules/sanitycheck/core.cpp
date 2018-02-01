/*
	*** SanityCheck Module - Core
	*** src/modules/sanitycheck/core.cpp
	Copyright T. Youngs 2012-2018

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

#include "modules/sanitycheck/sanitycheck.h"

// Static Members
List<Module> SanityCheckModule::instances_;

/*
 * Constructor / Destructor
 */

// Constructor
SanityCheckModule::SanityCheckModule() : Module()
{
	// Add to instances list and set unique name for this instance
	instances_.own(this);
	uniqueName_.sprintf("%s%02i", name(), instances_.nItems()-1);

	// Set up variables / control parameters
	setUpKeywords();
}

// Destructor
SanityCheckModule::~SanityCheckModule()
{
}

/*
 * Instances
 */

// Create instance of this module
List<Module>& SanityCheckModule::instances()
{
	return instances_;
}

// Create instance of this module
Module* SanityCheckModule::createInstance()
{
	return new SanityCheckModule;
}
