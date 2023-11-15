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
    // Timestep Type
    enum class TimestepType
    {
        Fixed,
        Variable,
        Automatic
    };
    // Return enum options for TimestepType
    static EnumOptions<TimestepType> timestepType();
    
    // Filename and format for CONTROL export
    DlPolyControlExportFileFormat dlPolyControlFormat_;


    private:
    // Target configurations
    Configuration *targetConfiguration_{nullptr};
    // Control whether atomic forces are capped every step
    bool capForces_{false};
    // Set cap on allowable force (kJ/mol) per atom
    double capForcesAt_{1.0e7};
    // Interatomic cutoff distance to employ
    std::optional<double> cutoffDistance_;
    // Timestep type to employ
    TimestepType timestepType_{TimestepType::Automatic};
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
    
    /*
     * Functions
     */
    private:
    // Cap forces in Configuration
    static int capForces(double maxForceSq, std::vector<Vec3<double>> &fInter, std::vector<Vec3<double>> &fIntra);
    // Determine timestep to use
    static std::optional<double> determineTimeStep(TimestepType timestepType, double requestedTimeStep,
                                                   const std::vector<Vec3<double>> &fInter,
                                                   const std::vector<Vec3<double>> &fIntra);

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
