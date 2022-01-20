// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "data/ff/oplsaa2005/alcohols.h"

/*
 * Set Up
 */

// Set up / create all forcefield terms
bool Forcefield_OPLSAA2005_Alcohols::setUp()
{
    // Call setup function in OPLS-AA Alkanes base FF
    if (!Forcefield_OPLSAA2005_Alkanes::setUp())
        return false;

    // Copy required types from OPLS-AA (2005) core list
    // -- Mono alcohols
    if (!copyAtomType(oplsAtomTypeById(154), "OH", "nh=1,-C(-O(root,-H))"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(155), "HO", "-&154"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(157), "CT", "nh>=2,-O"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(158), "CT", "nh=1,-O"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(159), "CT", "nh=0,-O"))
        return false;
    // -- Methanol
    if (!copyAtomType(oplsAtomTypeById(156), "HC", "-C(nh=3,-&154)"))
        return false;

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_OPLSAA2005_Alcohols::name() const { return "OPLSAA2005/Alcohols"; }

// Return description for Forcefield
std::string_view Forcefield_OPLSAA2005_Alcohols::description() const
{
    static std::string desc = fmt::format("Alcohols from OPLS-AA (2005), covering mono-alcohols only.<br/><br/>References: {}",
                                          publicationReferences());

    return desc;
}
