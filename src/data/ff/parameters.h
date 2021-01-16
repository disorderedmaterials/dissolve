// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

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
    ForcefieldParameters(std::string_view name = "", const std::vector<double> parameters = {});
    ~ForcefieldParameters() = default;

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
