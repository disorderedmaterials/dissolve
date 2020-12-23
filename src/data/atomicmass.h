// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/elements.h"

// Elemental (natural) atomic mass
class AtomicMassData : public Elements::ElementReference
{
    public:
    AtomicMassData(int z, double massMin, double massMax = -1.0);

    private:
    // Mass of element
    double mass_;

    public:
    // Return mass of element
    double mass() const;
};

// Atomic Mass Helper Class
class AtomicMass
{
    private:
    // Return mass data for specified Z
    static const AtomicMassData &massData(int Z);

    public:
    // Return atomic mass for specified Z
    static double mass(int Z);
    // Return atomic mass for specified Element
    static double mass(Elements::Element *element);
    // Return reduced mass for specified pair of Z's
    static double reducedMass(int Z1, int Z2);
    // Return reduced mass for specified pair of Elements
    static double reducedMass(Elements::Element *e1, Elements::Element *e2);
};
