/*
    *** SpeciesImproper Definition
    *** src/classes/speciesimproper.cpp
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "classes/speciesimproper.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "classes/speciesatom.h"
#include "templates/enumhelpers.h"

SpeciesImproper::SpeciesImproper() : SpeciesIntra() { clear(); }

SpeciesImproper::SpeciesImproper(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l)
    : SpeciesIntra(), i_(i), j_(j), k_(k), l_(l)
{
}

SpeciesImproper::SpeciesImproper(SpeciesImproper &&source) : SpeciesIntra(source)
{
    i_ = source.i_;
    j_ = source.j_;
    k_ = source.k_;
    l_ = source.l_;
    form_ = source.form_;

    source.i_ = nullptr;
    source.j_ = nullptr;
    source.k_ = nullptr;
    source.l_ = nullptr;
}

/*
 * DynamicArrayObject Virtuals
 */

// Clear object, ready for re-use
void SpeciesImproper::clear()
{
    parent_ = nullptr;
    i_ = nullptr;
    j_ = nullptr;
    k_ = nullptr;
    l_ = nullptr;
    form_ = SpeciesImproper::NoForm;
}

/*
 * Atom Information
 */

// Set Atoms involved in Improper
void SpeciesImproper::assign(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l)
{
    i_ = i;
    j_ = j;
    k_ = k;
    l_ = l;
#ifdef CHECKS
    if (i_ == nullptr)
        Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* i in SpeciesImproper::set().\n");
    if (j_ == nullptr)
        Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* j in SpeciesImproper::set().\n");
    if (k_ == nullptr)
        Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* k in SpeciesImproper::set().\n");
    if (l_ == nullptr)
        Messenger::error("NULL_POINTER - NULL pointer passed for SpeciesAtom* l in SpeciesImproper::set().\n");
#endif

    if (i_)
        i_->addImproper(*this);
    if (j_)
        j_->addImproper(*this);
    if (k_)
        k_->addImproper(*this);
    if (l_)
        l_->addImproper(*this);
}

// Return first SpeciesAtom
SpeciesAtom *SpeciesImproper::i() const { return i_; }

// Return second SpeciesAtom
SpeciesAtom *SpeciesImproper::j() const { return j_; }

// Return third SpeciesAtom
SpeciesAtom *SpeciesImproper::k() const { return k_; }

// Return fourth SpeciesAtom
SpeciesAtom *SpeciesImproper::l() const { return l_; }

// Return whether the improper uses the specified SpeciesAtom
bool SpeciesImproper::uses(SpeciesAtom *spAtom) const
{
    return ((i_ == spAtom) || (j_ == spAtom) || (k_ == spAtom) || (l_ == spAtom));
}

// Return index (in parent Species) of first SpeciesAtom
int SpeciesImproper::indexI() const
{
#ifdef CHECKS
    if (i_ == nullptr)
    {
        Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'i' found in SpeciesImproper::indexI(). Returning 0...\n");
        return 0;
    }
#endif
    return i_->index();
}

// Return index (in parent Species) of second (central) SpeciesAtom
int SpeciesImproper::indexJ() const
{
#ifdef CHECKS
    if (j_ == nullptr)
    {
        Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'j' found in SpeciesImproper::indexJ(). Returning 0...\n");
        return 0;
    }
#endif
    return j_->index();
}

// Return index (in parent Species) of third SpeciesAtom
int SpeciesImproper::indexK() const
{
#ifdef CHECKS
    if (k_ == nullptr)
    {
        Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'k' found in SpeciesImproper::indexK(). Returning 0...\n");
        return 0;
    }
#endif
    return k_->index();
}

// Return index (in parent Species) of fourth SpeciesAtom
int SpeciesImproper::indexL() const
{
#ifdef CHECKS
    if (l_ == nullptr)
    {
        Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'l' found in SpeciesImproper::indexL(). Returning 0...\n");
        return 0;
    }
#endif
    return l_->index();
}

