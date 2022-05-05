// SPDX-License-Identifier: GPL-3.0-or-later

// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include "pairpotential.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <toml11/toml.hpp>

class SerializablePairPotential
{
    private:
    double &range_;
    double &delta_;
    bool &atomTypeChargeSource_;

    std::vector<std::shared_ptr<AtomType>> &atomTypes_;

    PairPotential::CoulombTruncationScheme &coulombTruncationScheme_;
    PairPotential::ShortRangeTruncationScheme &shortRangeTruncationScheme_;

    // AtomTypes
    public:
    SerializablePairPotential(double &range, double &delta, bool &source, std::vector<std::shared_ptr<AtomType>> &types);

    double &range();
    const double &range() const;
    double &delta();
    const double &delta() const;
    bool &atomTypeChargeSource();
    const bool &atomTypeChargeSource() const;

    std::vector<std::shared_ptr<AtomType>> &atomTypes();
    const std::vector<std::shared_ptr<AtomType>> &atomTypes() const;

    PairPotential::CoulombTruncationScheme &coulombTruncationScheme();
    const PairPotential::CoulombTruncationScheme &coulombTruncationScheme() const;
    PairPotential::ShortRangeTruncationScheme &shortRangeTruncationScheme();
    const PairPotential::ShortRangeTruncationScheme &shortRangeTruncationScheme() const;

    // This method generates a 'pairPotentials' TOML node from the object's members
    toml::basic_value<toml::discard_comments, std::map, std::vector> serialise();
    // This method populates the object's members with values read from a 'pairPotentials' TOML node
    void deserialise(toml::value node);
};
