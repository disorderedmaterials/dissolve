// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/dlPoly/dlPoly.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/integer.h"
#include "keywords/optionalDouble.h"
#include "keywords/optionalInt.h"
#include "keywords/enumOptions.h"
#include "keywords/speciesVector.h"
#include "keywords/fileAndFormat.h"
#include "keywords/stdString.h"

DlPolyModule::DlPolyModule() : Module(ModuleTypes::DlPoly)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);
    keywords_.add<StringKeyword>("DlPolyPath", "Filepath of ./DLPOLY.Z installation", dlPolyPath_);
    
    keywords_.setOrganisation("Options", "Simulation");
    keywords_.add<IntegerKeyword>("NSteps", "Set duration of simulation (inc. equilibration)", nSteps_, 1);
    keywords_.add<BoolKeyword>("VariableTimestep", "Enable variable timestep", timestepVariable_);
    keywords_.add<DoubleKeyword>("DeltaT", "Set calculation timestep or initial timestep for variable timestep calculations", fixedTimestep_, 0.0);
    keywords_.add<BoolKeyword>("RandomVelocities",
                               "Whether random velocities should always be assigned before beginning DlPoly simulation",
                               randomVelocities_);

    keywords_.setOrganisation("Options", "Output");
    keywords_.add<OptionalIntegerKeyword>("EnergyFrequency", "Set frequency of printing results to output",
                                          energyFrequency_, 0, std::nullopt, 5, "Off");
    keywords_.add<OptionalIntegerKeyword>("OutputFrequency", "Set frequency of stats sampling to statis file", outputFrequency_,
                                          0, std::nullopt, 5, "Off");
    keywords_.add<OptionalIntegerKeyword>("TrajectoryFrequency", "Interval between dumping trajectory configurations", trajectoryFrequency_, 0, std::nullopt, 5, "Off");
    keywords_.add<EnumOptionsKeyword<DlPolyModule::TrajectoryKey>>(
        "TrajectoryKey", "Set trajectory output, options: pos, pos-vel, pos-vel-force, compressed", trajectoryKey_, DlPolyModule::trajectoryKey());
    keywords_.add<EnumOptionsKeyword<DlPolyModule::CoulMethod>>(
        "CoulMethod", "Set method for electrostatics method, options: off, spme, dddp, pairwise, reaction_field, force_shifted", coulMethod_, DlPolyModule::coulMethod());
    keywords_.add<DoubleKeyword>("Coul Precision", "Calculate electrostatics using Fennell damping (Ewald-like) with given precision", coulPrecision_, 0.0);

    keywords_.setOrganisation("Advanced");
    keywords_.add<BoolKeyword>("CapForces", "Control whether atomic forces are capped every step", capForces_);
    keywords_.add<DoubleKeyword>("CapForcesAt", "Set force cap clamping maximum force during equilibration", capForcesAt_, 0.0);
    keywords_.add<OptionalDoubleKeyword>(
        "CutoffDistance", "Set the global cutoff for real-speace potentials",
        cutoffDistance_, 0.0, std::nullopt, 1.0, "Use PairPotential Range");
    keywords_.add<DoubleKeyword>("padding", "Set padding for sizing of Verlet neighbour lists", padding_, 0.0);
    keywords_.add<EnumOptionsKeyword<DlPolyModule::Ensemble>>(
        "Ensemble", "Set ensemble constraints, options: NVE, PMF, NVT, NPT, NST", ensemble_, DlPolyModule::ensemble());
    keywords_.add<EnumOptionsKeyword<DlPolyModule::EnsembleMethod>>(
        "EnsembleMethod", "Set ensemble method, options NVT: Evans, Langevin, Andersen, Berendsen, Hoover, gentle, ttm, dpds1, dpds2. NP|ST: Langevin, Berendsen, Hoover, MTK.", ensembleMethod_, DlPolyModule::ensembleMethod());
    keywords_.add<DoubleKeyword>("EnsembleThermostatCoupling", "Set thermostat relaxation/decorrelation times (use ensemble_thermostat_friction for langevin)", ensembleThermostatCoupling_, 0.0);
    keywords_.add<EnumOptionsKeyword<DlPolyModule::MinimisationCriterion>>(
        "MinimisationCriterion", "Set minimisation criterion, options: off, force, energy, distance", minimisationCriterion_, DlPolyModule::minimisationCriterion());
    keywords_.add<DoubleKeyword>("minimisationTolerance", "Set minimisation tolerance, units: determined by criterion", minimisationTolerance_, 0.0);
    keywords_.add<DoubleKeyword>("minimisationFrequency", "Set minimisation frequency", minimisationFrequency_, 0.0);     
}