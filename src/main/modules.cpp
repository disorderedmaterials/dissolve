/*
	*** Dissolve - Modules
	*** src/main/modules.cpp
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

#include "main/dissolve.h"
#include "module/registrar.h"
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

// Register master instances for all Modules
void Dissolve::registerModules()
{
	ModuleRegistrar<AnalyseModule> analyseRegistrar(moduleList_, moduleInstances_);
	ModuleRegistrar<AtomShakeModule> atomShakeRegistrar(moduleList_, moduleInstances_);
	ModuleRegistrar<CalculateRDFModule> calculateRDFRegistrar(moduleList_, moduleInstances_);
	ModuleRegistrar<CalibrationModule> calibrationRegistrar(moduleList_, moduleInstances_);
	ModuleRegistrar<ChecksModule> checksRegistrar(moduleList_, moduleInstances_);
	ModuleRegistrar<DataTestModule> dataTestRegistrar(moduleList_, moduleInstances_);
	ModuleRegistrar<EnergyModule> energyRegistrar(moduleList_, moduleInstances_);
	ModuleRegistrar<EPSRModule> epsrRegistrar(moduleList_, moduleInstances_);
	ModuleRegistrar<ExportModule> exportRegistrar(moduleList_, moduleInstances_);
	ModuleRegistrar<ForcesModule> forcesRegistrar(moduleList_, moduleInstances_);
	ModuleRegistrar<ImportModule> importRegistrar(moduleList_, moduleInstances_);
	ModuleRegistrar<IntraShakeModule> intraShakeRegistrar(moduleList_, moduleInstances_);
	ModuleRegistrar<MDModule> mdRegistrar(moduleList_, moduleInstances_);
	ModuleRegistrar<MolShakeModule> molShakeRegistrar(moduleList_, moduleInstances_);
	ModuleRegistrar<RDFModule> rdfRegistrar(moduleList_, moduleInstances_);
	ModuleRegistrar<RefineModule> refineRegistrar(moduleList_, moduleInstances_);
	ModuleRegistrar<SanityCheckModule> sanityCheckRegistrar(moduleList_, moduleInstances_);
	ModuleRegistrar<NeutronSQModule> neutronSQRegistrar(moduleList_, moduleInstances_);
	ModuleRegistrar<TestModule> testRegistrar(moduleList_, moduleInstances_);
}

// Clear master (and all other) instances for all Modules
void Dissolve::clearModules()
{
	RefListIterator< List<Module>, bool> listIterator(moduleInstances_);
	while (List<Module>* list = listIterator.iterate()) list->clear();
	moduleInstances_.clear();

	// Need to clear the accompanying reference lists in our ModuleList
	moduleList_.clearMasterInstances();
}

// Search for and return the master reference for the named Module
Module* Dissolve::findMasterModule(const char* moduleName)
{
	return moduleList_.findMasterInstance(moduleName);
}

// Return master Module instances
const List<ModuleReference>& Dissolve::masterModules() const
{
	return moduleList_.masterInstances();
}

// Return number of failed Module registrations
int Dissolve::nFailedModuleRegistrations() const
{
	return moduleList_.nFailedRegistrations();
}

// Print out registered module information, and return false if any registration errors were encountered
void Dissolve::printMasterModuleInformation() const
{
	moduleList_.printMasterModuleInformation();
}

// Search for any instance of any module with the specified unique name
Module* Dissolve::findModule(const char* uniqueName)
{
	moduleList_.findInstanceByUniqueName(uniqueName);
}
