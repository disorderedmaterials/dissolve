/*
    *** Forcefield Short-Range Parameters
    *** src/data/ffparameters.h
    Copyright T. Youngs 2019-2020

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

#include "base/parameters.h"
#include "templates/listitem.h"
#include <string>

// Forward Declarations
class Forcefield;

// Forcefield Parameters Base Class
class ForcefieldParameters : public ListItem<ForcefieldParameters>
{
    public:
    ForcefieldParameters(std::string_view name = "", double data0 = 0.0, double data1 = 0.0, double data2 = 0.0,
                         double data3 = 0.0);
    ~ForcefieldParameters();

    /*
     * Identity
     */
    private:
    // Name of parameters
    std::string name_;

    public:
    // Return name of parameter
    std::string_view name() const;

    /*
     * Parameters
     */
    private:
    // Interatomic interaction parameters
    InteractionParameters parameters_;

    public:
    // Return interatomic interaction parameters
    const InteractionParameters &parameters() const;
};
