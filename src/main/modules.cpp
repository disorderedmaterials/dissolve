/*
	*** Dissolve - Modules
	*** src/main/modules.cpp
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

#include "main/dissolve.h"
#include "modules/analyse/analyse.h"
#include "modules/atomshake/atomshake.h"
#include "modules/calculate/rdf/rdf.h"
#include "modules/checks/checks.h"
#include "modules/datatest/datatest.h"
#include "modules/energy/energy.h"
#include "modules/epsr/epsr.h"
#include "modules/export/export.h"
#include "modules/forces/forces.h"
#include "modules/import/import.h"
#include "modules/intrashake/intrashake.h"
#include "modules/md/md.h"
#include "modules/molshake/molshake.h"
#include "modules/calibration/calibration.h"
#include "modules/rdf/rdf.h"
#include "modules/refine/refine.h"
#include "modules/sanitycheck/sanitycheck.h"
#include "modules/neutronsq/neutronsq.h"
#include "modules/test/test.h"

/*
 * Module Registration
 * ===================
 * All modules to be registered for use in the code must be placed here.
 */

// Register master Module
bool Dissolve::registerMasterModule(Module* masterInstance)
{
	// Do sanity check on name
	ListIterator<Module> moduleIterator(masterModules_);
	while (Module* module = moduleIterator.iterate())
	{
		if (DissolveSys::sameString(module->type(), masterInstance->type()))
		{
			Messenger::error("Two modules cannot have the same name (%s).\n", module->type());
			return false;
		}
	}

	masterModules_.own(masterInstance);

	return true;
}

// Register master instances for all Modules
bool Dissolve::registerMasterModules()
{
	if (!registerMasterModule(new AnalyseModule)) return false;
	if (!registerMasterModule(new AtomShakeModule)) return false;
	if (!registerMasterModule(new CalculateRDFModule)) return false;
	if (!registerMasterModule(new CalibrationModule)) return false;
	if (!registerMasterModule(new ChecksModule)) return false;
	if (!registerMasterModule(new DataTestModule)) return false;
	if (!registerMasterModule(new EnergyModule)) return false;
	if (!registerMasterModule(new EPSRModule)) return false;
	if (!registerMasterModule(new ExportModule)) return false;
	if (!registerMasterModule(new ForcesModule)) return false;
	if (!registerMasterModule(new ImportModule)) return false;
	if (!registerMasterModule(new IntraShakeModule)) return false;
	if (!registerMasterModule(new MDModule)) return false;
	if (!registerMasterModule(new MolShakeModule)) return false;
	if (!registerMasterModule(new RDFModule)) return false;
	if (!registerMasterModule(new RefineModule)) return false;
	if (!registerMasterModule(new SanityCheckModule)) return false;
	if (!registerMasterModule(new NeutronSQModule)) return false;
	if (!registerMasterModule(new TestModule)) return false;

	Messenger::print("Module Information (%i available):\n", masterModules_.nItems());
	ListIterator<Module> moduleIterator(masterModules_);
	while (Module* module = moduleIterator.iterate())
	{
		Messenger::print(" --> %s\n", module->type());
		Messenger::print("     %s\n", module->brief());
	}
}

// Search for master Module of the named type
Module* Dissolve::findMasterModule(const char* moduleType) const
{
	ListIterator<Module> moduleIterator(masterModules_);
	while (Module* module = moduleIterator.iterate())
	{
		if (DissolveSys::sameString(module->type(), moduleType)) return module;
	}

	return NULL;
}

// Return master Module instances
const List<Module>& Dissolve::masterModules() const
{
	return masterModules_;
}

// Create a Module instance for the named Module type
Module* Dissolve::createModuleInstance(const char* moduleType)
{
	Module* masterModule = findMasterModule(moduleType);
	if (!masterModule)
	{
		Messenger::error("No Module type '%s' exists.\n", moduleType);
		return NULL;
	}

	// Creat a new instance of the specified Module and add it to our list
	Module* instance = masterModule->createInstance();
	moduleInstances_.own(instance);

	return instance;
}

// Search for any instance of any module with the specified unique name
Module* Dissolve::findModuleInstance(const char* uniqueName)
{
	for (Module* module = moduleInstances_.first(); module != NULL; module = module->next) if (DissolveSys::sameString(module->uniqueName(), uniqueName)) return module;

	return NULL;
}
