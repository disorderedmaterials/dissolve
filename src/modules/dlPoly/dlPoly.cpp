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

DlPolyModule::DlPolyModule() : Module(ModuleTypes::DlPoly)
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);
    
    keywords_.setOrganisation("Options", "File");
    keywords_.add<FileAndFormatKeyword>("CONTROL", "File / format for CONTROL", dlPolyControlFormat_, "EndFormat");
    keywords_.add<FileAndFormatKeyword>("FIELD", "File / format for FIELD", dlPolyFieldFormat_, "EndFormat");

    keywords_.setOrganisation("Options", "Simulation");
    keywords_.add<IntegerKeyword>("NSteps", "Number of DlPoly steps to perform", nSteps_, 1);
    keywords_.add<BoolKeyword>("VariableTimestep", 
                               "Whether a variable timestep should be used, determined from the maximal force vector",
                               timestepVariable_);
    keywords_.add<DoubleKeyword>("DeltaT", "Fixed timestep (ps) to use in DlPoly simulation", fixedTimestep_, 0.0);
    keywords_.add<BoolKeyword>("RandomVelocities",
                               "Whether random velocities should always be assigned before beginning DlPoly simulation",
                               randomVelocities_);

    keywords_.setOrganisation("Options", "Output");
    keywords_.add<OptionalIntegerKeyword>("EnergyFrequency", "Frequency at which to calculate total system energy",
                                          energyFrequency_, 0, std::nullopt, 5, "Off");
    keywords_.add<OptionalIntegerKeyword>("OutputFrequency", "Frequency at which to output step information", outputFrequency_,
                                          0, std::nullopt, 5, "Off");
    keywords_.add<OptionalIntegerKeyword>("TrajectoryFrequency", "Write frequency for trajectory file", trajectoryFrequency_, 0,
                                          std::nullopt, 5, "Off");
    keywords_.add<EnumOptionsKeyword<DlPolyModule::TrajectoryKey>>(
        "TrajectoryKey", "Set trajectory key", trajectoryKey_, DlPolyModule::trajectoryKey());
    keywords_.add<EnumOptionsKeyword<DlPolyModule::CoulMethod>>(
        "CoulMethod", "Set Coul Method", coulMethod_, DlPolyModule::coulMethod());
    keywords_.add<DoubleKeyword>("Coul Precision", "Calculate electrostatics using Fennell damping (Ewald-like) with given precision", coulPrecision_, 0.0);

    keywords_.setOrganisation("Advanced");
    keywords_.add<BoolKeyword>("CapForces", "Control whether atomic forces are capped every step", capForces_);
    keywords_.add<DoubleKeyword>("CapForcesAt", "Set cap on allowable force (kJ/mol) per atom", capForcesAt_, 0.0);
    keywords_.add<OptionalDoubleKeyword>(
        "CutoffDistance", "Interatomic cutoff distance to use for energy calculation (0.0 to use pair potential range)",
        cutoffDistance_, 0.0, std::nullopt, 0.1, "Use PairPotential Range");
    keywords_.add<EnumOptionsKeyword<DlPolyModule::Ensemble>>(
        "Ensemble", " Set ensemble constraints", ensemble_, DlPolyModule::ensemble());
    keywords_.add<DoubleKeyword>("EnsembleThermostatCoupling", "Set thermostat relaxation/decorrelation times (use ensemble_thermostat_friction for langevin)", ensembleThermostatCoupling_, 0.0);
        
}