// Return index (in parent Species) of nth SpeciesAtom in interaction
int SpeciesImproper::index(int n) const
{
    if (n == 0)
        return indexI();
    else if (n == 1)
        return indexJ();
    else if (n == 2)
        return indexK();
    else if (n == 3)
        return indexL();

    Messenger::error("SpeciesAtom index {} is out of range in SpeciesImproper::index(int). Returning 0...\n");
    return 0;
}

// Return whether Atoms in Improper match those specified
bool SpeciesImproper::matches(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l) const
{
    if (i_ == i)
    {
        if (j_ != j)
            return false;
        if (k_ != k)
            return false;
        if (l_ == l)
            return true;
    }
    else if (i_ == l)
    {
        if (j_ != k)
            return false;
        if (k_ != j)
            return false;
        if (l_ == i)
            return true;
    }

    return false;
}

// Return whether all atoms in the interaction are currently selected
bool SpeciesImproper::isSelected() const
{
#ifdef CHECKS
    if (i_ == nullptr || j_ == nullptr || k_ == nullptr || l_ == nullptr)
    {
        Messenger::error(
            "NULL_POINTER - NULL SpeciesAtom pointer found in SpeciesImproper::isSelected(). Returning false...\n");
        return false;
    }
#endif
    return (i_->isSelected() && j_->isSelected() && k_->isSelected() && l_->isSelected());
}

/*
 * Interaction Parameters
 */

// Return enum options for ImproperFunction
EnumOptions<SpeciesImproper::ImproperFunction> SpeciesImproper::improperFunctions()
{
    static EnumOptionsList ImproperFunctionOptions = EnumOptionsList() << EnumOption(SpeciesImproper::CosineForm, "Cos", 4, 4);

    static EnumOptions<SpeciesImproper::ImproperFunction> options("ImproperFunction", ImproperFunctionOptions);

    return options;
}

// Set up any necessary parameters
void SpeciesImproper::setUp() {}

// Calculate and return fundamental frequency for the interaction
double SpeciesImproper::fundamentalFrequency(double reducedMass) const
{
    Messenger::warn("No fundamental frequency can be calculated for this improper interaction.\n");
    return 0.0;
}

// Return type of this interaction
SpeciesIntra::InteractionType SpeciesImproper::type() const { return SpeciesIntra::ImproperInteraction; }

// Return energy for specified angle
double SpeciesImproper::energy(double angleInDegrees) const
{
    // Get pointer to relevant parameters array
    const auto &params = parameters();

    // Convert torsion angle from degrees to radians
    double phi = angleInDegrees / DEGRAD;

    if (form() == SpeciesImproper::NoForm)
        return 0.0;
    else if (form() == SpeciesImproper::CosineForm)
    {
        /*
         * U(phi) = k * (1 + s*cos(n*phi - eq))
         *
         * Parameters:
         * 0 : force constant k
         * 1 : Period 'n'
         * 2 : equilibrium angle (degrees)
         * 3 : Sign 's'
         */
        return params[0] * (1.0 + params[3] * cos(params[1] * phi - (params[2] / DEGRAD)));
    }

    Messenger::error("Functional form of SpeciesImproper term not accounted for, so can't calculate energy.\n");
    return 0.0;
}

// Return force multiplier for specified angle
double SpeciesImproper::force(double angleInDegrees) const
{
    // Get pointer to relevant parameters array
    const auto &params = parameters();

    // Convert torsion angle from degrees to radians, and calculate derivative w.r.t. change in torsion angle
    double phi = angleInDegrees / DEGRAD;
    double dphi_dcosphi = (phi < 1E-8 ? 0.0 : -1.0 / sin(phi));

    if (form() == SpeciesImproper::NoForm)
        return 0.0;
    else if (form() == SpeciesImproper::CosineForm)
    {
        /*
         * dU/dphi = k * n * s * -sin(n*phi - eq)
         *
         * Parameters:
         * 0 : Force constant 'k'
         * 1 : Period 'n'
         * 2 : Equilibrium angle (degrees)
         * 3 : Sign 's'
         */
        return dphi_dcosphi * params[1] * params[0] * params[3] * -sin(params[1] * phi - (params[2] / DEGRAD));
    }

    Messenger::error("Functional form of SpeciesImproper term not accounted for, so can't calculate force.\n");
    return 0.0;
}
