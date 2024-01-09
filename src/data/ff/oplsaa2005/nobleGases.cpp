// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "data/ff/oplsaa2005/nobleGases.h"

/*
 * Set Up
 */

// Set up / create all forcefield terms
bool Forcefield_OPLSAA2005_NobleGases::setUp()
{
    // Copy required types from OPLS-AA (2005) core list
    if (!copyAtomType(oplsAtomTypeById(101), "He"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(102), "Ne"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(103), "Ar"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(104), "Kr"))
        return false;
    if (!copyAtomType(oplsAtomTypeById(105), "Xe"))
        return false;

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_OPLSAA2005_NobleGases::name() const { return "OPLSAA2005/NobleGases"; }

// Return description for Forcefield
std::string_view Forcefield_OPLSAA2005_NobleGases::description() const
{
    static std::string desc = fmt::format("Noble gases from OPLS-AA (2005).<br/><br/>References: {}", publicationReferences());

    return desc;
}
