/*
    *** Interatomic Interaction Parameters Definition
    *** src/base/parameters.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "base/parameters.h"
#include "base/messenger.h"

InteractionParameters::InteractionParameters()
{
    for (int n = 0; n < MAXSRPARAMETERS; ++n)
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
