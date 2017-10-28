/*
	*** GrainShake Module
	*** src/modules/grainshake.h
	Copyright T. Youngs 2012-2017

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

#ifndef DUQ_GRAINSHAKEMODULE_H
#define DUQ_GRAINSHAKEMODULE_H

#include "module/module.h"

// Forward Declarations
/* none */

/*
 * GrainShake Module
 * ----------------
 *
 * Performs a grain-by-grain Monte Carlo cycle over all grains in a configuration
 */
class GrainShakeModule : public Module
{
	public:
	// Constructor
	GrainShakeModule();
	// Destructor
	~GrainShakeModule();


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


	/*
	 * Method
	 */
	protected:
	// Execute method
	bool execute(DUQ& duq);
};

#endif

