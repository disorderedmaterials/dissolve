// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "modules/registry.h"
#include "modules/accumulate/accumulate.h"
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
#include "modules/import_trajectory/importtraj.h"
#include "modules/intrashake/intrashake.h"
#include "modules/md/md.h"
#include "modules/molshake/molshake.h"
#include "modules/neutronsq/neutronsq.h"
#include "modules/rdf/rdf.h"
#include "modules/sq/sq.h"
#include "modules/test/test.h"
#include "modules/xraysq/xraysq.h"

ModuleRegistry::ModuleRegistry()
{
    registerProducer<AccumulateModule>("Accumulate", "Accumulate partials data to form an average", "Correlation Functions");
    registerProducer<AnalyseModule>("Analyse", "Perform custom analysis of one or more Configurations", "Analysis");
    registerProducer<AtomShakeModule>("AtomShake", "Perform atomic Monte Carlo on all atoms", "Evolution");
    registerProducer<BenchmarkModule>("Benchmark", "Perform benchmarking on a variety of functions", "Checks & Tests");
    registerProducer<BraggModule>("Bragg", "Calculate Bragg intensities", "Correlation Functions");
    registerProducer<CalculateAngleModule>("CalculateAngle", "Calculate distance/angle maps", "Analysis");
    registerProducer<CalculateAvgMolModule>(
        "CalculateAvgMol", "Calculate average atomic positions of a species around an oriented site", "Analysis");
    registerProducer<CalculateAxisAngleModule>("CalculateAxisAngle", "Calculate distance/angle map between site axes",
                                               "Analysis");
    registerProducer<CalculateCNModule>(
        "CalculateCN", "Calculate coordination numbers from an existing radial distribution function", "Analysis");
    registerProducer<CalculateDAngleModule>("CalculateDAngle", "Calculate distance/angle maps", "Analysis");
    registerProducer<CalculateRDFModule>("CalculateRDF", "Calculate radial distribution functions between sites", "Analysis");
    registerProducer<CalculateSDFModule>("CalculateSDF", "Calculate spatial density functions around oriented sites",
                                         "Analysis");
    registerProducer<ChecksModule>("Checks", "Conduct simple checks on Configurations", "Checks & Tests");
    registerProducer<CheckSpeciesModule>("CheckSpecies", "Check the contents of a Species against reference values",
                                         "Checks & Tests");
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
    registerProducer<ImportTrajectoryModule>(
        "ImportTrajectory", "Calculate coordination numbers from an existing radial distribution function", "Import");
    registerProducer<IntraShakeModule>("IntraShake", "Perform Monte Carlo shakes on intramolecular terms within molecules",
                                       "Evolution");
    registerProducer<MDModule>("MD", "Evolve a Configuration using molecular dynamics", "Evolution");
    registerProducer<MolShakeModule>("MolShake", "Perform molecular Monte Carlo moves", "Evolution");
    registerProducer<NeutronSQModule>("NeutronSQ", "Calculate neutron-weighted S(Q)", "Correlation Functions");
    registerProducer<RDFModule>("RDF", "Calculate partial and total g(r)", "Correlation Functions");
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
        throw(std::runtime_error(fmt::format(
            "A producer has not been registered for module type '{}', so a new instance cannot be created.\n", moduleType)));

    return (it->second.first)();
}

// Return categorised map of modules
const std::vector<std::pair<std::string, std::vector<ModuleRegistry::ModuleInfoData>>> &ModuleRegistry::categories() const
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
 * Static Functions
 */

// Return category map
const std::vector<std::pair<std::string, std::vector<ModuleRegistry::ModuleInfoData>>> &ModuleRegistry::categoryMap()
{
    return instance().categories();
}

// Create new item via template
std::unique_ptr<Module> ModuleRegistry::create(std::string_view moduleType)
{
    return std::unique_ptr<Module>(instance().produce(std::string(moduleType)));
}
