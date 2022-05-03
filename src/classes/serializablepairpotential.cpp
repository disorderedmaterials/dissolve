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

toml::basic_value<toml::discard_comments, std::map, std::vector> SerializablePairPotential::serialize()
{
    toml::basic_value<toml::discard_comments, std::map, std::vector> pairPotentials;
    pairPotentials["range"] = range_;
    pairPotentials["delta"] = delta_;
    pairPotentials["includeCoulomb"] = atomTypeChargeSource_;
    pairPotentials["coulombTruncation"] = PairPotential::coulombTruncationSchemes().keyword(coulombTruncationScheme_);
    pairPotentials["shortRangeTruncation"] = PairPotential::shortRangeTruncationSchemes().keyword(shortRangeTruncationScheme_);
    if (!atomTypes_.empty())
        for (auto &atomType : atomTypes_)
            pairPotentials[atomType->name().data()] = atomType->serialize();
    return pairPotentials;
}

void SerializablePairPotential::deserialize(toml::value node)
{
    range_ = node["range"].as_floating();
    delta_ = node["delta"].as_floating();
    atomTypeChargeSource_ = node["includeCoulomb"].as_boolean();
    coulombTruncationScheme_ =
        PairPotential::coulombTruncationSchemes().enumeration(std::string(node["coulombTruncation"].as_string()));
    shortRangeTruncationScheme_ =
        PairPotential::shortRangeTruncationSchemes().enumeration(std::string(node["shortRangeTruncation"].as_string()));

    toml::value file = toml::parse("C:/ProjectDissolve/Dissolve/build/Release/input.toml");
    PairPotential::setCoulombTruncationScheme(
        PairPotential::coulombTruncationSchemes().enumeration(std::string(file["coulombTruncation"].as_string())));
    PairPotential::setShortRangeTruncationScheme(
        PairPotential::shortRangeTruncationSchemes().enumeration(std::string(file["shortRangeTruncation"].as_string())));
    //std::vector tomlAtomTypes = file["pairPotentials"].as_array();
    //for (auto atomType : tomlAtomTypes)
    //    addAtomType(Elements::element(std::string(atomType["z"].as_string())));
}
