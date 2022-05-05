// SPDX-License-Identifier: GPL-3.0-or-later

// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/serializablepairpotential.h"

SerializablePairPotential::SerializablePairPotential(double &range, double &delta, bool &source,
                                                     std::vector<std::shared_ptr<AtomType>> &types)
    : range_(range), delta_(delta), atomTypeChargeSource_(source), atomTypes_(types),
      coulombTruncationScheme_(PairPotential::coulombTruncationScheme_),
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

// This method generates a 'pairPotentials' TOML node from the object's members
toml::basic_value<toml::discard_comments, std::map, std::vector> SerializablePairPotential::serialise()
{
    toml::basic_value<toml::discard_comments, std::map, std::vector> pairPotentials;
    pairPotentials["range"] = range_;
    pairPotentials["delta"] = delta_;
    pairPotentials["includeCoulomb"] = atomTypeChargeSource_;
    pairPotentials["coulombTruncation"] = PairPotential::coulombTruncationSchemes().keyword(coulombTruncationScheme_);
    pairPotentials["shortRangeTruncation"] = PairPotential::shortRangeTruncationSchemes().keyword(shortRangeTruncationScheme_);
    for (auto &atomType : atomTypes_)
        pairPotentials["atomTypes"][atomType->name().data()] = atomType->serialise();
    return pairPotentials;
}

// This method populates the object's members with values read from a 'pairPotentials' TOML node
void SerializablePairPotential::deserialise(toml::value node)
{
    if (node.contains("range"))
        range_ = node["range"].as_floating();
    if (node.contains("delta"))
        delta_ = node["delta"].as_floating();
    if (node.contains("includeCoulomb"))
        atomTypeChargeSource_ = node["includeCoulomb"].as_boolean();
    if (node.contains("coulombTruncation"))
        coulombTruncationScheme_ =
            PairPotential::coulombTruncationSchemes().enumeration(std::string(node["coulombTruncation"].as_string()));
    if (node.contains("shortRangeTruncation"))
        shortRangeTruncationScheme_ =
            PairPotential::shortRangeTruncationSchemes().enumeration(std::string(node["shortRangeTruncation"].as_string()));

    if (node.contains("atomTypes"))
    {
        toml::value atomTypesNode = node["atomTypes"];
        for (auto &[name, data] : atomTypesNode.as_table())
            atomTypes_.emplace_back(std::make_unique<AtomType>(name))->deserialise(data);
    }
}
