// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/speciesbond.h"
#include "base/sysfunc.h"
#include "classes/coredata.h"
#include "classes/speciesatom.h"
#include "data/atomicmasses.h"
#include <map>

// Return enum options for BondFunction
EnumOptions<BondFunctions::Form> BondFunctions::forms()
{
    return EnumOptions<BondFunctions::Form>("BondFunction", {{BondFunctions::Form::None, "None"},
                                                             {BondFunctions::Form::Harmonic, "Harmonic", 2},
                                                             {BondFunctions::Form::EPSR, "EPSR", 2}});
}

// Return parameters for specified form
const std::vector<std::string> &BondFunctions::parameters(Form form)
{
    static std::map<BondFunctions::Form, std::vector<std::string>> params_ = {{BondFunctions::Form::None, {}},
                                                                              {BondFunctions::Form::Harmonic, {"k", "eq"}},
                                                                              {BondFunctions::Form::EPSR, {"C/2", "eq"}}};
    return params_[form];
}

// Return nth parameter for the given form
std::string BondFunctions::parameter(Form form, int n)
{
    return (n < 0 || n >= parameters(form).size()) ? "" : parameters(form)[n];
}

// Return index of parameter in the given form
std::optional<int> BondFunctions::parameterIndex(Form form, std::string_view name)
{
    auto it = std::find_if(parameters(form).begin(), parameters(form).end(),
                           [name](const auto &param) { return DissolveSys::sameString(name, param); });
    if (it == parameters(form).end())
        return {};

    return it - parameters(form).begin();
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

// Return fundamental frequency for the interaction
double SpeciesBond::fundamentalFrequency(double reducedMass) const
{
    // Get pointer to relevant parameters array
    const auto &params = interactionParameters();
    const auto bondForm = interactionForm();

    double k = 0.0;
    if (bondForm == BondFunctions::Form::Harmonic)
        k = params[0];
    else if (bondForm == BondFunctions::Form::EPSR)
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

    throw(std::runtime_error(fmt::format("Bond functional form '{}' not accounted for, so can't calculate force.\n",
                                         BondFunctions::forms().keyword(bondForm))));
}

// Express as a tree node
SerialisedValue SpeciesBond::serialise() const
{
    SerialisedValue bond;
    if (i_ != nullptr)
        bond["i"] = i_->userIndex();
    if (j_ != nullptr)
        bond["j"] = j_->userIndex();

    std::string form = "@";
    if (masterTerm_ != nullptr)
        form += masterTerm_->name();
    else
        form = BondFunctions::forms().keyword(interactionForm());
    bond["form"] = form;

    std::vector<double> values = SpeciesBond::interactionPotential().parameters();
    if (!values.empty())
    {
        SerialisedValue parametersNode;
        std::vector<std::string> parameters = BondFunctions::parameters(interactionForm());
        for (int parameterIndex = 0; parameterIndex < values.size(); parameterIndex++)
            parametersNode[parameters[parameterIndex]] = values[parameterIndex];
        bond["parameters"] = parametersNode;
    }

    return bond;
}
// This method populates the object's members with values read from a 'bond' TOML node
void SpeciesBond::deserialise(SerialisedValue &node, CoreData &coreData)
{
    deserialiseForm(node, [&coreData](auto &form) { return coreData.getMasterBond(form); });
}
