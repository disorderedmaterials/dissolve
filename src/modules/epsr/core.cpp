/*
	*** EPSR Module - Core
	*** src/modules/epsr/core.cpp
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

#include "modules/epsr/epsr.h"

// Static Members
List<Module> EPSRModule::instances_;

/*
 * Constructor / Destructor
 */

// Constructor
EPSRModule::EPSRModule() : Module()
{
	// Add to instances list and set unique name for this instance
	instances_.own(this);
	uniqueName_.sprintf("%s%02i", type(), instances_.nItems()-1);

	// Set up variables / control parameters
	setUpKeywords();
}

// Destructor
EPSRModule::~EPSRModule()
{
}

/*
 * Instances
 */

// Create instance of this module
List<Module>& EPSRModule::instances()
{
	return instances_;
}

// Create instance of this module
Module* EPSRModule::createInstance()
{
	return new EPSRModule;
}
