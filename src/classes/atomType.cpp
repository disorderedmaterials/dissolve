// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/atomType.h"
#include "base/serialiserLibrary.h"
#include "data/elements.h"
#include "templates/algorithms.h"
#include <cmath>

AtomType::AtomType(Elements::Element Z) : Z_(Z), interactionPotential_(ShortRangeFunctions::Form::Undefined) {}
AtomType::AtomType(std::string_view name) : name_(name), interactionPotential_(ShortRangeFunctions::Form::Undefined) {}
AtomType::AtomType(Elements::Element Z, std::string_view name)
    : Z_(Z), name_(name), interactionPotential_(ShortRangeFunctions::Form::Undefined)
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

// Set whether the atom type is exchangeable
void AtomType::setExchangeable(bool exchangeable) { exchangeable_ = exchangeable; }

// Return whether the atom type is exchangeable
bool AtomType::isExchangeable() const { return exchangeable_; }

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

// Return whether our parameters are the same as those provided
bool AtomType::sameParametersAs(const AtomType *other, bool checkCharge) const
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
void AtomType::serialise(std::string tag, SerialisedValue &target) const
{
    auto &atomType = target[tag];

    atomType["z"] = Serialisable::ser(Elements::symbol(Z_));
    atomType["charge"] = charge_;
    atomType["form"] = ShortRangeFunctions::forms().keyword(interactionPotential_.form());
    atomType["exchangeable"] = exchangeable_;

    auto &values = interactionPotential().parameters();
    if (!values.empty())
    {
        SerialisedValue atomTypeParameters;
        auto &parameters = ShortRangeFunctions::parameters(interactionPotential_.form());
        for (auto &&[parameter, value] : zip(parameters, values))
            atomTypeParameters[parameter] = value;
        atomType["parameters"] = atomTypeParameters;
    }
}

// Read values from a serialisable value
void AtomType::deserialise(SerialisedValue node)
{
    Z_ = Elements::element(Deserialisable::deser<std::string>(node.at("z")));
    charge_ = Deserialisable::deser_or<double>(node, "charge", 0.0);
    exchangeable_ = Deserialisable::deser_or<bool>(node, "exchangeable", false);

    Deserialisable::optionalOn(
        node, "form", [this](const auto node)
        { interactionPotential_.setForm(ShortRangeFunctions::forms().enumeration(std::string(node.as_string()))); });

    Deserialisable::optionalOn(node, "parameters",
                               [this](const auto node)
                               {
                                   auto &parameters = ShortRangeFunctions::parameters(interactionPotential_.form());
                                   std::vector<double> values;
                                   std::transform(parameters.begin(), parameters.end(), std::back_inserter(values),
                                                  [&node](const auto parameter) { return node.at(parameter).as_floating(); });
                                   interactionPotential_.setFormAndParameters(interactionPotential_.form(), values);
                               });
}
