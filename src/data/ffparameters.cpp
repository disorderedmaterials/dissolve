// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/ffparameters.h"
#include "data/ff.h"

ForcefieldParameters::ForcefieldParameters(std::string_view name, const std::vector<double> parameters)
    : ListItem<ForcefieldParameters>()
{
    name_ = name;
    parameters_.setParameters(parameters);
}

/*
 * Identity
 */

// Return name of type
std::string_view ForcefieldParameters::name() const { return name_; }

/*
 * Parameters
 */

// Return interatomic interaction parameters
const InteractionParameters &ForcefieldParameters::parameters() const { return parameters_; }
