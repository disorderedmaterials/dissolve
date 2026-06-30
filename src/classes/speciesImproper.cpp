// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/speciesImproper.h"
#include "classes/species.h"
#include "classes/speciesAtom.h"
#include "classes/speciesTorsion.h"

SpeciesImproper::SpeciesImproper() : SpeciesIntra(nullptr, TorsionFunctions::Form::None) {}

SpeciesImproper::SpeciesImproper(Species *parent, SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l)
    : SpeciesIntra(parent, TorsionFunctions::Form::None), i_(i), j_(j), k_(k), l_(l)
{
}

SpeciesImproper::~SpeciesImproper() = default;

/*
 * Atom Information
 */

// Return first SpeciesAtom
SpeciesAtom *SpeciesImproper::i() const { return i_; }

// Return second SpeciesAtom
SpeciesAtom *SpeciesImproper::j() const { return j_; }

// Return third SpeciesAtom
SpeciesAtom *SpeciesImproper::k() const { return k_; }

// Return fourth SpeciesAtom
SpeciesAtom *SpeciesImproper::l() const { return l_; }

// Return vector of involved atoms
std::vector<const SpeciesAtom *> SpeciesImproper::atoms() const { return {i_, j_, k_, l_}; }

// Return whether Atoms in Improper match those specified
bool SpeciesImproper::matches(const SpeciesAtom *i, const SpeciesAtom *j, const SpeciesAtom *k, const SpeciesAtom *l) const
{
    if (i_ != i)
        return false;
    if (j_ == j)
        return (k_ == k && l_ == l) || (k_ == l && l_ == k);
    else if (j_ == k)
        return (k_ == j && l_ == l) || (k_ == l && l_ == j);
    else if (j_ == l)
        return (k_ == j && l_ == k) || (k_ == k && l_ == j);

    return false;
}

/*
 * Interaction Parameters
 */

// Return energy for specified angle phi (in radians)
double SpeciesImproper::energy(double phi) const
{
    return SpeciesTorsion::energy(phi, interactionForm(), interactionParameters());
}

// Return force multiplier for specified angle phi (in radians)
double SpeciesImproper::force(double phi) const
{
    return SpeciesTorsion::force(phi, interactionForm(), interactionParameters());
}

/*
 * Serialisation
 */

// Express as a serialisable value
void SpeciesImproper::serialise(std::string tag, SerialisedValue &target) const
{
    SpeciesIntra<SpeciesImproper, TorsionFunctions>::serialise(tag, target);
    auto &improper = target.at(tag);

    if (i_ && j_ && k_ && l_)
    {
        improper["i"] = i_->index();
        improper["j"] = j_->index();
        improper["k"] = k_->index();
        improper["l"] = l_->index();
    }
}

// Read values from a serialisable value
void SpeciesImproper::deserialise(const SerialisedValue &node)
{
    SpeciesIntra<SpeciesImproper, TorsionFunctions>::deserialise(node,
                                                                 [&](auto &form) { return parent_->getCommonImproper(form); });
}
