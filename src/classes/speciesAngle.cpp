// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/speciesAngle.h"
#include "classes/coreData.h"
#include "classes/speciesAtom.h"
#include "math/mathFunc.h"
#include <map>

SpeciesAngle::SpeciesAngle() : SpeciesIntra(nullptr, AngleFunctions::Form::None) {}

SpeciesAngle::SpeciesAngle(Species *parent, SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k)
    : SpeciesIntra(parent, AngleFunctions::Form::None), i_(i), j_(j), k_(k)
{
}

SpeciesAngle::~SpeciesAngle() = default;

/*
 * Atom Information
 */

// Return first SpeciesAtom
SpeciesAtom *SpeciesAngle::i() const { return i_; }

// Return second (central) SpeciesAtom
SpeciesAtom *SpeciesAngle::j() const { return j_; }

// Return third SpeciesAtom
SpeciesAtom *SpeciesAngle::k() const { return k_; }

// Return vector of involved atoms
std::vector<const SpeciesAtom *> SpeciesAngle::atoms() const { return {i_, j_, k_}; }

// Return index (in parent Species) of first SpeciesAtom
int SpeciesAngle::indexI() const
{
    assert(i_);
    return i_->index();
}

// Return index (in parent Species) of second (central) SpeciesAtom
int SpeciesAngle::indexJ() const
{
    assert(j_);
    return j_->index();
}

// Return index (in parent Species) of third SpeciesAtom
int SpeciesAngle::indexK() const
{
    assert(k_);
    return k_->index();
}

// Return index (in parent Species) of nth SpeciesAtom in interaction
int SpeciesAngle::index(int n) const
{
    if (n == 0)
        return indexI();
    else if (n == 1)
        return indexJ();
    else if (n == 2)
        return indexK();

    Messenger::error("SpeciesAtom index {} is out of range in SpeciesAngle::index(int). Returning 0...\n", n);
    return 0;
}

// Return whether Atoms in Angle match those specified
bool SpeciesAngle::matches(const SpeciesAtom *i, const SpeciesAtom *j, const SpeciesAtom *k) const
{
    return (j_ == j) && ((i_ == i && k_ == k) || (i_ == k && k_ == i));
}

/*
 * Interaction Parameters
 */

// Return energy for specified angle theta (in radians)
double SpeciesAngle::energy(double theta) const
{
    // Get pointer to relevant parameters array
    const auto &params = interactionParameters();
    const auto angleForm = interactionForm();

    if (angleForm == AngleFunctions::Form::None)
        return 0.0;
    else if (angleForm == AngleFunctions::Form::Harmonic)
    {
        /*
         * U(theta) = 0.5 * forcek * (theta - eq)**2
         *
         * Parameters:
         * 0 : Force constant, k
         * 1 : Equilibrium angle, eq (degrees)
         */
        const auto delta = theta - DissolveMath::toRadians(params[1]);
        return 0.5 * params[0] * delta * delta;
    }
    else if (angleForm == AngleFunctions::Form::Cosine)
    {
        /*
         * U(theta) = forcek * (1 + s * cos(n*theta - eq))
         *
         * Parameters:
         * 0 : Force constant, k
         * 1 : Periodicity, N
         * 2 : Equilibrium angle, eq (degrees)
         * 3 : Sign, s
         */
        return params[0] * (1.0 + params[3] * cos(params[1] * theta - DissolveMath::toRadians(params[2])));
    }
    else if (angleForm == AngleFunctions::Form::Cos2)
    {
        /*
         * U(theta) = forcek * (C0 + C1 * cos(theta) + C2 * cos(2*theta))
         *
         * Parameters:
         * 0 : Force constant, k
         * 1 : Constant C0
         * 2 : Constant C1
         * 3 : Constant C2
         */
        return params[0] * (params[1] + params[2] * cos(theta) + params[3] * cos(2.0 * theta));
    }

    Messenger::exception("Angle functional form '{}' not accounted for, so can't calculate energy.\n",
                         AngleFunctions::forms().keyword(angleForm));
}

// Return force multiplier for specified angle theta (in radians)
double SpeciesAngle::force(double theta) const
{
    /*
     * Force of any angle form is given via the chain rule:
     *
     *                    dU     dTheta
     *     F(theta) = - ------ ----------
     *                  dTheta cos(theta)
     *
     *                  dU       1
     *              = ------ ---------
     *                dTheta sin(theta)
     */

    // Get pointer to relevant parameters array
    const auto &params = interactionParameters();
    const auto angleForm = interactionForm();

    if (angleForm == AngleFunctions::Form::None)
        return 0.0;
    else if (angleForm == AngleFunctions::Form::Harmonic)
    {
        /*
         * dU/dTheta = k * (theta - eq)
         *
         * Parameters:
         * 0 : Force constant, k
         * 1 : Equilibrium angle, eq (degrees)
         */

        return params[0] * (theta - DissolveMath::toRadians(params[1])) / sin(theta);
    }
    else if (angleForm == AngleFunctions::Form::Cosine)
    {
        /*
         * dU/dTheta = -k * n * s * sin(n*theta - eq)
         *
         * Parameters:
         * 0 : Force constant, k
         * 1 : Periodicity, N
         * 2 : Equilibrium angle, eq (degrees)
         * 3 : Sign, s
         */

        return -params[0] * params[1] * params[3] * sin(params[1] * theta - DissolveMath::toRadians(params[2])) / sin(theta);
    }
    else if (angleForm == AngleFunctions::Form::Cos2)
    {
        /*
         * dU/dTheta = -k * (c1 * sin(theta) + 2 * c2 * sin(2*theta))
         *
         * Parameters:
         * 0 : Force constant, k
         * 1 : Constant C0 (unused)
         * 2 : Constant C1
         * 3 : Constant C2
         */

        return -params[0] * (params[2] * sin(theta) + 2.0 * params[3] * sin(2.0 * theta)) / sin(theta);
    }

    Messenger::exception("Angle functional form '{}' not accounted for, so can't calculate force.\n",
                         AngleFunctions::forms().keyword(angleForm));
}

/*
 * Serialisation
 */

// Express as a serialisable value
void SpeciesAngle::serialise(std::string tag, SerialisedValue &target) const
{
    SpeciesIntra<SpeciesAngle, AngleFunctions>::serialise(tag, target);
    auto &angle = target[tag];
    if (i_ != nullptr)
        angle["i"] = i_->userIndex();
    if (j_ != nullptr)
        angle["j"] = j_->userIndex();
    if (k_ != nullptr)
        angle["k"] = k_->userIndex();
}
// Read values from a serialisable value
void SpeciesAngle::deserialise(const SerialisedValue &node)
{
    SpeciesIntra<SpeciesAngle, AngleFunctions>::deserialise(node, [&](auto &form) { return parent_->getCommonAngle(form); });
}
