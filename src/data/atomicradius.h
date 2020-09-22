// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "data/elements.h"

// Elemental atomic radius
class AtomicRadiusData : public ElementReference
{
    public:
    AtomicRadiusData(int z, double radius);

    private:
    // Radius of element
    double radius_;

    public:
    // Return radius of element
    double radius() const;
};

// Atomic Radius Helper Class
class AtomicRadius : public Elements
{
    private:
    // Return radius data for specified Z
    static const AtomicRadiusData &radiusData(int Z);

    public:
    // Return atomic radius for specified Z
    static double radius(int Z);
    // Return atomic radius for specified Element
    static double radius(Element *element);
};
