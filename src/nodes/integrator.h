// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "math/data1D.h"
#include "math/integrator.h"
#include "nodes/node.h"
#include "nodes/number.h"

class Integrator1DNode : public Node
{
    public:
    Integrator1DNode(Graph *parentGraph);
    ~Integrator1DNode() override = default;

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
    // IntegratorMethod
    enum class IntegratorMethod
    {
        Trapezoidal,
        AbsoluteTrapezoidal,
        Sum,
        AbsoluteSum,
        SumOfSquares
    };
    // Return enum options for IntegratorMethod
    static EnumOptions<IntegratorMethod> integratorMethods();

    private:
    // Input vector
    Data1D inputData_;
    // Integral
    Number integral_;
    // Integration method
    IntegratorMethod method_;

    /*
     * Processing
     */
    protected:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};

EnumOptions<Integrator1DNode::IntegratorMethod> getEnumOptions(Integrator1DNode::IntegratorMethod);
