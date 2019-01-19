/*
	*** Module Reference
	*** src/module/reference.cpp
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

#include "module/reference.h"

// Constructor
ModuleReference::ModuleReference() : ListItem<ModuleReference>()
{
	module_ = NULL;
	configuration_ = NULL;
	parentList_ = NULL;
}

// Destructor
ModuleReference::~ModuleReference()
{
}

/*
 * Data
 */

// Set Module and location
void ModuleReference::set(Module* module, ModuleList* parentList, Configuration* cfg)
{
	module_ = module;
	parentList_ = parentList;
	configuration_ = cfg;
}

// Return referenced Module
Module* ModuleReference::module()
{
	return module_;
}

// Return referenced Configuration
Configuration* ModuleReference::configuration()
{
	return configuration_;
}

// Return parent ModuleList
ModuleList* ModuleReference::parentList()
{
	return parentList_;
}

// Return whether the Module is a processing Module (i.e. is not in a Configuration)
bool ModuleReference::isProcessingModule() const
{
	return (configuration_ == NULL);
}
