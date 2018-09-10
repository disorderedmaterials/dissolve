/*
	*** Twist Module
	*** src/modules/twist.h
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

#ifndef DISSOLVE_TWIST_H
#define DISSOLVE_TWIST_H

#include "module/module.h"

// Forward Declarations
/* none */

/*
 * Twist Module
 * ----------------
 *
 * XXX
 */
class Twist : public Module
{
	public:
	// Constructor
	Twist();
	// Destructor
	~Twist();


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
	// Return type of module
	const char* type() const;
	// Return brief description of module
	const char* brief() const;
	// Return type of module
	ModuleType type();


	/*
	 * Processing
	 */
	protected:
	// Run main processing
	bool execute(Dissolve& dissolve);
};

#endif

