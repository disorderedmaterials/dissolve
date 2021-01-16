// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/parameters.h"
#include "base/messenger.h"

InteractionParameters::InteractionParameters() : parameters_{0.0}, charge_(0.0), empty_(true) {}

/*
 * Parameters
 */

// Return whether the parameters / charge are empty (i.e. none have ever been set)
bool InteractionParameters::isEmpty() const { return empty_; }

// Set parameter with index specified
void InteractionParameters::setParameter(int index, double value)
{
    parameters_[index] = value;
    empty_ = false;
}

// Set parameters array
void InteractionParameters::setParameters(const std::vector<double> &params)
{
    parameters_ = params;
    empty_ = false;
}

// Return parameters array
const std::vector<double> &InteractionParameters::parameters() const { return parameters_; }

// Return parameter with index specified
double InteractionParameters::parameter(int index) const { return parameters_[index]; }

// Set atomic charge
void InteractionParameters::setCharge(double charge)
{
    charge_ = charge;

    empty_ = false;
}

// Return atomic charge
double InteractionParameters::charge() const { return charge_; }
