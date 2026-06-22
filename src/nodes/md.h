// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "base/lineParser.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "main/dissolve.h"
#include "nodes/node.h"
#include "nodes/number.h"

class MDNode : public Node
{
    public:
    MDNode(Graph *parentGraph);
    ~MDNode() override = default;

    /*
     * Definition
     */
    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    /*
     * Data
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

    private:
    // Target configurations
    Configuration *targetConfiguration_{nullptr};
    // Control whether atomic forces are capped every step
    bool capForces_{false};
    // Set cap on allowable force (kJ/mol) per atom
    Number capForcesAt_{1.0e7};
    // Timestep type to employ
    TimestepType timestepType_{TimestepType::Automatic};
    // Fixed timestep (ps) to use in MD simulation
    Number fixedTimestep_{5.0e-4};
    // Frequency at which to calculate total system energy
    std::optional<Number> energyFrequency_{10};
    // Whether to restrict force calculation to intramolecular contributions only
    bool intramolecularForcesOnly_{false};
    // Number of steps to perform
    Number nSteps_{50};
    // Only run MD when target Configuration energies are stable
    bool onlyWhenEnergyStable_{true};
    // Frequency at which to output step information
    std::optional<Number> outputFrequency_{5};
    // Whether random velocities should always be assigned before beginning MD simulation
    bool randomVelocities_{false};
    // Species to restrict calculation to
    std::vector<const Species *> restrictToSpecies_;
    // Write frequency for trajectory file
    std::optional<Number> trajectoryFrequency_;
    // Velocities
    std::optional<std::vector<Vector3>> velocities_;

    /*
     * Processing
     */
    private:
    // Cap forces
    static int capForces(double maxForceSq, std::vector<Vector3> &fInter, std::vector<Vector3> &fIntra);
    // Determine timestep to use
    static std::optional<double> determineTimeStep(TimestepType timestepType, double requestedTimeStep,
                                                   const std::vector<Vector3> &fInter, const std::vector<Vector3> &fIntra);

    protected:
    // Run main processing
    NodeConstants::ProcessResult process() override;

    public:
    // Evolve Species coordinates, returning new coordinates
    static std::vector<Vector3> evolve(const ProcessPool &procPool, const PotentialMap &potentialMap, const Species *sp,
                                       double temperature, int nSteps, double maxDeltaT, const std::vector<Vector3> &rInit,
                                       std::vector<Vector3> &velocities);
};

EnumOptions<MDNode::TimestepType> getEnumOptions(MDNode::TimestepType);
