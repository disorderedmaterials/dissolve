// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/pairPotentialOverride.h"
#include "classes/atomType.h"

PairPotentialOverride::PairPotentialOverride() : interactionPotential_(ShortRangeFunctions::Form::None) {}

PairPotentialOverride::PairPotentialOverride(std::string_view matchI, std::string_view matchJ,
                                             PairPotentialOverride::PairPotentialOverrideType overrideType,
                                             const InteractionPotential<ShortRangeFunctions> &potential)
    : matchI_(matchI), matchJ_(matchJ), type_(overrideType), interactionPotential_(potential)
{
}

// Return enum options for
EnumOptions<PairPotentialOverride::PairPotentialOverrideType> PairPotentialOverride::pairPotentialOverrideTypes()
{
    return EnumOptions<PairPotentialOverride::PairPotentialOverrideType>("PairPotentialOverrideType",
                                                                         {{PairPotentialOverride::Off, "Off"},
                                                                          {PairPotentialOverride::Add, "Add"},
                                                                          {PairPotentialOverride::Replace, "Replace"}});
}

// Set first AtomType name to match
void PairPotentialOverride::setMatchI(std::string_view match) { matchI_ = match; }

// Return first AtomType name to match
std::string_view PairPotentialOverride::matchI() const { return matchI_; }

// Set second AtomType name to match
void PairPotentialOverride::setMatchJ(std::string_view match) { matchJ_ = match; }

// Return second AtomType name to match
std::string_view PairPotentialOverride::matchJ() const { return matchJ_; }

// Set override type
void PairPotentialOverride::setType(PairPotentialOverrideType overrideType) { type_ = overrideType; }

// Return override type
PairPotentialOverride::PairPotentialOverrideType PairPotentialOverride::type() const { return type_; }

// Return interaction potential
InteractionPotential<ShortRangeFunctions> &PairPotentialOverride::interactionPotential() { return interactionPotential_; }
const InteractionPotential<ShortRangeFunctions> &PairPotentialOverride::interactionPotential() const
{
    return interactionPotential_;
}

/*
 * I/O
 */

// Express as a serialisable value
SerialisedValue PairPotentialOverride::serialise() const
{
    SerialisedValue value;

    value["matchI"] = matchI_;
    value["matchJ"] = matchJ_;
    value["type"] = pairPotentialOverrideTypes().keyword(type_);

    value["form"] = ShortRangeFunctions::forms().keyword(interactionPotential_.form());
    auto &values = interactionPotential().parameters();
    if (!values.empty())
    {
        SerialisedValue interactionParameters;
        auto &parameters = ShortRangeFunctions::parameters(interactionPotential_.form());
        for (auto &&[parameter, value] : zip(parameters, values))
            interactionParameters[parameter] = value;
        value["parameters"] = interactionParameters;
    }

    return value;
}

// Read values from a serialisable value
void PairPotentialOverride::deserialise(const SerialisedValue &node)
{
    matchI_ = toml::find<std::string>(node, "matchI");
    matchJ_ = toml::find<std::string>(node, "matchJ");

    Serialisable::optionalOn(node, "type",
                             [this](const auto node)
                             { type_ = pairPotentialOverrideTypes().enumeration(std::string(node.as_string())); });

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
