// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/speciesBond.h"
#include "base/sysFunc.h"
#include "classes/coreData.h"
#include "classes/speciesAtom.h"
#include "data/atomicMasses.h"
#include <map>

SpeciesBond::SpeciesBond() : SpeciesIntra(nullptr, BondFunctions::Form::None) {}

SpeciesBond::SpeciesBond(Species *parent, SpeciesAtom *i, SpeciesAtom *j)
    : Bond(i, j), SpeciesIntra(parent, BondFunctions::Form::None)
{
}

SpeciesBond::~SpeciesBond() = default;

/*
 * SpeciesAtom Information
 */

// Return vector of involved atoms
std::vector<const SpeciesAtom *> SpeciesBond::atoms() const { return {i_, j_}; }

// Return whether SpeciesAtoms in Angle match those specified
bool SpeciesBond::matches(const SpeciesAtom *i, const SpeciesAtom *j) const
{
    return (i_ == i && j_ == j) || (i_ == j && j_ == i);
}

/*
 * Interaction Parameters
 */

// Return energy for specified distance
double SpeciesBond::energy(double distance) const
{
    // Get pointer to relevant parameters array
    const auto &params = interactionParameters();
    const auto bondForm = interactionForm();

    if (bondForm == BondFunctions::Form::None)
        return 0.0;
    else if (bondForm == BondFunctions::Form::Harmonic)
    {
        /*
         * Parameters:
         * 0 : force constant
         * 1 : equilibrium distance
         */
        auto delta = distance - params[1];
        return 0.5 * params[0] * delta * delta;
    }
    else if (bondForm == BondFunctions::Form::EPSR)
    {
        /*
         * Basically a harmonic oscillator metered by the mass of the atoms
         *
         * Parameters:
         * 0 : general force constant C / 2.0
         * 1 : equilibrium distance
         *                        eq
         * omegaSq = -----------------------------
         *           sqrt( (mi + mj) / (mi * mj) )
         */
        auto delta = distance - params[1];
        auto massI = AtomicMass::mass(i_->Z());
        auto massJ = AtomicMass::mass(j_->Z());
        return params[0] * delta * delta / (params[1] / sqrt((massI + massJ) / (massI * massJ)));
    }
    else if (bondForm == BondFunctions::Form::Morse)
    {
        /*
         * Parameters:
         * 0 : potential well depth (D)
         * 1 : potential well width (alpha)
         * 2 : equilibrium bond length
         */
        auto oneMinusE = 1.0 - exp(-params[1] * (distance - params[2]));
        return params[0] * oneMinusE * oneMinusE;
    }
    else if (bondForm == BondFunctions::Form::Buckingham)
    {
        /*
         * Parameters:
         * 0 : A
         * 1 : B
         * 2 : C
         */
        return params[0] * exp(-params[1] * distance) - params[2] / pow(distance, 6.0);
    }

    Messenger::exception("Bond functional form '{}' not accounted for, so can't calculate energy.\n",
                         BondFunctions::forms().keyword(bondForm));
}

// Return force multiplier for specified distance
double SpeciesBond::force(double distance) const
{
    // Get pointer to relevant parameters array
    const auto &params = interactionParameters();
    const auto bondForm = interactionForm();

    if (bondForm == BondFunctions::Form::None)
        return 0.0;
    else if (bondForm == BondFunctions::Form::Harmonic)
    {
        /*
         * V = -k * (r - eq)
         *
         * Parameters:
         * 0 : force constant
         * 1 : equilibrium distance
         */
        return -params[0] * (distance - params[1]);
    }
    else if (bondForm == BondFunctions::Form::EPSR)
    {
        /*
         * Basically a harmonic oscillator metered by the mass of the atoms
         *
         * Parameters:
         * 0 : general force constant C / 2.0
         * 1 : equilibrium distance
         */
        auto massI = AtomicMass::mass(i_->Z());
        auto massJ = AtomicMass::mass(j_->Z());
        return -2.0 * params[0] * (distance - params[1]) / (params[1] / sqrt((massI + massJ) / (massI * massJ)));
    }
    else if (bondForm == BondFunctions::Form::Morse)
    {
        /*
         * Parameters:
         * 0 : potential well depth (D)
         * 1 : potential well width (alpha)
         * 2 : equilibrium bond length
         */
        auto e = exp(-params[1] * (distance - params[2]));
        return -2.0 * params[0] * params[1] * (1.0 - e) * e;
    }
    else if (bondForm == BondFunctions::Form::Buckingham)
    {
        /*
         * Parameters:
         * 0 : A
         * 1 : B
         * 2 : C
         */
        return params[1] * params[0] * exp(-params[1] * distance) - 6 * params[2] * pow(distance, -7.0);
    }

    Messenger::exception("Bond functional form '{}' not accounted for, so can't calculate force.\n",
                         BondFunctions::forms().keyword(bondForm));
}

/*
 * Serialisation
 */

// Express as a serialisable value
void SpeciesBond::serialise(std::string tag, SerialisedValue &target) const
{
    SpeciesIntra<SpeciesBond, BondFunctions>::serialise(tag, target);
    auto &bond = target[tag];

    if (i_ && j_)
    {
        bond["i"] = i_->index();
        bond["j"] = j_->index();
    }
}

// Read values from a serialisable value
void SpeciesBond::deserialise(const SerialisedValue &node)
{
    SpeciesIntra<SpeciesBond, BondFunctions>::deserialise(node, [&](auto &form) { return parent_->getCommonBond(form); });
}
