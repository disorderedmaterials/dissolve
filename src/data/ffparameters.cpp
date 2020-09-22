// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#include "data/ffparameters.h"
#include "data/ff.h"

ForcefieldParameters::ForcefieldParameters(std::string_view name, double data0, double data1, double data2, double data3)
    : ListItem<ForcefieldParameters>()
{
    name_ = name;
    parameters_.setParameter(0, data0);
    parameters_.setParameter(1, data1);
    parameters_.setParameter(2, data2);
    parameters_.setParameter(3, data3);
}

ForcefieldParameters::~ForcefieldParameters() {}

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
