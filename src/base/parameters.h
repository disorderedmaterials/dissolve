// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#define MAXSRPARAMETERS 4

// Interatomic Interaction Parameters
class InteractionParameters
{
    public:
    InteractionParameters();
    ~InteractionParameters();

    /*
     * Parameters
     */
    private:
    // Whether the parameters / charge are empty (i.e. none have ever been set)
    bool empty_;
    // Parameter array
    double parameters_[MAXSRPARAMETERS];
    // Atomic charge
    double charge_;

    public:
    // Return whether the parameters / charge are empty (i.e. none have ever been set)
    bool isEmpty() const;
    // Set parameter with index specified
    void setParameter(int index, double value);
    // Return parameter with index specified
    double parameter(int index) const;
    // Set atomic charge
    void setCharge(double charge);
    // Return atomic charge
    double charge() const;
};
