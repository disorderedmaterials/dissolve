// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/ff/oplsaa2005/alkenes.h"

/*
 * Set Up
 */

// Set up / create all forcefield terms
bool Forcefield_OPLSAA2005_Alkenes::setUp()
{
    // Call setup function in OPLS-AA Alkanes base FF
    if (!Forcefield_OPLSAA2005_Alkanes::setUp())
        return false;

    // Add required types from OPLS-AA (2005) core list
    // -- AA Alkenes
    if (!copyAtomType(oplsAtomTypeById(141), "CM_a", "nbonds=3,nh=0,-C", "CM"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(142), "CM_a", "nbonds=3,nh=1,-C", "CM"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(143), "CM_a", "nbonds=3,nh=2,-C", "CM"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(144), "HC_d", "-C(nbonds=3)", "HC"))
        return false;
    // -- AA Dienes
    if (!copyAtomType(oplsAtomTypeById(150), "C=", "nbonds=3,nh=1,-C(nh=1,nbonds=3)"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(178), "C=", "nbonds=3,nh=0,-C(nh=0,nbonds=3)"))
        return false;

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_OPLSAA2005_Alkenes::name() const { return "OPLSAA2005/Alkenes"; }

// Return description for Forcefield
std::string_view Forcefield_OPLSAA2005_Alkenes::description() const
{
    static std::string desc = fmt::format("Alkenes from OPLS-AA (2005), covering alkenes and dienes.<br/><br/>References: {}",
                                          publicationReferences());

    return desc;
}
