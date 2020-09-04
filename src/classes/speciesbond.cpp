/*
    *** SpeciesBond Definition
    *** src/classes/speciesbond.cpp
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

#include "classes/speciesbond.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "classes/speciesatom.h"
#include "data/atomicmass.h"
#include "templates/enumhelpers.h"

SpeciesBond::SpeciesBond(SpeciesAtom *i, SpeciesAtom *j) : SpeciesIntra()
{
    assign(i, j);
    bondType_ = SpeciesBond::SingleBond;
    form_ = SpeciesBond::NoForm;
}

SpeciesBond::SpeciesBond(SpeciesBond &source) : SpeciesIntra(source) { this->operator=(source); }

void SpeciesBond::assign(SpeciesAtom *i, SpeciesAtom *j)
{
    i_ = i;
    j_ = j;

    // Add ourself to the list of bonds on each atom
    if (i_ && j_)
    {
        i_->addBond(*this);
        j_->addBond(*this);
    }
}

SpeciesBond::SpeciesBond(SpeciesBond &&source) : SpeciesIntra(source)
{
    // Detach source bond referred to by the species atoms
    if (source.i_ && source.j_)
    {
        source.i_->removeBond(source);
        source.j_->removeBond(source);
    }

    // Copy data
    assign(source.i_, source.j_);
    bondType_ = source.bondType_;
    form_ = source.form_;

    // Reset source data
    source.i_ = nullptr;
    source.j_ = nullptr;
}

SpeciesBond &SpeciesBond::operator=(const SpeciesBond &source)
{
    // Copy data
    i_ = source.i_;
    j_ = source.j_;
    if (i_ && j_)
    {
        i_->addBond(*this);
        j_->addBond(*this);
    }
    bondType_ = source.bondType_;
    form_ = source.form_;
    SpeciesIntra::operator=(source);

    return *this;
}

SpeciesBond &SpeciesBond::operator=(SpeciesBond &&source)
{
    // Detach any current atoms
    if (i_ && j_)
        detach();

    // Copy data
    assign(source.i_, source.j_);
    bondType_ = source.bondType_;
    form_ = source.form_;
    SpeciesIntra::operator=(source);

    // Clean source
    source.detach();

    return *this;
}

/*
 * SpeciesAtom Information
 */

// Return first SpeciesAtom involved in interaction
SpeciesAtom *SpeciesBond::i() const { return i_; }

// Return second SpeciesAtom involved in SpeciesBond
SpeciesAtom *SpeciesBond::j() const { return j_; }

// Return the 'other' SpeciesAtom in the SpeciesBond
SpeciesAtom *SpeciesBond::partner(const SpeciesAtom *i) const { return (i == i_ ? j_ : i_); }

// Return index (in parent Species) of first SpeciesAtom
int SpeciesBond::indexI() const
{
#ifdef CHECKS
    if (i_ == nullptr)
    {
        Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'i' found in SpeciesBond::indexI(). Returning 0...\n");
        return 0;
    }
#endif
    return i_->index();
}

// Return index (in parent Species) of second SpeciesAtom
int SpeciesBond::indexJ() const
{
#ifdef CHECKS
    if (j_ == nullptr)
    {
        Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer 'j' found in SpeciesBond::indexJ(). Returning 0...\n");
        return 0;
    }
#endif
    return j_->index();
}

// Return index (in parent Species) of nth SpeciesAtom in interaction
int SpeciesBond::index(int n) const
{
    if (n == 0)
        return indexI();
    else if (n == 1)
        return indexJ();

    Messenger::error("SpeciesAtom index {} is out of range in SpeciesBond::index(int). Returning 0...\n");
    return 0;
}

// Return whether SpeciesAtoms in Angle match those specified
bool SpeciesBond::matches(const SpeciesAtom *i, const SpeciesAtom *j) const
{
    if ((i_ == i) && (j_ == j))
        return true;
    if ((i_ == j) && (j_ == i))
        return true;
    return false;
}

// Return whether all atoms in the interaction are currently selected
bool SpeciesBond::isSelected() const
{
#ifdef CHECKS
    if (i_ == nullptr || j_ == nullptr)
    {
        Messenger::error("NULL_POINTER - NULL SpeciesAtom pointer found in SpeciesBond::isSelected(). Returning false...\n");
        return false;
    }
#endif
    return (i_->isSelected() && j_->isSelected());
}

// Detach from current atoms
void SpeciesBond::detach()
{
    if (i_ && j_)
    {
        i_->removeBond(*this);
        j_->removeBond(*this);
    }
    i_ = nullptr;
    j_ = nullptr;
}

/*
 * Bond Type
 */

// Bond type keywords
std::string_view BondTypeKeywords[] = {"Single", "Double", "Triple", "Quadruple", "Aromatic"};
double BondTypeOrders[] = {1.0, 2.0, 3.0, 4.0, 1.5};

// Convert bond type string to functional form
SpeciesBond::BondType SpeciesBond::bondType(std::string_view s)
{
    for (int n = 0; n < SpeciesBond::nBondTypes; ++n)
        if (DissolveSys::sameString(s, BondTypeKeywords[n]))
            return (SpeciesBond::BondType)n;
    return SpeciesBond::nBondTypes;
}

