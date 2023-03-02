// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/registry.h"
#include "modules/accumulate/accumulate.h"
#include "modules/analyse/analyse.h"
#include "modules/angle/angle.h"
#include "modules/atomshake/atomshake.h"
#include "modules/avgmol/avgmol.h"
#include "modules/axisangle/axisangle.h"
#include "modules/benchmark/benchmark.h"
#include "modules/bragg/bragg.h"
#include "modules/checks/checks.h"
#include "modules/checkspecies/checkspecies.h"
#include "modules/dangle/dangle.h"
#include "modules/datatest/datatest.h"
#include "modules/energy/energy.h"
#include "modules/epsr/epsr.h"
#include "modules/export_coordinates/exportcoords.h"
#include "modules/export_pairpotentials/exportpp.h"
#include "modules/export_trajectory/exporttraj.h"
#include "modules/forces/forces.h"
#include "modules/geomopt/geomopt.h"
#include "modules/gr/gr.h"
#include "modules/import_trajectory/importtraj.h"
#include "modules/intrashake/intrashake.h"
#include "modules/md/md.h"
#include "modules/molshake/molshake.h"
#include "modules/neutronsq/neutronsq.h"
#include "modules/sdf/sdf.h"
#include "modules/siterdf/siterdf.h"
#include "modules/sq/sq.h"
#include "modules/test/test.h"
#include "modules/xraysq/xraysq.h"

ModuleRegistry::ModuleRegistry()
{
    registerProducer<AccumulateModule>("Accumulate", "Accumulate partials data to form an average", "Correlation Functions");
    registerProducer<AnalyseModule>("Analyse", "Perform custom analysis of one or more Configurations", "Analysis");
    registerProducer<AngleModule>("Angle", "Calculate distance/angle maps", "Analysis");
    registerProducer<AtomShakeModule>("AtomShake", "Perform atomic Monte Carlo on all atoms", "Evolution");
    registerProducer<AvgMolModule>("AvgMol", "Calculate average atomic positions of a species around an oriented site",
                                   "Analysis");
    registerProducer<AxisAngleModule>("AxisAngle", "Calculate distance/angle map between site axes", "Analysis");
    registerProducer<BenchmarkModule>("Benchmark", "Perform benchmarking on a variety of functions", "Checks & Tests");
    registerProducer<BraggModule>("Bragg", "Calculate Bragg intensities", "Correlation Functions");
    registerProducer<ChecksModule>("Checks", "Conduct simple checks on Configurations", "Checks & Tests");
    registerProducer<CheckSpeciesModule>("CheckSpecies", "Check the contents of a Species against reference values",
                                         "Checks & Tests");
    registerProducer<DAngleModule>("DAngle", "Calculate distance/angle maps", "Analysis");
    registerProducer<DataTestModule>("DataTest", "Test named data in other modules against reference data", "Checks & Tests");
    registerProducer<EnergyModule>("Energy", "Calculate the total energy of a Configuration", "Forcefield");
    registerProducer<EPSRModule>("EPSR", "Refine interatomic potentials in a manner consistent with EPSR", "Forcefield");
    registerProducer<ExportCoordinatesModule>("ExportCoordinates", "Export coordinates for one or more Configurations",
                                              "Export");
    registerProducer<ExportPairPotentialsModule>("ExportPairPotentials", "Export pair potentials", "Export");
    registerProducer<ExportTrajectoryModule>("ExportTrajectory", "Export trajectory for a Configuration", "Export");
    registerProducer<ForcesModule>("Forces", "Calculate the total atomic forces within a Configuration", "Forcefield");
    registerProducer<GeometryOptimisationModule>("GeometryOptimisation", "Optimise geometry with respect to energy (minimise)",
                                                 "Optimisation");
    registerProducer<GRModule>("GR", "Calculate partial and total g(r)", "Correlation Functions");
    registerProducer<ImportTrajectoryModule>(
        "ImportTrajectory", "Calculate coordination numbers from an existing radial distribution function", "Import");
    registerProducer<IntraShakeModule>("IntraShake", "Perform Monte Carlo shakes on intramolecular terms within molecules",
                                       "Evolution");
    registerProducer<MDModule>("MD", "Evolve a Configuration using molecular dynamics", "Evolution");
    registerProducer<MolShakeModule>("MolShake", "Perform molecular Monte Carlo moves", "Evolution");
    registerProducer<NeutronSQModule>("NeutronSQ", "Calculate neutron-weighted S(Q)", "Correlation Functions");
    registerProducer<SDFModule>("SDF", "Calculate spatial density functions around oriented sites", "Analysis");
    registerProducer<SiteRDFModule>("SiteRDF", "Calculate radial distribution functions between sites", "Analysis");
    registerProducer<SQModule>("SQ", "Transform g(r) into unweighted S(Q)", "Correlation Functions");
    registerProducer<TestModule>("Test", "Development Module");
    registerProducer<XRaySQModule>("XRaySQ", "Calculate x-ray-weighted S(Q)", "Correlation Functions");
}

/*
 * Producers
 */

// Produce module of specified type
Module *ModuleRegistry::produce(std::string moduleType) const
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
std::unique_ptr<Module> ModuleRegistry::create(std::string_view moduleType)
{
    auto m = std::unique_ptr<Module>(instance().produce(std::string(moduleType)));

    if (m)
        m->setName(DissolveSys::uniqueName(m->type(), Module::instances(),
                                           [&](const auto &inst) { return inst == m.get() ? std::string() : inst->name(); }));
    return m;
}

// Create a Module instance for the named Module type, and add it to the specified layer
Module *ModuleRegistry::create(std::string_view moduleType, ModuleLayer *destinationLayer)
{
    auto instance = create(moduleType);
    if (!instance)
        return nullptr;

    // Add the new module instance to the specified destination layer
    auto *m = instance.get();
    destinationLayer->modules().emplace_back(std::move(instance));

    return m;
}
