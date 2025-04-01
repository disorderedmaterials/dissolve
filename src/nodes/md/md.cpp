// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/md.h"
#include "base/lineParser.h"
#include "base/randomBuffer.h"
#include "base/timer.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "data/atomicMasses.h"
#include "keywords/bool.h"
#include "keywords/configuration.h"
#include "keywords/double.h"
#include "keywords/integer.h"
#include "keywords/optionalDouble.h"
#include "keywords/optionalInt.h"
#include "keywords/speciesVector.h"
#include "main/dissolve.h"
#include "module/context.h"
#include "modules/energy/energy.h"
#include "modules/forces/forces.h"

MDNode::MDNode()
{
    addInput<Configuration *>("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setFlags({ParameterBase::Required, ParameterBase::Invalidates});

    addInput<Number>("NSteps", "Number of MD steps to perform", nSteps, 1);
    addOption<EnumOptions<MDNode::TimestepType>>("Timestep", "Timestep type to use in calculation", timestepType_,
                                                 MDNode::timestepType());
    addOption<Number>("DeltaT", "Fixed timestep (ps) to use in MD simulation", fixedTimestep_, 0.0);
    addOption<bool>("RandomVelocities", "Whether random velocities should always be assigned before beginning MD simulation",
                    randomVelocities_);

    addOption<std::vector<const Species *>>("RestrictToSpecies", "Restrict the calculation to the specified Species",
                                            restrictToSpecies_);
    addOption<bool>("OnlyWhenEnergyStable", "Only run MD when target Configuration energies are stable", onlyWhenEnergyStable_);

    addOption<std::optional<Number>>("EnergyFrequency", "Frequency at which to calculate total system energy", energyFrequency_,
                                     0, std::nullopt, 5, "Off");
    addOption<std::optional<Number>>("OutputFrequency", "Frequency at which to output step information", outputFrequency_, 0,
                                     std::nullopt, 5, "Off");
    addOption<std::optional<Number>>("TrajectoryFrequency", "Write frequency for trajectory file", trajectoryFrequency_, 0,
                                     std::nullopt, 5, "Off");

    addOption<bool>("CapForces", "Control whether atomic forces are capped every step", capForces_);
    addOption<Number>("CapForcesAt", "Set cap on allowable force (kJ/mol) per atom", capForcesAt_, 0.0);
    addOption<bool>("IntraOnly",
                    "Only forces arising from intramolecular terms (including pair potential contributions) will be calculated",
                    intramolecularForcesOnly_);
}

std::string_view MDNode::type() const { return "MD"; }

std::string_view MDNode::summary() const { return "Run a short molecular dynamics simulation."; }
