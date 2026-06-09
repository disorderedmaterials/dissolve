// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "modules/registry.h"
#include "modules/accumulate/accumulate.h"
#include "modules/bragg/bragg.h"
#include "modules/clustering/clustering.h"
#include "modules/compare/compare.h"
#include "modules/epsrManager/epsrManager.h"
#include "modules/exportPairPotentials/exportPairPotentials.h"
#include "modules/gr/gr.h"
#include "modules/importTrajectory/importTrajectory.h"
#include "modules/molShake/molShake.h"
#include "modules/neutronSQ/neutronSQ.h"
#include "modules/sq/sq.h"
#include "modules/temperatureSchedule/temperatureSchedule.h"
#include "modules/test/test.h"
#include "modules/tr/tr.h"
#include "modules/xRaySQ/xRaySQ.h"

ModuleRegistry::ModuleRegistry()
{
    registerProducer<AccumulateModule>(ModuleTypes::Accumulate, "Accumulate partials data to form an average",
                                       "Correlation Functions");
    registerProducer<BraggModule>(ModuleTypes::Bragg, "Calculate Bragg intensities", "Correlation Functions");
    registerProducer<ClusteringModule>(ModuleTypes::Clustering, "Analyse clustering between sites", "Analysis");
    registerProducer<CompareModule>(ModuleTypes::Compare, "Compare data sets and calculate errors", "Checks & Tests");
    registerProducer<EPSRManagerModule>(ModuleTypes::EPSRManager, "Manage EPSR modules with more control", "Forcefield");
    registerProducer<ExportPairPotentialsModule>(ModuleTypes::ExportPairPotentials, "Export pair potentials", "Export");
    registerProducer<GRModule>(ModuleTypes::GR, "Calculate partial and total g(r)", "Correlation Functions");
    registerProducer<ImportTrajectoryModule>(ModuleTypes::ImportTrajectory,
                                             "Calculate coordination numbers from an existing radial distribution function",
                                             "Import");
    registerProducer<MolShakeModule>(ModuleTypes::MolShake, "Perform molecular Monte Carlo moves", "Evolution");
    registerProducer<NeutronSQModule>(ModuleTypes::NeutronSQ, "Calculate neutron-weighted S(Q)", "Correlation Functions");
    registerProducer<SQModule>(ModuleTypes::SQ, "Transform g(r) into unweighted S(Q)", "Correlation Functions");
    registerProducer<TemperatureScheduleModule>(ModuleTypes::TemperatureSchedule,
                                                "Adjust the temperature of a configuration during a simulation", "Evolution");
    registerProducer<TestModule>(ModuleTypes::Test, "Development Module");
    registerProducer<TRModule>(ModuleTypes::TR, "Calculate the partial and total t(r)", "Correlation Functions");
    registerProducer<XRaySQModule>(ModuleTypes::XRaySQ, "Calculate x-ray-weighted S(Q)", "Correlation Functions");
}

/*
 * Producers
 */

// Produce module of specified type
Module *ModuleRegistry::produce(ModuleTypes::ModuleType moduleType) const
{
    auto it = producers_.find(moduleType);
    if (it == producers_.end())
        return {};

    return (it->second.first)();
}

// Return categorised map of modules
const std::map<std::string, std::vector<ModuleRegistry::ModuleInfoData>> &ModuleRegistry::categories() const
{
    return categories_;
}

/*
 * Instance
 */

// Return the producer instance
const ModuleRegistry &ModuleRegistry::instance()
{
    static ModuleRegistry instance;

    return instance;
}

/*
 * Module Management
 */

// Return category map
const std::map<std::string, std::vector<ModuleRegistry::ModuleInfoData>> &ModuleRegistry::categoryMap()
{
    return instance().categories();
}

// Create a Module instance for the named Module type
std::unique_ptr<Module> ModuleRegistry::create(CoreData &coreData, ModuleTypes::ModuleType type)
{
    auto m = std::unique_ptr<Module>(instance().produce(type));

    if (m)
        m->setName(DissolveSys::uniqueName(ModuleTypes::moduleType(m->type()), coreData.moduleInstances(),
                                           [&](const auto &inst) { return inst == m.get() ? "" : inst->name(); }));
    return m;
}

// Create a Module instance for the specified Module type, and add it to the specified layer
Module *ModuleRegistry::create(CoreData &coreData, ModuleTypes::ModuleType moduleType, ModuleLayer *destinationLayer)
{
    auto instance = create(coreData, moduleType);
    if (!instance)
        return nullptr;

    // Add the new module instance to the specified destination layer
    auto *m = instance.get();
    destinationLayer->modules().emplace_back(std::move(instance));

    return m;
}
