/*
	*** dUQ Module Registry
	*** src/module/registry.cpp
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

#include "module/registry.h"
#include "modules/atomshake/atomshake.h"
#include "modules/checks/checks.h"
#include "modules/energy/energy.h"
#include "modules/export/export.h"
#include "modules/forces/forces.h"
#include "modules/intrashake/intrashake.h"
#include "modules/md/md.h"
#include "modules/molshake/molshake.h"
#include "modules/partials/partials.h"
#include "modules/refine/refine.h"

/*
 * Module Registration
 * ===================
 * All modules to be registered for use in the code must be placed here.
 */

// Constructor
ModuleRegistry::ModuleRegistry()
{
	ModuleRegistrar<AtomShakeModule> atomShakeRegistrar;
	ModuleRegistrar<ChecksModule> checksRegistrar;
	ModuleRegistrar<EnergyModule> energyRegistrar;
	ModuleRegistrar<ExportModule> exportRegistrar;
	ModuleRegistrar<ForcesModule> forcesRegistrar;
	ModuleRegistrar<IntraShakeModule> intraShakeRegistrar;
	ModuleRegistrar<MDModule> mdRegistrar;
	ModuleRegistrar<MolShakeModule> molShakeRegistrar;
	ModuleRegistrar<PartialsModule> partailsRegistrar;
	ModuleRegistrar<RefineModule> testRegistrar;
}
