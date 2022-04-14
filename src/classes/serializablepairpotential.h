#pragma once

#include "pairpotential.h"

#include <memory>
#include <string>
#include <vector>

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
};