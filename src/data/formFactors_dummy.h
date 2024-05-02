// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/formFactorData.h"
#include <vector>

// Dummy Form Factor Data
class FormFactorData_Dummy : public FormFactorData
{
    public:
    FormFactorData_Dummy();
    FormFactorData_Dummy &operator=(const FormFactorData_Dummy &source);

    /*
     * Form Factor Generation
     */
    public:
    // Return magnitude of form factor at specified Q value
    double magnitude(double Q = 0.0) const override;
};
