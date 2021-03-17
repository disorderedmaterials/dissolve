// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "keywords/types.h"
#include "modules/md/md.h"

// Perform any necessary initialisation for the Module
void MDModule::initialise()
{
    // Control
    keywords_.add("Control", new DoubleKeyword(-1.0), "CutoffDistance", "Interatomic cutoff distance to employ");
    keywords_.add("Control", new IntegerKeyword(50), "NSteps", "Number of MD steps to perform");
    keywords_.add("Control", new BoolKeyword(false), "CapForces", "Control whether atomic forces are capped every step");
    keywords_.add("Control", new DoubleKeyword(1.0e7), "CapForcesAt", "Set cap on allowable force (kJ/mol) per atom");
    keywords_.add("Control", new DoubleKeyword(1.0e-4), "DeltaT", "Timestep (ps) to use in MD simulation");
    keywords_.add("Control", new BoolKeyword(true), "OnlyWhenEnergyStable",
                  "Only run MD when target Configuration energies are stable");
    keywords_.add("Control", new BoolKeyword(true), "VariableTimestep",
                  "Whether a variable timestep should be used, determined from the maximal force vector");
    keywords_.add("Control", new BoolKeyword(false), "RandomVelocities",
                  "Whether random velocities should always be assigned before beginning MD simulation");
    keywords_.add("Control", new SpeciesRefListKeyword(restrictToSpecies_), "RestrictToSpecies",
                  "Restrict the calculation to the specified Species");

    // Output
    keywords_.add("Output", new IntegerKeyword(10), "EnergyFrequency",
                  "Frequency at which to calculate total system energy (or 0 to inhibit)");
    keywords_.add("Output", new IntegerKeyword(5), "OutputFrequency",
                  "Frequency at which to output step information (or 0 to inhibit)");
    keywords_.add("Output", new IntegerKeyword(0), "TrajectoryFrequency",
                  "Write frequency for trajectory file (or 0 to inhibit)");
}
