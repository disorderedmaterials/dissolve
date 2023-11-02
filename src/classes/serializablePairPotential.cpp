// SPDX-License-Identifier: GPL-3.0-or-later

// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/serializablePairPotential.h"

SerializablePairPotential::SerializablePairPotential(double &range, double &delta, bool &source, bool &forceCharge,
                                                     bool &autoCharge, std::vector<std::shared_ptr<AtomType>> &types)
    : range_(range), delta_(delta), atomTypeChargeSource_(source), atomTypes_(types), forceCharge_(forceCharge),
      autoCharge_(autoCharge), coulombTruncationScheme_(PairPotential::coulombTruncationScheme_),
      shortRangeTruncationScheme_(PairPotential::shortRangeTruncationScheme_){};

double &SerializablePairPotential::range() { return range_; }
const double &SerializablePairPotential::range() const { return range_; }
double &SerializablePairPotential::delta() { return delta_; }
const double &SerializablePairPotential::delta() const { return delta_; }
bool &SerializablePairPotential::atomTypeChargeSource() { return atomTypeChargeSource_; }
const bool &SerializablePairPotential::atomTypeChargeSource() const { return atomTypeChargeSource_; }

std::vector<std::shared_ptr<AtomType>> &SerializablePairPotential::atomTypes() { return atomTypes_; }
const std::vector<std::shared_ptr<AtomType>> &SerializablePairPotential::atomTypes() const { return atomTypes_; }

PairPotential::CoulombTruncationScheme &SerializablePairPotential::coulombTruncationScheme()
{
    return coulombTruncationScheme_;
}
const PairPotential::CoulombTruncationScheme &SerializablePairPotential::coulombTruncationScheme() const
{
    return coulombTruncationScheme_;
}
PairPotential::ShortRangeTruncationScheme &SerializablePairPotential::shortRangeTruncationScheme()
{
    return shortRangeTruncationScheme_;
}
const PairPotential::ShortRangeTruncationScheme &SerializablePairPotential::shortRangeTruncationScheme() const
{
    return shortRangeTruncationScheme_;
}

// Express as a serialisable value
SerialisedValue SerializablePairPotential::serialise() const
{
    SerialisedValue pairPotentials = {
        {"range", range_},
        {"delta", delta_},
        {"autoChargeSource", autoCharge_},
        {"coulombTruncation", PairPotential::coulombTruncationSchemes().serialise(coulombTruncationScheme_)},
        {"shortRangeTruncation", PairPotential::shortRangeTruncationSchemes().serialise(shortRangeTruncationScheme_)}};
    if (!autoCharge_)
        pairPotentials["autoChargeSource"] = false;
    if (forceCharge_)
        pairPotentials["forceChargeSource"] = true;
    if (atomTypeChargeSource_)
        pairPotentials["includeCoulomb"] = true;
    for (auto &atomType : atomTypes_)
        pairPotentials["atomTypes"][atomType->name().data()] = atomType->serialise();
    return pairPotentials;
}

// This method populates the object's members with values read from a 'pairPotentials' TOML node
void SerializablePairPotential::deserialise(const SerialisedValue &node)
{
    range_ = toml::find_or<double>(node, "range", 15.0);
    delta_ = toml::find_or<double>(node, "delta", 0.005);
    atomTypeChargeSource_ = toml::find_or<bool>(node, "includeCoulomb", false);
    forceCharge_ = toml::find_or<bool>(node, "forceChargeSource", false);
    autoCharge_ = toml::find_or<bool>(node, "autoChargeSource", true);

    coulombTruncationScheme_ =
        PairPotential::coulombTruncationSchemes().deserialise(toml::find_or<std::string>(node, "coulombTruncation", "Shifted"));
    shortRangeTruncationScheme_ = PairPotential::shortRangeTruncationSchemes().deserialise(
        toml::find_or<std::string>(node, "shortRangeTruncation", "Shifted"));

    toMap(node, "atomTypes",
          [this](const auto &name, const auto &data)
          { atomTypes_.emplace_back(std::make_unique<AtomType>(name))->deserialise(data); });
}
