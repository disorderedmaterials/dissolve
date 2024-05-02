// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "data/formFactorData.h"
#include <vector>

// Waasmaier & Kirfel '95 Form Factor Data
class FormFactorData_WK1995 : public FormFactorData
{
    public:
    FormFactorData_WK1995(Elements::Element Z = Elements::Unknown, int formalCharge = 0, std::vector<double> a = {},
                          std::vector<double> b = {}, double c = 0.0);
    FormFactorData_WK1995 &operator=(const FormFactorData_WK1995 &source);

    /*
     * Form Factor Data
     */
    private:
    // Exponential pre-factors
    std::vector<double> a_;
    // Exponential factors
    std::vector<double> b_;
    // Constant offset
    double c_;

    public:
    // Return index of isotope in it's Element parent's list
    int index() const;

    /*
     * Form Factor Generation
     */
    public:
    // Return magnitude of form factor at specified Q value
    double magnitude(double Q = 0.0) const override;
};
