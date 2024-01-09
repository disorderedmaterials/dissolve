// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "math/range.h"
#include "module/module.h"
#include "procedure/procedure.h"

// Forward Declarations
class IntegerCollect1DProcedureNode;
class CalculateExpressionProcedureNode;
class OperateSitePopulationNormaliseProcedureNode;
class Process1DProcedureNode;
class SelectProcedureNode;
class SpeciesSite;
class Sum1DProcedureNode;

// Calculate Q-Species Module
class QSpeciesModule : public Module
{
    public:
    QSpeciesModule();
    ~QSpeciesModule() override = default;

    /*
     * Definition
     */
    private:
    // Target configuration
    Configuration *targetConfiguration_{nullptr};
    // Whether to exclude correlations between sites on the same molecule
    bool excludeSameMolecule_{false};
    // Distance range for calculation
    Range distanceRange_{0.0, 2.5};
    // Analysis procedure to be run
    Procedure analyser_;
    // SelectNode for site A
    std::shared_ptr<SelectProcedureNode> selectBO_;
    // SelectNode for site B
    std::shared_ptr<SelectProcedureNode> selectNF_;

    public:
    /*
     * Processing
     */
    private:
    // Run main processing
    Module::ExecutionResult process(ModuleContext &moduleContext) override;
};
