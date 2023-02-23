// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "module/module.h"
#include "procedure/procedure.h"

// Forward Declarations
class Collect1DProcedureNode;
class OperateSitePopulationNormaliseProcedureNode;
class Process1DProcedureNode;
class SelectProcedureNode;
class SpeciesSite;
class Sum1DProcedureNode;

// Calculate RDF Module
class IntraDistanceModule : public Module
{
    public:
    IntraDistanceModule();
    ~IntraDistanceModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Whether to exclude correlations between sites on the same molecule
    bool excludeSameMolecule_{false};
    // Range (min, max, delta) of distance axis
    Vec3<double> distanceRange_{0.0, 10.0, 0.05};
    // Whether to calculate the instantaneous coordination numbers rather than forming an average
    bool instantaneous_{false};
    // Whether to export instantaneous coordination numbers to disk
    bool exportInstantaneous_{false};
    // Threshold difference at which test comparisons will fail
    double testThreshold_{0.1};
    // Analysis procedure to be run
    Procedure analyser_;
    // SelectNode for site A
    std::shared_ptr<SelectProcedureNode> selectA_;
    // SelectNode for site B
    std::shared_ptr<SelectProcedureNode> selectB_;
    // Collect1DNode for A-B RDF
    std::shared_ptr<Collect1DProcedureNode> collectDistance_;
    // Process1DNode for A-B RDF
    std::shared_ptr<Process1DProcedureNode> processDistance_;

    /*
     * Processing
     */
    private:
    // Run main processing
    bool process(Dissolve &dissolve, const ProcessPool &procPool) override;
};
