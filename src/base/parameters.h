// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <vector>

// Interatomic Interaction Parameters
class InteractionParameters
{
    public:
    InteractionParameters();
    ~InteractionParameters() = default;

    /*
     * Parameters
     */
    private:
    // Whether the parameters / charge are empty (i.e. none have ever been set)
    bool empty_;
    // Vector of parameters
    std::vector<double> parameters_;
    // Atomic charge
    double charge_;

    public:
    // Return whether the parameters / charge are empty (i.e. none have ever been set)
    bool isEmpty() const;
    // Set parameter with index specified
    void setParameter(int index, double value);
    // Set parameters array
    void setParameters(const std::vector<double> &params);
    // Return parameters array
    const std::vector<double> &parameters() const;
    // Return parameter with index specified
    double parameter(int index) const;
    // Set atomic charge
    void setCharge(double charge);
    // Return atomic charge
    double charge() const;
};
