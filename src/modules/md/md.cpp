// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "modules/md/md.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/integer.h"
#include "keywords/optionaldouble.h"
#include "keywords/speciesvector.h"

MDModule::MDModule() : Module("MD")
{
    // Targets
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    // Control
    keywords_.add<OptionalDoubleKeyword>(
        "Control", "CutoffDistance",
        "Interatomic cutoff distance to use for energy calculation (0.0 to use pair potential range)", cutoffDistance_, 0.0,
        std::nullopt, 0.1, "Use PairPotential Range");
    keywords_.add<IntegerKeyword>("Control", "NSteps", "Number of MD steps to perform", nSteps_, 1);
    keywords_.add<BoolKeyword>("Control", "CapForces", "Control whether atomic forces are capped every step", capForces_);
    keywords_.add<DoubleKeyword>("Control", "CapForcesAt", "Set cap on allowable force (kJ/mol) per atom", capForcesAt_, 0.0);
    keywords_.add<DoubleKeyword>("Control", "DeltaT", "Timestep (ps) to use in MD simulation", deltaT_, 0.0);
    keywords_.add<BoolKeyword>("Control", "OnlyWhenEnergyStable", "Only run MD when target Configuration energies are stable",
                               onlyWhenEnergyStable_);
    keywords_.add<BoolKeyword>("Control", "VariableTimestep",
                               "Whether a variable timestep should be used, determined from the maximal force vector",
                               variableTimestep_);
    keywords_.add<BoolKeyword>("Control", "RandomVelocities",
                               "Whether random velocities should always be assigned before beginning MD simulation",
                               randomVelocities_);
    keywords_.add<SpeciesVectorKeyword>("Control", "RestrictToSpecies", "Restrict the calculation to the specified Species",
                                        restrictToSpecies_);

    // Output
    keywords_.add<IntegerKeyword>("Output", "EnergyFrequency",
                                  "Frequency at which to calculate total system energy (or 0 to inhibit)", energyFrequency_, 0);
    keywords_.add<IntegerKeyword>("Output", "OutputFrequency",
                                  "Frequency at which to output step information (or 0 to inhibit)", outputFrequency_, 0);
    keywords_.add<IntegerKeyword>("Output", "TrajectoryFrequency", "Write frequency for trajectory file (or 0 to inhibit)",
                                  trajectoryFrequency_, 0);
}
