// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "modules/registry.h"
#include "modules/accumulate/accumulate.h"
#include "modules/angle/angle.h"
#include "modules/atomShake/atomShake.h"
#include "modules/avgMol/avgMol.h"
#include "modules/axisAngle/axisAngle.h"
#include "modules/benchmark/benchmark.h"
#include "modules/bragg/bragg.h"
#include "modules/compare/compare.h"
#include "modules/dAngle/dAngle.h"
#include "modules/drivenMD/drivenMD.h"
#include "modules/energy/energy.h"
#include "modules/epsr/epsr.h"
#include "modules/epsrManager/epsrManager.h"
#include "modules/exportCoordinates/exportCoordinates.h"
#include "modules/exportPairPotentials/exportPairPotentials.h"
#include "modules/exportTrajectory/exportTrajectory.h"
#include "modules/forces/forces.h"
#include "modules/geomOpt/geomOpt.h"
#include "modules/gr/gr.h"
#include "modules/histogramCN/histogramCN.h"
#include "modules/importTrajectory/importTrajectory.h"
#include "modules/intraAngle/intraAngle.h"
#include "modules/intraDistance/intraDistance.h"
#include "modules/intraShake/intraShake.h"
#include "modules/md/md.h"
#include "modules/modifierOSites/modifierOSites.h"
#include "modules/molShake/molShake.h"
#include "modules/neutronSQ/neutronSQ.h"
#include "modules/orientedSDF/orientedSDF.h"
#include "modules/qSpecies/qSpecies.h"
#include "modules/sdf/sdf.h"
#include "modules/siteRDF/siteRDF.h"
#include "modules/sq/sq.h"
#include "modules/temperatureSchedule/temperatureSchedule.h"
#include "modules/test/test.h"
#include "modules/voxelDensity/voxelDensity.h"
#include "modules/xRaySQ/xRaySQ.h"

ModuleRegistry::ModuleRegistry()
{
    registerProducer<AccumulateModule>(ModuleTypes::Accumulate, "Accumulate partials data to form an average",
                                       "Correlation Functions");
    registerProducer<AngleModule>(ModuleTypes::Angle, "Calculate distance/angle maps", "Analysis");
    registerProducer<AtomShakeModule>(ModuleTypes::AtomShake, "Perform atomic Monte Carlo on all atoms", "Evolution");
    registerProducer<AvgMolModule>(ModuleTypes::AvgMol,
                                   "Calculate average atomic positions of a species around an oriented site", "Analysis");
    registerProducer<AxisAngleModule>(ModuleTypes::AxisAngle, "Calculate distance/angle map between site axes", "Analysis");
    registerProducer<BenchmarkModule>(ModuleTypes::Benchmark, "Perform benchmarking on a variety of functions",
                                      "Checks & Tests");
    registerProducer<BraggModule>(ModuleTypes::Bragg, "Calculate Bragg intensities", "Correlation Functions");
    registerProducer<CompareModule>(ModuleTypes::Compare, "Compare data sets and calculate errors", "Checks & Tests");
    registerProducer<DAngleModule>(ModuleTypes::DAngle, "Calculate distance/angle maps", "Analysis");
    registerProducer<DrivenMDModule>(ModuleTypes::DrivenMD, "SAXS/SANS driven molecular dynamics test", "Forcefield");
    registerProducer<EnergyModule>(ModuleTypes::Energy, "Calculate the total energy of a Configuration", "Forcefield");
    registerProducer<EPSRModule>(ModuleTypes::EPSR, "Refine interatomic potentials in a manner consistent with EPSR",
                                 "Forcefield");
    registerProducer<EPSRManagerModule>(ModuleTypes::EPSRManager, "Manage EPSR modules with more control", "Forcefield");
    registerProducer<ExportCoordinatesModule>(ModuleTypes::ExportCoordinates,
                                              "Export coordinates for one or more Configurations", "Export");
    registerProducer<ExportPairPotentialsModule>(ModuleTypes::ExportPairPotentials, "Export pair potentials", "Export");
    registerProducer<ExportTrajectoryModule>(ModuleTypes::ExportTrajectory, "Export trajectory for a Configuration", "Export");
    registerProducer<ForcesModule>(ModuleTypes::Forces, "Calculate the total atomic forces within a Configuration",
                                   "Forcefield");
    registerProducer<GeometryOptimisationModule>(ModuleTypes::GeometryOptimisation,
                                                 "Optimise geometry with respect to energy (minimise)", "Optimisation");
    registerProducer<GRModule>(ModuleTypes::GR, "Calculate partial and total g(r)", "Correlation Functions");
    registerProducer<HistogramCNModule>(ModuleTypes::HistogramCN, "Produce a histogram of Coordination numbers between sites",
                                        "Analysis");
    registerProducer<ImportTrajectoryModule>(ModuleTypes::ImportTrajectory,
                                             "Calculate coordination numbers from an existing radial distribution function",
                                             "Import");
    registerProducer<IntraAngleModule>(ModuleTypes::IntraAngle, "Calculate intramolecular angle between two sites", "Analysis");
    registerProducer<IntraDistanceModule>(ModuleTypes::IntraDistance, "Calculate intramolecular histogram between two sites",
                                          "Analysis");
    registerProducer<IntraShakeModule>(ModuleTypes::IntraShake,
                                       "Perform Monte Carlo shakes on intramolecular terms within molecules", "Evolution");
    registerProducer<MDModule>(ModuleTypes::MD, "Evolve a Configuration using molecular dynamics", "Evolution");
    registerProducer<MolShakeModule>(ModuleTypes::MolShake, "Perform molecular Monte Carlo moves", "Evolution");
    registerProducer<NeutronSQModule>(ModuleTypes::NeutronSQ, "Calculate neutron-weighted S(Q)", "Correlation Functions");
    registerProducer<OrientedSDFModule>(
        ModuleTypes::OrientedSDF,
        "Calculate spatial density functions around oriented sites, restricted by relative molecule orientation", "Analysis");
    registerProducer<ModifierOSitesModule>(ModuleTypes::ModifierOSites,
                                           "Calculate the percentage FO, BO and NBO bonded to a modifier atom", "Analysis");
    registerProducer<QSpeciesModule>(ModuleTypes::QSpecies, "Calculate QSpecies of a network former", "Analysis");
    registerProducer<SDFModule>(ModuleTypes::SDF, "Calculate spatial density functions around oriented sites", "Analysis");
    registerProducer<SiteRDFModule>(ModuleTypes::SiteRDF, "Calculate radial distribution functions between sites", "Analysis");
    registerProducer<SQModule>(ModuleTypes::SQ, "Transform g(r) into unweighted S(Q)", "Correlation Functions");
    registerProducer<TemperatureScheduleModule>(ModuleTypes::TemperatureSchedule,
                                                "Adjust the temperature of a configuration during a simulation", "Evolution");
    registerProducer<TestModule>(ModuleTypes::Test, "Development Module");
    registerProducer<VoxelDensityModule>(
        ModuleTypes::VoxelDensity,
        "Describe distribution of atomic number, mass, and scattering length density across unit cell voxels", "Analysis");
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
