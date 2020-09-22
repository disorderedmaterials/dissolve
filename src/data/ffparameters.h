// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

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
