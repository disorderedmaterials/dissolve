/*
	*** Structure Factor Module
	*** src/modules/fq.h
	Copyright T. Youngs 2012-2016

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

#ifndef DUQ_STRUCTUREFACTOR_H
#define DUQ_STRUCTUREFACTOR_H

#include "modules/module.h"

// Forward Declarations
/* none */

// Structure Factor Module
class StructureFactor : public Module
{
	/*
	 * Calculates partial and full structure factors for all Samples associated to the Configuration.
	 * Partial RDFs according to atomtype isotopes are constructed, and combined into total F(Q).
	 * This module also handles calculation of Bragg scattering.
	 */

	public:
	// Constructor
	StructureFactor();
	// Destructor
	~StructureFactor();


	/*
	 * Instances
	 */
	public:
	// Create instance of this module
	Module* createInstance();


	/*
	 * Definition
	 */
	public:
	// Return name of module
	const char* name();
	// Return brief description of module
	const char* brief();
	// Return type of module
	ModuleType type();
	// Number of Configurations that this module requires to run
	int nConfigurationsRequired();
	// Return instance type for module
	InstanceType instanceType();


	/*
	 * Method
	 */
	protected:
	// Execute method
	bool execute(DUQ& duq);
};

#endif

