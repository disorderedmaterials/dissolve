// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2025 Team Dissolve and contributors

#include "data/ff/camelloneBKS/camelloneBKS.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/coreData.h"
#include "classes/speciesAtom.h"
#include "data/ff/angleTerm.h"
#include "data/ff/atomType.h"
#include "data/ff/bondTerm.h"

/*
 * Implements "Eﬃcient generation of realistic model systems of amorphous silica"
 * Matteo Farnesi Camellone, Joachim Reiner, Urs Sennhauser, Louis Schlapbach
 * 2011
 * https://doi.org/10.48550/arXiv.1109.2852
 *
 * Notes:
 * Any inconsistencies between the forcefield as implemented here and the original work are the sole responsibility of AB.
 * All energy values are in kJ/mol, converted from eV in the paper.
 */

/*
 * Set Up
 */

// Set up / create all forcefield terms
bool Forcefield_CamelloneBKS::setUp()
{
    // Atom types
    addAtomType(Elements::O, 1, "O", "", "Silica oxygen", -1.2, "");
    addAtomType(Elements::Si, 2, "Si", "", "Silica silicon", 2.4, "");

    return true;
}

/*
 * Definition
 */

// Return name of Forcefield
std::string_view Forcefield_CamelloneBKS::name() const { return "CamelloneBKS"; }

// Return description for Forcefield
std::string_view Forcefield_CamelloneBKS::description() const
{
    return "Implements 'Eﬃcient generation of realistic model systems of amorphous silica'. "
           "Matteo Farnesi Camellone, Joachim Reiner, Urs Sennhauser, Louis Schlapbach (2011) "
           "https://doi.org/10.48550/arXiv.1109.2852";
}

// Return short-range interaction style for AtomTypes
ShortRangeFunctions::Form Forcefield_CamelloneBKS::shortRangeForm() const { return ShortRangeFunctions::Form::Undefined; }

// Return avaliable pair potential overrides
std::vector<PairPotentialOverride> Forcefield_CamelloneBKS::pairPotentialOverrides()
{
    return {{"O",
             "O",
             PairPotentialOverride::PairPotentialOverrideType::Add,
             {Functions1D::Form::Buckingham128, {133996.2240, 2.76, 16884.9331, 17367.3598, 2315.6480}}},
            {"Si",
             "O",
             PairPotentialOverride::PairPotentialOverrideType::Add,
             {Functions1D::Form::Buckingham128, {1737098.491, 4.8732, 12884.4679, 1929.7066, 578.912}}}};
}
