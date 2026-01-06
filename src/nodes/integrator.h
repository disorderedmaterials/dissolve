// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/enumOptions.h"
#include "math/data1D.h"
#include "math/integrator.h"
#include "nodes/node.h"
#include "nodes/number.h"

// Integrator1D Node
class Integrator1DNode : public Node
{
    public:
    Integrator1DNode(Graph *parentGraph);
    ~Integrator1DNode() override = default;

    public:
    std::string_view type() const override;
    std::string_view summary() const override;

    // Integrator type
    enum class Method
    {
        Trapezoidal,
        AbsoluteTrapezoidal,
        Sum,
        AbsoluteSum,
        SumOfSquares
    };

    // Return enum options for form
    static EnumOptions<Method> types();

    /*
     * Definition
     */
    private:
    //
    std::string_view type_;
    // Input vector
    Data1D inputData_;
    // Integral
    Number integral_;

    /*
     * Processing
     */
    private:
    // Run main processing
    NodeConstants::ProcessResult process() override;
};
