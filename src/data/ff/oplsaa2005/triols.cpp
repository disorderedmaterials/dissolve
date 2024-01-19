// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/ff/oplsaa2005/triols.h"

/*
 * Set Up
 */

// Set up / create all forcefield terms
bool Forcefield_OPLSAA2005_Triols::setUp()
{
    // Call setup function in OPLS-AA Alkanes base FF
    if (!Forcefield_OPLSAA2005_Alkanes::setUp())
        return false;

    // Copy required types from OPLS-AA (2005) core list
    // -- Triols
    if (!copyAtomType(oplsAtomTypeById(171), "OH", "nh=1,-C(-O(root,-H))"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(172), "HO", "-&171"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(173), "CT", "nh=2,-O(-H)"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(174), "CT", "nh=1,-O(-H)"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(175), "CT", "nh=0,-O(-H)"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(176), "HC", "-[&173,&174,&175]"))
        return false;

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_OPLSAA2005_Triols::name() const { return "OPLSAA2005/Triols"; }

// Return description for Forcefield
std::string_view Forcefield_OPLSAA2005_Triols::description() const
{
    static std::string desc =
        fmt::format("Alcohols from OPLS-AA (2005), covering triols only.<br/><br/>References: {}", publicationReferences());

    return desc;
}
