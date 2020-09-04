/*
    *** Dissolve - Modules
    *** src/main/modules.cpp
    Copyright T. Youngs 2012-2020

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
#include "modules/benchmark/benchmark.h"
#include "modules/bragg/bragg.h"
#include "modules/calculate_angle/angle.h"
#include "modules/calculate_avgmol/avgmol.h"
#include "modules/calculate_axisangle/axisangle.h"
#include "modules/calculate_cn/cn.h"
#include "modules/calculate_dangle/dangle.h"
#include "modules/calculate_rdf/rdf.h"
#include "modules/calculate_sdf/sdf.h"
#include "modules/calibration/calibration.h"
#include "modules/checks/checks.h"
#include "modules/checkspecies/checkspecies.h"
#include "modules/datatest/datatest.h"
#include "modules/energy/energy.h"
#include "modules/epsr/epsr.h"
#include "modules/export_coordinates/exportcoords.h"
#include "modules/export_pairpotentials/exportpp.h"
#include "modules/export_trajectory/exporttraj.h"
#include "modules/forces/forces.h"
#include "modules/geomopt/geomopt.h"
#include "modules/import/import.h"
#include "modules/intrashake/intrashake.h"
#include "modules/md/md.h"
#include "modules/molshake/molshake.h"
#include "modules/neutronsq/neutronsq.h"
#include "modules/rdf/rdf.h"
#include "modules/refine/refine.h"
#include "modules/sanitycheck/sanitycheck.h"
#include "modules/sq/sq.h"
#include "modules/test/test.h"

/*
 * Module Registration
 * ===================
 * All modules to be registered for use in the code must be placed here.
 */

// Register master Module
bool Dissolve::registerMasterModule(Module *masterInstance)
{
    // Do sanity check on name
    ListIterator<Module> moduleIterator(masterModules_);
    while (Module *module = moduleIterator.iterate())
    {
        if (DissolveSys::sameString(module->type(), masterInstance->type()))
        {
            Messenger::error("Two modules cannot have the same name ({}).\n", module->type());
            return false;
        }
    }

    // Set the unique name of the Module
    masterInstance->setUniqueName(fmt::format("{}_MASTER", masterInstance->type()));

    // Own the module into our master instances list
    masterModules_.own(masterInstance);

    return true;
}

// Register master instances for all Modules
bool Dissolve::registerMasterModules()
{
    if (!registerMasterModule(new AnalyseModule))
        return false;
    if (!registerMasterModule(new AtomShakeModule))
        return false;
    if (!registerMasterModule(new BenchmarkModule))
        return false;
    if (!registerMasterModule(new BraggModule))
        return false;
    if (!registerMasterModule(new CalculateAngleModule))
        return false;
    if (!registerMasterModule(new CalculateAvgMolModule))
        return false;
    if (!registerMasterModule(new CalculateAxisAngleModule))
        return false;
    if (!registerMasterModule(new CalculateCNModule))
        return false;
    if (!registerMasterModule(new CalculateDAngleModule))
        return false;
    if (!registerMasterModule(new CalculateRDFModule))
        return false;
    if (!registerMasterModule(new CalculateSDFModule))
        return false;
    if (!registerMasterModule(new CalibrationModule))
        return false;
    if (!registerMasterModule(new ChecksModule))
        return false;
    if (!registerMasterModule(new CheckSpeciesModule))
        return false;
    if (!registerMasterModule(new DataTestModule))
        return false;
    if (!registerMasterModule(new EnergyModule))
        return false;
    if (!registerMasterModule(new EPSRModule))
        return false;
    if (!registerMasterModule(new ExportCoordinatesModule))
        return false;
    if (!registerMasterModule(new ExportPairPotentialsModule))
        return false;
    if (!registerMasterModule(new ExportTrajectoryModule))
        return false;
    if (!registerMasterModule(new ForcesModule))
        return false;
    if (!registerMasterModule(new GeometryOptimisationModule))
        return false;
    if (!registerMasterModule(new ImportModule))
        return false;
    if (!registerMasterModule(new IntraShakeModule))
        return false;
    if (!registerMasterModule(new MDModule))
        return false;
    if (!registerMasterModule(new MolShakeModule))
        return false;
    if (!registerMasterModule(new NeutronSQModule))
        return false;
    if (!registerMasterModule(new RDFModule))
        return false;
    if (!registerMasterModule(new RefineModule))
        return false;
    if (!registerMasterModule(new SanityCheckModule))
        return false;
    if (!registerMasterModule(new SQModule))
        return false;
    if (!registerMasterModule(new TestModule))
        return false;

    Messenger::print("Module Information ({} available):\n", masterModules_.nItems());
    ListIterator<Module> moduleIterator(masterModules_);
    while (Module *module = moduleIterator.iterate())
    {
        Messenger::print(" --> {}\n", module->type());
        Messenger::print("     {}\n", module->brief());
    }

    return true;
}

