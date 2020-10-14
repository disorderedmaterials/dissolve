// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

// Form Factor Base Class
class FormFactorData
{
    public:
    FormFactorData(int Z = 0, int formalCharge = 0);

    /*
     * Element State
     */
    protected:
    // Element to which the data relates
    int Z_;
    // Formal charge of element to which the data relates
    int formalCharge_;

    public:
    // Return element to which the data relates
    int Z() const;
    // Return formal charge of element to which the data relates
    int formalCharge() const;

    /*
     * Form Factor Generation
     */
    public:
    // Return magnitude of form factor at specified Q value
    virtual double magnitude(double Q = 0.0) const = 0;
};
