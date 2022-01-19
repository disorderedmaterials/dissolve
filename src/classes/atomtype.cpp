// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/atomtype.h"
#include "data/elements.h"
#include "templates/algorithms.h"
#include <map>

// Return enum options for ShortRangeType
EnumOptions<ShortRangeFunctions::Form> ShortRangeFunctions::forms()
{
    return EnumOptions<ShortRangeFunctions::Form>("ShortRangeType",
                                                  {{ShortRangeFunctions::Form::None, "None"},
                                                   {ShortRangeFunctions::Form::LennardJones, "LJ", 2, 2},
                                                   {ShortRangeFunctions::Form::LennardJonesGeometric, "LJGeometric", 2, 2}});
}

// Return parameters for specified form
const std::vector<std::string> &ShortRangeFunctions::parameters(Form form)
{
    static std::map<ShortRangeFunctions::Form, std::vector<std::string>> params_ = {
        {ShortRangeFunctions::Form::None, {}},
        {ShortRangeFunctions::Form::LennardJones, {"epsilon", "sigma"}},
        {ShortRangeFunctions::Form::LennardJonesGeometric, {"epsilon", "sigma"}}};
    return params_[form];
}

// Return nth parameter for the given form
std::string ShortRangeFunctions::parameter(Form form, int n)
{
    return (n < 0 || n >= parameters(form).size()) ? "" : parameters(form)[n];
}

// Return index of parameter in the given form
std::optional<int> ShortRangeFunctions::parameterIndex(Form form, std::string_view name)
{
    auto it = std::find_if(parameters(form).begin(), parameters(form).end(),
                           [name](const auto &param) { return DissolveSys::sameString(name, param); });
    if (it == parameters(form).end())
        return {};

    return it - parameters(form).begin();
}

AtomType::AtomType(Elements::Element Z) : interactionPotential_(ShortRangeFunctions::Form::None), Z_(Z) {}

/*
 * Character
 */

// Set name of AtomType
void AtomType::setName(std::string_view name) { name_ = name; }

// Return name of AtomType
std::string_view AtomType::name() const { return name_; }

// Set atomic element
void AtomType::setZ(Elements::Element Z) { Z_ = Z; }

// Return atomic element
Elements::Element AtomType::Z() const { return Z_; }

/*
 * Interaction Parameters
 */

// Return short-range interaction potential
InteractionPotential<ShortRangeFunctions> &AtomType::interactionPotential() { return interactionPotential_; }
const InteractionPotential<ShortRangeFunctions> &AtomType::interactionPotential() const { return interactionPotential_; }

// Set atomic charge
void AtomType::setCharge(double q) { charge_ = q; }

// Return atomic charge
double AtomType::charge() const { return charge_; }

// Set index of this type in the main type index
void AtomType::setIndex(int id) { index_ = id; }

// Return index of this type in the main type index
int AtomType::index() const { return index_; }

// Return whether our parameters are the same as those provided
bool AtomType::sameParametersAs(const AtomType *other, bool checkCharge)
{
    if (Z_ != other->Z_ || interactionPotential_.form() != other->interactionPotential_.form() ||
        interactionPotential_.nParameters() != other->interactionPotential_.nParameters())
        return false;
    if (checkCharge && fabs(charge_ - other->charge_) > 1.0e-8)
        return false;
    for (auto &&[p1, p2] : zip(interactionPotential_.parameters(), other->interactionPotential_.parameters()))
        if (fabs(p1 - p2) > 1.0e-8)
            return false;
    return true;
}