// Search for master Module of the named type
Module *Dissolve::findMasterModule(std::string_view moduleType) const
{
    ListIterator<Module> moduleIterator(masterModules_);
    while (Module *module = moduleIterator.iterate())
    {
        if (DissolveSys::sameString(module->type(), moduleType))
            return module;
    }

    return nullptr;
}

// Return master Module instances
const List<Module> &Dissolve::masterModules() const { return masterModules_; }

// Create a Module instance for the named Module type
Module *Dissolve::createModuleInstance(std::string_view moduleType)
{
    Module *masterModule = findMasterModule(moduleType);
    if (!masterModule)
    {
        Messenger::error("No Module type '{}' exists.\n", moduleType);
        return nullptr;
    }

    // Find a suitable unique name for the Module
    auto instanceId = 1;
    std::string uniqueName;
    do
    {
        uniqueName = fmt::format("{}{:02d}", moduleType, instanceId++);
    } while (findModuleInstance(uniqueName));

    // Create a new instance of the specified Module and add it to our list
    Module *instance = masterModule->createInstance();
    moduleInstances_.append(instance);
    instance->setUniqueName(uniqueName);

    return instance;
}

// Create a Module instance for the named Module type, and add it to the specified layer
Module *Dissolve::createModuleInstance(std::string_view moduleType, ModuleLayer *destinationLayer, bool configurationLocal)
{
    Module *module = createModuleInstance(moduleType);
    if (!module)
        return nullptr;

    // Add the new module instance to the specified destination layer
    destinationLayer->own(module);

    module->setConfigurationLocal(configurationLocal);

    return module;
}

// Search for any instance of any Module with the specified unique name
Module *Dissolve::findModuleInstance(std::string_view uniqueName)
{
    for (auto *module : moduleInstances_)
        if (DissolveSys::sameString(module->uniqueName(), uniqueName))
            return module;

    return nullptr;
}

// Search for any instance of any Module with the specified Module type
RefList<Module> Dissolve::findModuleInstances(std::string_view moduleType)
{
    RefList<Module> instances;

    for (auto *module : moduleInstances_)
        if (DissolveSys::sameString(module->type(), moduleType))
            instances.append(module);

    return instances;
}

// Generate unique Module name with base name provided
std::string Dissolve::uniqueModuleName(std::string_view name, Module *excludeThis)
{
    std::string_view baseName = name.empty() ? "Unnamed" : name;
    std::string uniqueName{baseName};

    // Find an unused name starting with the baseName provided
    auto suffix = 0;
    while (const auto *existingModule = findModuleInstance(uniqueName))
    {
        if (existingModule == excludeThis)
            break;

        // Increase suffix value and regenerate uniqueName from baseName
        ++suffix;
        uniqueName = fmt::format("{}{:02d}", baseName, suffix);
    }

    return uniqueName;
}

// Delete specified Module instance
bool Dissolve::deleteModuleInstance(Module *instance)
{
    if (!moduleInstances_.contains(instance))
        return Messenger::error("Can't find Module instance to remove.\n");

    // Remove the reference from our list
    moduleInstances_.remove(instance);

    // Delete the actual Module - we assume that it has been removed from any ModuleList
    delete instance;

    return true;
}
