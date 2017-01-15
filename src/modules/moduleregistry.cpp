/*
	*** dUQ Module Registry
	*** src/modules/moduleregistry.cpp
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

#include "modules/moduleregistry.h"
#include "modules/atomshake.h"
#include "modules/energy.h"
#include "modules/export.h"
#include "modules/partials.h"
#include "modules/test.h"

/*
 * Module Registration
 * ===================
 * All modules to be registered for use in the code must be placed here.
 */

// Constructor
ModuleRegistry::ModuleRegistry()
{
	ModuleRegistrar<AtomShake> atomShakeRegistrar;
	ModuleRegistrar<Energy> energyRegistrar;
	ModuleRegistrar<Export> exportRegistrar;
	ModuleRegistrar<Partials> partailsRegistrar;
	ModuleRegistrar<Test> testRegistrar;
}
