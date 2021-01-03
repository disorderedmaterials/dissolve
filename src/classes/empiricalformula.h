// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/elements.h"
#include "templates/reflist.h"

// Forward Declarations
class Species;
class SpeciesAtom;

// Empirical Formula Generator
class EmpiricalFormula
{
    private:
    // Construct empirical formula from supplied element counts
    static std::string constructFormula(const std::vector<int> &elCounts, bool richText = false);

    public:
    // Return empirical formula for supplied Species
    static std::string formula(const Species *species, bool richText = false);
    // Return empirical formula for supplied SpeciesAtom reflist
    static std::string formula(const RefList<SpeciesAtom> &atoms, bool richText = false);
};
