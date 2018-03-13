/*
	*** Module Reference
	*** src/module/reference.h
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

#ifndef DUQ_MODULEREFERENCE_H
#define DUQ_MODULEREFERENCE_H

#include "templates/listitem.h"

// Forward Declarations
class Module;
class Configuration;

// Module Reference
class ModuleReference : public ListItem<ModuleReference>
{
	public:
	// Constructor
	ModuleReference();
	// Destructor
	~ModuleReference();
	// Conversion to Module*
	operator Module*();


	/*
	 * Data
	 */
	private:
	// Pointer to instance of Module
	Module* module_;
	// Configuration location of Module (if relevant)
	Configuration* configuration_;

	public:
	// Set Module and location
	void set(Module* module, Configuration* cfg = NULL);
	// Return referenced Module
	Module* module();
	// Return referenced Configuration
	Configuration* configuration();
	// Return whether the Module is a processing Module (i.e. is not in a Configuration)
	bool isProcessingModule() const;
};

#endif

