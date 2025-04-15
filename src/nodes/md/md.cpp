// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "nodes/md/md.h"

MDNode::MDNode(Graph *parentGraph) : Node(parentGraph)
{
    addInput<Configuration *>("Configuration", "Set target configuration for the module", targetConfiguration_)
        ->setFlags({ParameterBase::Required, ParameterBase::ClearData});

    addInput<Number>("NSteps", "Number of MD steps to perform", nSteps_);
    addOption<MDNode::TimestepType>("Timestep", "Timestep type to use in calculation", timestepType_);
    addOption<Number>("DeltaT", "Fixed timestep (ps) to use in MD simulation", fixedTimestep_);
    addOption<bool>("RandomVelocities", "Whether random velocities should always be assigned before beginning MD simulation",
                    randomVelocities_);

    addOption<std::vector<const Species *>>("RestrictToSpecies", "Restrict the calculation to the specified Species",
                                            restrictToSpecies_);
    addOption<bool>("OnlyWhenEnergyStable", "Only run MD when target Configuration energies are stable", onlyWhenEnergyStable_);

    addOption<std::optional<Number>>("EnergyFrequency", "Frequency at which to calculate total system energy",
                                     energyFrequency_);
    addOption<std::optional<Number>>("OutputFrequency", "Frequency at which to output step information", outputFrequency_);
    addOption<std::optional<Number>>("TrajectoryFrequency", "Write frequency for trajectory file", trajectoryFrequency_);

    addOption<bool>("CapForces", "Control whether atomic forces are capped every step", capForces_);
    addOption<Number>("CapForcesAt", "Set cap on allowable force (kJ/mol) per atom", capForcesAt_);
    addOption<bool>("IntraOnly",
                    "Only forces arising from intramolecular terms (including pair potential contributions) will be calculated",
                    intramolecularForcesOnly_);
}

std::string_view MDNode::type() const { return "MD"; }

std::string_view MDNode::summary() const { return "Run a short molecular dynamics simulation."; }

EnumOptions<MDNode::TimestepType> getEnumOptions(MDNode::TimestepType) { return MDNode::timestepType(); }
