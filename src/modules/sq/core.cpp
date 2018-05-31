/*
	*** SQ Module - Core Functions
	*** src/modules/sq/core.cpp
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

#include "modules/sq/sq.h"

// Static Members
List<Module> SQModule::instances_;

/*
 * Constructor / Destructor
 */

// Constructor
SQModule::SQModule() : Module()
{
	// Add to instances list and set unique name for this instance
	uniqueName_.sprintf("%s%02i", name(), instances_.nItems());
	instances_.own(this);

	// Set up variables / control parameters
	setUpKeywords();

	// Set representative colour
	colour_[0] = 0;
	colour_[1] = 0;
	colour_[2] = 200;
}

// Destructor
SQModule::~SQModule()
{
}

/*
 * Instances
 */

// Create instance of this module
List<Module>& SQModule::instances()
{
	return instances_;
}

// Create instance of this module
Module* SQModule::createInstance()
{
	return new SQModule;
}

