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
