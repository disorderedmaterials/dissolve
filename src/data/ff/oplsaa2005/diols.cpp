// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/ff/oplsaa2005/diols.h"

/*
 * Set Up
 */

// Set up / create all forcefield terms
bool Forcefield_OPLSAA2005_Diols::setUp()
{
    // Call setup function in OPLS-AA Alkanes base FF
    if (!Forcefield_OPLSAA2005_Alkanes::setUp())
        return false;

    // Copy required types from OPLS-AA (2005) core list
    // -- Diols
    if (!copyAtomType(oplsAtomTypeById(169), "OH", "nh=1,-C(-O(root,-H))"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(170), "HO", "-&169"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(157), "CT", "nh>=2,-O(-H)"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(158), "CT", "nh=1,-O(-H)"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(159), "CT", "nh=0,-O(-H)"))
        return false;

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_OPLSAA2005_Diols::name() const { return "OPLSAA2005/Diols"; }

// Return description for Forcefield
std::string_view Forcefield_OPLSAA2005_Diols::description() const
{
    static std::string desc =
        fmt::format("Alcohols from OPLS-AA (2005), covering diols only.<br/><br/>References: {}", publicationReferences());

    return desc;
}
