// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "modules/md/md.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/integer.h"
#include "keywords/optionaldouble.h"
#include "keywords/speciesvector.h"

MDModule::MDModule() : Module("MD")
{
    keywords_.addTarget<ConfigurationKeyword>("Configuration", "Set target configuration for the module", targetConfiguration_);

    keywords_.setOrganisation("Options", "Simulation");
    keywords_.add<IntegerKeyword>("NSteps", "Number of MD steps to perform", nSteps_, 1);
    keywords_.add<EnumOptionsKeyword<MDModule::TimestepType>>("Timestep", "Timestep type to use in calculation", timestepType_,
                                                              MDModule::timestepType());
    keywords_.add<DoubleKeyword>("DeltaT", "Fixed timestep (ps) to use in MD simulation", fixedTimestep_, 0.0);
    keywords_.add<BoolKeyword>("RandomVelocities",
                               "Whether random velocities should always be assigned before beginning MD simulation",
                               randomVelocities_);

    keywords_.setOrganisation("Options", "Control");
    keywords_
        .add<SpeciesVectorKeyword>("RestrictToSpecies", "Restrict the calculation to the specified Species", restrictToSpecies_)
        ->setEditSignals({KeywordBase::KeywordSignal::ClearModuleData});
    keywords_.add<BoolKeyword>("OnlyWhenEnergyStable", "Only run MD when target Configuration energies are stable",
                               onlyWhenEnergyStable_);

    keywords_.setOrganisation("Options", "Output");
    keywords_.add<IntegerKeyword>("EnergyFrequency", "Frequency at which to calculate total system energy (or 0 to inhibit)",
                                  energyFrequency_, 0);
    keywords_.add<IntegerKeyword>("OutputFrequency", "Frequency at which to output step information (or 0 to inhibit)",
                                  outputFrequency_, 0);
    keywords_.add<IntegerKeyword>("TrajectoryFrequency", "Write frequency for trajectory file (or 0 to inhibit)",
                                  trajectoryFrequency_, 0);

    keywords_.setOrganisation("Advanced");
    keywords_.add<BoolKeyword>("CapForces", "Control whether atomic forces are capped every step", capForces_);
    keywords_.add<DoubleKeyword>("CapForcesAt", "Set cap on allowable force (kJ/mol) per atom", capForcesAt_, 0.0);
    keywords_.add<OptionalDoubleKeyword>(
        "CutoffDistance", "Interatomic cutoff distance to use for energy calculation (0.0 to use pair potential range)",
        cutoffDistance_, 0.0, std::nullopt, 0.1, "Use PairPotential Range");
    keywords_.add<BoolKeyword>(
        "IntraOnly",
        "Only forces arising from intramolecular terms (including pair potential contributions) will be calculated",
        intramolecularForcesOnly_);

    // Deprecated
    static bool deprecatedBool_{false};
    keywords_.addDeprecated<BoolKeyword>("VariableTimestep",
                                         "Whether a variable timestep should be used, determined from the maximal force vector",
                                         deprecatedBool_);
}

// Return enum options for TimestepType
EnumOptions<MDModule::TimestepType> MDModule::timestepType()
{
    return EnumOptions<MDModule::TimestepType>(
        "TimestepType",
        {{TimestepType::Fixed, "Fixed"}, {TimestepType::Variable, "Variable"}, {TimestepType::Automatic, "Auto"}});
}
