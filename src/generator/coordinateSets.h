// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "generator/node.h"
#include "generator/nodeValue.h"
#include "io/import/coordinates.h"
#include <vector>

// Forward Declarations
class Species;

// MD Node
class CoordinateSetsProcedureNode : public ProcedureNode
{
    public:
    CoordinateSetsProcedureNode(const Species *sp = nullptr);
    ~CoordinateSetsProcedureNode() override = default;

    /*
     * Identity
     */
    public:
    // Return whether a name for the node must be provided
    bool mustBeNamed() const override;

    /*
     * Node Data
     */
    public:
    // Coordinate Set Source
    enum class CoordinateSetSource
    {
        File, /* Import coordinates sets from file */
        MD    /* Create coordinates via MD evolution of the parent species */
    };
    // Return enum option info for CoordinateSetSource
    static EnumOptions<CoordinateSetSource> coordinateSetSources();

    private:
    // Target Species
    const Species *species_{nullptr};
    // Available coordinate sets
    std::vector<std::vector<Vec3<double>>> sets_;
    // Set Source
    CoordinateSetSource source_{CoordinateSetSource::MD};
    // Whether to force recreation of the coordinate sets
    bool force_{false};
    // File / format of coordinate sets file, if provided
    CoordinateImportFileFormat fileSource_;
    // Number of sets to generate
    NodeValue nSets_{100};
    // Number of MD steps to run between sets
    NodeValue nSteps_{2000};
    // Simulation temperature
    NodeValue temperature_{300.0};
    // Simulation timestep
    NodeValue deltaT_{5.0e-4};

    private:
    // Add new coordinate set
    std::vector<Vec3<double>> &addSet();

    public:
    // Set coordinate sets
    void setSets(std::vector<std::vector<Vec3<double>>> sets);
    // Return number of available coordinates sets
    int nSets() const;
    // Return nth coordinate set
    const std::vector<Vec3<double>> &set(int n) const;

    /*
     * Execute
     */
    public:
    // Prepare any necessary data, ready for execution
    bool prepare(const ProcedureContext &procedureContext) override;
    // Execute node
    bool execute(const ProcedureContext &procedureContext) override;
};
