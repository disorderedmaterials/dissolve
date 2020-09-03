/*
    *** Interatomic Interaction Parameters Definition
    *** src/base/parameters.h
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
