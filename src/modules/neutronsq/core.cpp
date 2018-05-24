/*
	*** Neutron SQ Module - Core Functions
	*** src/modules/neutronsq/core.cpp
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

#include "modules/neutronsq/neutronsq.h"

// Static Members
List<Module> NeutronSQModule::instances_;

/*
 * Constructor / Destructor
 */

// Constructor
NeutronSQModule::NeutronSQModule() : Module()
{
	// Add to instances list and set unique name for this instance
	uniqueName_.sprintf("%s%02i", name(), instances_.nItems());
	instances_.own(this);

	// Set up variables / control parameters
	setUpKeywords();
}

// Destructor
NeutronSQModule::~NeutronSQModule()
{
}

/*
 * Instances
 */

// Create instance of this module
List<Module>& NeutronSQModule::instances()
{
	return instances_;
}

// Create instance of this module
Module* NeutronSQModule::createInstance()
{
	return new NeutronSQModule;
}

