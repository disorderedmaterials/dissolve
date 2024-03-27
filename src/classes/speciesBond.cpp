// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/speciesBond.h"
#include "base/sysFunc.h"
#include "classes/coreData.h"
#include "classes/speciesAtom.h"
#include "data/atomicMasses.h"
#include <map>

// Return enum options for BondFunction
EnumOptions<BondFunctions::Form> BondFunctions::forms()
{
    return EnumOptions<BondFunctions::Form>("BondFunction", {{BondFunctions::Form::None, "None"},
                                                             {BondFunctions::Form::Harmonic, "Harmonic", 2},
                                                             {BondFunctions::Form::EPSR, "EPSR", 2},
                                                             {BondFunctions::Form::Morse, "Morse", 3}});
}

// Return parameters for specified form
const std::vector<std::string> &BondFunctions::parameters(Form form)
{
    static std::map<BondFunctions::Form, std::vector<std::string>> params_ = {
        {BondFunctions::Form::None, {}},
        {BondFunctions::Form::Harmonic, {"k", "eq"}},
        {BondFunctions::Form::EPSR, {"C/2", "eq"}},
        {BondFunctions::Form::Morse, {"D", "alpha", "eq"}}};
    return params_[form];
}

// Return nth parameter for the given form
std::string BondFunctions::parameter(Form form, int n) { return DissolveSys::stringAt(parameters(form), n); }

// Return index of parameter in the given form
std::optional<int> BondFunctions::parameterIndex(Form form, std::string_view name)
{
    return DissolveSys::indexOf(parameters(form), name);
}

SpeciesBond::SpeciesBond() : SpeciesIntra(BondFunctions::Form::None) {}

SpeciesBond::SpeciesBond(SpeciesAtom *i, SpeciesAtom *j) : SpeciesIntra(BondFunctions::Form::None) { assign(i, j); }

SpeciesBond::SpeciesBond(SpeciesBond &source) : SpeciesIntra(source) { this->operator=(source); }

SpeciesBond::SpeciesBond(SpeciesBond &&source) noexcept : SpeciesIntra(source)
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
    interactionPotential_ = source.interactionPotential_;
    masterTerm_ = source.masterTerm_;

    // Reset source data
    source.i_ = nullptr;
    source.j_ = nullptr;
}

SpeciesBond &SpeciesBond::operator=(const SpeciesBond &source)
{
    // Copy data
    assign(source.i_, source.j_);
    bondType_ = source.bondType_;
    interactionPotential_ = source.interactionPotential_;
    masterTerm_ = source.masterTerm_;
    SpeciesIntra::operator=(source);

    return *this;
}

SpeciesBond &SpeciesBond::operator=(SpeciesBond &&source) noexcept
{
    // Detach any current atoms
    if (i_ && j_)
        detach();

    // Copy data
    assign(source.i_, source.j_);
    bondType_ = source.bondType_;
    interactionPotential_ = source.interactionPotential_;
    masterTerm_ = source.masterTerm_;
    SpeciesIntra::operator=(source);

    // Clean source
    source.detach();

    return *this;
}

/*
 * SpeciesAtom Information
 */

// Rewrite SpeciesAtom pointer
void SpeciesBond::switchAtom(const SpeciesAtom *oldPtr, SpeciesAtom *newPtr)
{
    assert(i_ == oldPtr || j_ == oldPtr);

    if (i_ == oldPtr)
        i_ = newPtr;
    else
        j_ = newPtr;
}

// Assign the two atoms in the bond
void SpeciesBond::assign(SpeciesAtom *i, SpeciesAtom *j)
{
    i_ = i;
    j_ = j;
    assert(i_ && j_);

    // Add ourself to the list of bonds on each atom
    i_->addBond(*this);
    j_->addBond(*this);
}

// Return first SpeciesAtom involved in interaction
SpeciesAtom *SpeciesBond::i() const { return i_; }

// Return second SpeciesAtom involved in SpeciesBond
SpeciesAtom *SpeciesBond::j() const { return j_; }

// Return vector of involved atoms
std::vector<const SpeciesAtom *> SpeciesBond::atoms() const { return {i_, j_}; }

// Return the 'other' SpeciesAtom in the SpeciesBond
SpeciesAtom *SpeciesBond::partner(const SpeciesAtom *i) const { return (i == i_ ? j_ : i_); }

// Return index (in parent Species) of first SpeciesAtom
int SpeciesBond::indexI() const
{
    assert(i_);
    return i_->index();
}

// Return index (in parent Species) of second SpeciesAtom
int SpeciesBond::indexJ() const
{
    assert(j_);
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
    return (i_ == i && j_ == j) || (i_ == j && j_ == i);
}

// Return whether all atoms in the interaction are currently selected
bool SpeciesBond::isSelected() const
{
    assert(i_ && j_);
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
    for (auto n = 0; n < SpeciesBond::nBondTypes; ++n)
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

    throw(std::runtime_error(fmt::format("Bond functional form '{}' not accounted for, so can't calculate energy.\n",
                                         BondFunctions::forms().keyword(bondForm))));
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

    throw(std::runtime_error(fmt::format("Bond functional form '{}' not accounted for, so can't calculate force.\n",
                                         BondFunctions::forms().keyword(bondForm))));
}

// Express as a serialisable value
SerialisedValue SpeciesBond::serialise() const
{
    auto bond = SpeciesIntra<SpeciesBond, BondFunctions>::serialise();
    if (i_ != nullptr)
        bond["i"] = i_->userIndex();
    if (j_ != nullptr)
        bond["j"] = j_->userIndex();

    return bond;
}

// This method populates the object's members with values read from a 'bond' TOML node
void SpeciesBond::deserialise(const SerialisedValue &node, CoreData &coreData)
{
    deserialiseForm(node, [&coreData](auto &form) { return coreData.getMasterBond(form); });
}
