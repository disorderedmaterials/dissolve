// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/ff/oplsaa2005/aromatics.h"

/*
 * Set Up
 */

// Set up / create all forcefield terms
bool Forcefield_OPLSAA2005_Aromatics::setUp()
{
    // Copy required types from OPLS-AA (2005) core list
    // -- AA Alkanes
    if (!copyAtomType(oplsAtomTypeById(135), "CT", "nh=3"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(140), "HC", "-&135"))
        return false;
    // -- Benzene
    if (!copyAtomType(oplsAtomTypeById(145), "CA", "-C(n=2),-H(n=1),ring(size=6)"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(146), "HA", "-&145"))
        return false;
    // -- Napthalene (or larger aromatics)
    if (!copyAtomType(oplsAtomTypeById(147), "CNap", "nbonds=3,-C(n=3),ring(size=6,C(n=6),n>=2)", "CA"))
        return false;
    // -- Toluene
    if (!copyAtomType(oplsAtomTypeById(148), "CT", "nh=3, -C(ring(size=6))"))
        return false;
    // -- Ethylbenzene
    if (!copyAtomType(oplsAtomTypeById(149), "CT", "nh=2, -C(nh=3), -C(ring(size=6))"))
        return false;
    // -- Phenol
    if (!copyAtomType(oplsAtomTypeById(166), "CA", "nbonds=3,-O(-H),-C(n=2)"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(167), "OH", "-&166"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(168), "HO", "-&167"))
        return false;

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_OPLSAA2005_Aromatics::name() const { return "OPLSAA2005/Aromatics"; }

// Return description for Forcefield
std::string_view Forcefield_OPLSAA2005_Aromatics::description() const
{
    static std::string desc = fmt::format(
        "Aromatics from OPLS-AA (2005), covering benzene, toluene, napthalene, ethylbenzene.<br/><br/>References: {}",
        publicationReferences());

    return desc;
}
