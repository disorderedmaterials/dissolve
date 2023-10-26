// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "classes/pairPotential.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

class SerializablePairPotential : public Serialisable<>
{
    private:
    double &range_;
    double &delta_;
    bool &atomTypeChargeSource_, &forceCharge_, &autoCharge_;

    std::vector<std::shared_ptr<AtomType>> &atomTypes_;

    PairPotential::CoulombTruncationScheme &coulombTruncationScheme_;
    PairPotential::ShortRangeTruncationScheme &shortRangeTruncationScheme_;

    // AtomTypes
    public:
    SerializablePairPotential(double &range, double &delta, bool &source, bool &forceCharge, bool &autoCharge,
                              std::vector<std::shared_ptr<AtomType>> &types);

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

    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
};
