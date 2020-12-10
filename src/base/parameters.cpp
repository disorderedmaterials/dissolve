// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "base/parameters.h"
#include "base/messenger.h"

InteractionParameters::InteractionParameters()
{
    for (auto n = 0; n < MAXSRPARAMETERS; ++n)
        parameters_[n] = 0.0;
    charge_ = 0.0;
    empty_ = true;
}

InteractionParameters::~InteractionParameters() {}

/*
 * Parameters
 */

// Return whether the parameters / charge are empty (i.e. none have ever been set)
bool InteractionParameters::isEmpty() const { return empty_; }

// Set parameter with index specified
void InteractionParameters::setParameter(int index, double value)
{
#ifdef CHECKS
    if ((index < 0) || (index >= MAXSRPARAMETERS))
    {
        Messenger::error("OUT_OF_RANGE - Parameter index {} is out of range (MAXSRPARAMETERS = {}) so it cannot be set.\n",
                         index, MAXSRPARAMETERS);
        return;
    }
#endif
    parameters_[index] = value;

    empty_ = false;
}

// Return parameter with index specified
double InteractionParameters::parameter(int index) const
{
#ifdef CHECKS
    if ((index < 0) || (index >= MAXSRPARAMETERS))
    {
        Messenger::error("OUT_OF_RANGE - Parameter index {} is out of range (MAXSRPARAMETERS = {}) so it cannot be returned.\n",
                         index, MAXSRPARAMETERS);
        return 0.0;
    }
#endif
    return parameters_[index];
}

// Set atomic charge
void InteractionParameters::setCharge(double charge)
{
    charge_ = charge;

    empty_ = false;
}

// Return atomic charge
double InteractionParameters::charge() const { return charge_; }
