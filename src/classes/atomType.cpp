// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/atomType.h"
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
std::string ShortRangeFunctions::parameter(Form form, int n) { return DissolveSys::stringAt(parameters(form), n); }

// Return index of parameter in the given form
std::optional<int> ShortRangeFunctions::parameterIndex(Form form, std::string_view name)
{
    return DissolveSys::indexOf(parameters(form), name);
}

AtomType::AtomType(Elements::Element Z) : Z_(Z), interactionPotential_(ShortRangeFunctions::Form::None) {}
AtomType::AtomType(std::string_view name) : name_(name), interactionPotential_(ShortRangeFunctions::Form::None) {}
AtomType::AtomType(Elements::Element Z, std::string_view name)
    : Z_(Z), name_(name), interactionPotential_(ShortRangeFunctions::Form::None)
{
}

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

// Express as a serialisable value
SerialisedValue AtomType::serialise() const
{
    SerialisedValue atomType;

    atomType["z"] = Z_;
    atomType["charge"] = charge_;
    atomType["form"] = ShortRangeFunctions::forms().keyword(interactionPotential_.form());

    auto &values = interactionPotential().parameters();
    if (!values.empty())
    {
        SerialisedValue atomTypeParameters;
        auto &parameters = ShortRangeFunctions::parameters(interactionPotential_.form());
        for (auto &&[parameter, value] : zip(parameters, values))
            atomTypeParameters[parameter] = value;
        atomType["parameters"] = atomTypeParameters;
    }

    return atomType;
}

// Read values from a serialisable value
void AtomType::deserialise(SerialisedValue node)
{
    Z_ = toml::find<Elements::Element>(node, "z");
    charge_ = toml::find_or<double>(node, "charge", 0.0);
    Serialisable::optionalOn(
        node, "form",
        [this](const auto node)
        { interactionPotential_.setForm(ShortRangeFunctions::forms().enumeration(std::string(node.as_string()))); });

    Serialisable::optionalOn(node, "parameters",
                             [this](const auto node)
                             {
                                 auto &parameters = ShortRangeFunctions::parameters(interactionPotential_.form());
                                 std::vector<double> values;
                                 std::transform(parameters.begin(), parameters.end(), std::back_inserter(values),
                                                [&node](const auto parameter) { return node.at(parameter).as_floating(); });
                                 interactionPotential_.setFormAndParameters(interactionPotential_.form(), values);
                             });
}
