// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors


#pragma once

#include <vector>

// Data Base class to allow for polymorphism between children of Data1DBase, Data2DBase and Data3DBase
class DataBase
{
    /*
     * Axis Information
     */
    public:
    // Return x axis vector
    virtual const std::vector<double> &xAxis() const = 0;

    /*
     * Values / Errors
     */
    public:
    // Return data version
    virtual int version() const = 0;
    // Return number of values present in the whole dataset
    virtual int nValues() const = 0;
    // Return whether the values have associated errors
    virtual bool valuesHaveErrors() const = 0;

    /*
     * Limits
     */
    public:
    // Return minimum value over all data points
    virtual double minValue() const = 0;
    // Return maximum value over all data points
    virtual double maxValue() const = 0;
};