// Return bond type functional form text
std::string_view SpeciesBond::bondType(SpeciesBond::BondType bt) { return BondTypeKeywords[bt]; }

// Return bond order for specified bond type
double SpeciesBond::bondOrder(SpeciesBond::BondType bt) { return BondTypeOrders[bt]; }

// Set bond type
void SpeciesBond::setBondType(BondType type) { bondType_ = type; }

// Return bond type
SpeciesBond::BondType SpeciesBond::bondType() const { return bondType_; }

// Return bond order for current bond type
double SpeciesBond::bondOrder() const { return SpeciesBond::bondOrder(bondType_); }

/*
 * Interaction Parameters
 */

// Return enum options for BondFunction
EnumOptions<SpeciesBond::BondFunction> SpeciesBond::bondFunctions()
{
    static EnumOptionsList BondFunctionOptions = EnumOptionsList() << EnumOption(SpeciesBond::NoForm, "None", 0, 0)
                                                                   << EnumOption(SpeciesBond::HarmonicForm, "Harmonic", 2, 2)
                                                                   << EnumOption(SpeciesBond::EPSRForm, "EPSR", 2, 2);

    static EnumOptions<SpeciesBond::BondFunction> options("BondFunction", BondFunctionOptions);

    return options;
}

// Set up any necessary parameters
void SpeciesBond::setUp()
{
    // Get pointer to relevant parameters array
    const auto &params = parameters();

    /*
     * Depending on the form, we may have other dependent parameters to set up
     * These are always stored in the *local* SpeciesIntra array, rather than those in any associated master parameters
     * This way, we can reference both general master parameters as well as others which depend on the atoms involved, for
     * instance
     */
    if (form() == SpeciesBond::EPSRForm)
    {
        // Work out omega-squared(ab) from mass of natural isotopes
        double massI = AtomicMass::mass(i_->element());
        double massJ = AtomicMass::mass(j_->element());
        parameters_[2] = params[1] / sqrt((massI + massJ) / (massI * massJ));
    }
}

// Return fundamental frequency for the interaction
double SpeciesBond::fundamentalFrequency(double reducedMass) const
{
    // Get pointer to relevant parameters array
    const auto &params = parameters();

    double k = 0.0;
    if (form() == SpeciesBond::HarmonicForm)
        k = params[0];
    else if (form() == SpeciesBond::EPSRForm)
        k = params[0];
    else
    {
        Messenger::error("Functional form of SpeciesBond term not set, or no force constant available, so can't "
                         "determine fundamental frequency.\n");
        return 0.0;
    }

    // Convert force constant from (assumed) kJ mol-1 A-2 into J m-2 (kg s-2)
    k *= 1000.0 * 1.0e20 / AVOGADRO;

    // Convert reduced mass from amu to kg
    double mu = reducedMass / (AVOGADRO * 1000.0);

    // Calculate fundamental frequency
    double v = (1.0 / TWOPI) * sqrt(k / mu);

    return v;
}

// Return type of this interaction
SpeciesIntra::InteractionType SpeciesBond::type() const { return SpeciesIntra::BondInteraction; }

// Return energy for specified distance
double SpeciesBond::energy(double distance) const
{
    // Get pointer to relevant parameters array
    const auto &params = parameters();

    if (form() == SpeciesBond::NoForm)
        return 0.0;
    else if (form() == SpeciesBond::HarmonicForm)
    {
        /*
         * Parameters:
         * 0 : force constant
         * 1 : equilibrium distance
         */
        double delta = distance - params[1];
        return 0.5 * params[0] * delta * delta;
    }
    else if (form() == SpeciesBond::EPSRForm)
    {
        /*
         * Basically a harmonic oscillator metered by the mass of the atoms (encapsulated in the omegaSquared parameter
         *
         * Parameters:
         * 0 : general force constant C / 2.0
         * 1 : equilibrium distance
         * 2 : omega squared (LOCAL parameter)
         */
        double delta = distance - params[1];
        return params[0] * (delta * delta) / parameters_[2];
    }

    Messenger::error("Functional form of SpeciesBond term not accounted for, so can't calculate energy.\n");
    return 0.0;
}

// Return force multiplier for specified distance
double SpeciesBond::force(double distance) const
{
    // Get pointer to relevant parameters array
    const auto &params = parameters();

    if (form() == SpeciesBond::NoForm)
        return 0.0;
    else if (form() == SpeciesBond::HarmonicForm)
    {
        /*
         * Parameters:
         * 0 : force constant
         * 1 : equilibrium distance
         */
        return -params[0] * (distance - params[1]);
    }
    else if (form() == SpeciesBond::EPSRForm)
    {
        /*
         * Basically a harmonic oscillator metered by the mass of the atoms (encapsulated in the omegaSquared parameter
         *
         * Parameters:
         * 0 : general force constant C / 2.0
         * 1 : equilibrium distance
         * 2 : omega squared (LOCAL parameter)
         */
        return -2.0 * params[0] * (distance - params[1]);
    }

    Messenger::error("Functional form of SpeciesBond term not accounted for, so can't calculate force.\n");
    return 0.0;
}
