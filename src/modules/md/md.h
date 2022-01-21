// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "module/module.h"

// Forward Declarations
class Species;

// Molecular Dynamics Module
class MDModule : public Module
{
    public:
    MDModule();
    ~MDModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configurations
    Configuration *targetConfiguration_{nullptr};
    // Control whether atomic forces are capped every step
    bool capForces_{false};
    // Set cap on allowable force (kJ/mol) per atom
    double capForcesAt_{1.0e7};
    // Interatomic cutoff distance to employ
    std::optional<double> cutoffDistance_;
    // Timestep (ps) to use in MD simulation
    double deltaT_{1.0e-4};
    // Frequency at which to calculate total system energy (or 0 to inhibit)
    int energyFrequency_{10};
    // Number of steps to perform
    int nSteps_{50};
    // Only run MD when target Configuration energies are stable
    bool onlyWhenEnergyStable_{true};
    // Frequency at which to output step information (or 0 to inhibit)
    int outputFrequency_{5};
    // Whether random velocities should always be assigned before beginning MD simulation
    bool randomVelocities_{false};
    // Species to restrict calculation to
    std::vector<const Species *> restrictToSpecies_;
    // Write frequency for trajectory file (or 0 to inhibit)
    int trajectoryFrequency_{0};
    // Whether a variable timestep should be used, determined from the maximal force vector
    bool variableTimestep_{true};

    /*
     * Functions
     */
    private:
    // Cap forces in Configuration
    int capForces(double maxForceSq, std::vector<Vec3<double>> &f);
    // Determine timestep based on maximal force component
    double determineTimeStep(const std::vector<Vec3<double>> &f);

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, ProcessPool &procPool) override;
};
