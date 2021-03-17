// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/isotopologueweight.h"

IsotopologueWeight::IsotopologueWeight(const Isotopologue *iso, double weight) : isotopologue_(iso), weight_(weight) {}

IsotopologueWeight::~IsotopologueWeight() {}

/*
 * Isotopologue and Weight
 */

// Set associated Isotopologue and weight
void IsotopologueWeight::set(const Isotopologue *iso, double weight)
{
    isotopologue_ = iso;
    weight_ = weight;
}

// Set associated Isotopologue
void IsotopologueWeight::setIsotopologue(const Isotopologue *iso) { isotopologue_ = iso; }

// Return associated Isotopologue
const Isotopologue *IsotopologueWeight::isotopologue() const { return isotopologue_; }

// Set relative weight
void IsotopologueWeight::setWeight(double weight) { weight_ = weight; }

// Return relative weight
double IsotopologueWeight::weight() const { return weight_; }
