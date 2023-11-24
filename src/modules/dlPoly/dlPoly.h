// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "io/export/dlPolyControl.h"
#include "base/enumOptions.h"
#include "module/module.h"

// Forward Declarations
class PotentialMap;
class Species;

// Molecular Dynamics Module
class DlPolyModule : public Module
{
    public:
    DlPolyModule();
    ~DlPolyModule() override = default;

    /*
     * Definition
     */
    public:
    // TrajectoryKey Enum
    enum TrajectoryKey
    {
        pos,
        pos_vel,
        pos_vel_force
    };
    // Return EnumOptions for TrajectoryKey
    static EnumOptions<TrajectoryKey> trajectoryKey();
    
    // CoulMethod Enum
    enum CoulMethod
    {
        off,
        spme,
        dddp,
        pairwise,
        reaction_field,
        force_shifted
    };
    // Return EnumOptions for CoulMethod
    static EnumOptions<CoulMethod> coulMethod();
         
    private:
    // Filename and format for CONTROL export
    DlPolyControlExportFileFormat dlPolyControlFormat_;
    // Target configurations
    Configuration *targetConfiguration_{nullptr};
    // Control whether atomic forces are capped every step
    bool capForces_{false};
    // Set cap on allowable force (kJ/mol) per atom
    double capForcesAt_{1.0e7};
    // Interatomic cutoff distance to employ
    std::optional<double> cutoffDistance_;
    // Timestep type to employ
    bool timestepVariable_{false};
    // Fixed timestep (ps) to use in MD simulation
    double fixedTimestep_{5.0e-4};
    // Frequency at which to calculate total system energy
    std::optional<int> energyFrequency_{10};
    // Number of steps to perform
    int nSteps_{50};
    // Frequency at which to output step information
    std::optional<int> outputFrequency_{5};
    // Whether random velocities should always be assigned before beginning MD simulation
    bool randomVelocities_{false};
    // Write frequency for trajectory file
    std::optional<int> trajectoryFrequency_;
    // Type of target TrajectoryKeySet
    DlPolyModule::TrajectoryKey trajectoryKey_{TrajectoryKey::pos};
    // Set method for electrostatics method
    DlPolyModule::CoulMethod coulMethod_{CoulMethod::off};
    // Calculate electrostatics with given precision
    double coulPrecision_{0.0}; 
    
    /*
     * Functions
     */
    private:
    // Cap forces in Configuration
    static int capForces(double maxForceSq, std::vector<Vec3<double>> &fInter, std::vector<Vec3<double>> &fIntra);

    public:
    // Evolve Species coordinates, returning new coordinates
    static std::vector<Vec3<double>> evolve(const ProcessPool &procPool, const PotentialMap &potentialMap, const Species *sp,
                                            double temperature, int nSteps, double maxDeltaT,
                                            const std::vector<Vec3<double>> &rInit, std::vector<Vec3<double>> &velocities);

    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
