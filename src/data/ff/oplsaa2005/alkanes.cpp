// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/ff/oplsaa2005/alkanes.h"

/*
 * Set Up
 */

// Set up / create all forcefield terms
bool Forcefield_OPLSAA2005_Alkanes::setUp()
{
    // Copy required types from OPLS-AA (2005) core list
    // -- AA Alkanes
    if (!copyAtomType(oplsAtomTypeById(135), "CT3", "nbonds=4,nh=3", "CT"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(136), "CT2", "nbonds=4,nh=2", "CT"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(137), "CT1", "nbonds=4,nh=1", "CT"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(138), "CT4", "nbonds=4,nh=4", "CT"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(140), "HC", "-[&135,&136,&137,&138]"))
        return false;

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_OPLSAA2005_Alkanes::name() const { return "OPLSAA2005/Alkanes"; }

// Return description for Forcefield
std::string_view Forcefield_OPLSAA2005_Alkanes::description() const
{
    static std::string desc = fmt::format(
        "Alkanes from OPLS-AA (2005), covering linear and branched alkanes.<br/><br/>References: {}", publicationReferences());

    return desc;
}
