#pragma once

#include "pairpotential.h"

#include <memory>
#include <string>
#include <vector>

class SerializablePairPotential
{
    private:
    double &pairPotentialRange_;
    double &pairPotentialDelta_;
    bool &atomTypeChargeSource_;

    std::vector<std::shared_ptr<AtomType>> &atomTypes_;

    PairPotential::CoulombTruncationScheme &coulombTruncationScheme_;
    PairPotential::ShortRangeTruncationScheme &shortRangeTruncationScheme_;

    // AtomTypes
    public:
    SerializablePairPotential(double &range, double &delta, bool &source, std::vector<std::shared_ptr<AtomType>> &types,
                              PairPotential::CoulombTruncationScheme &coulombScheme,
                              PairPotential::ShortRangeTruncationScheme &shortRangeScheme);
    ~SerializablePairPotential();

    double &pairPotentialRange();
    double &pairPotentialDelta();
    bool &atomTypeChargeSource();

    std::vector<std::shared_ptr<AtomType>> &atomTypes();

    PairPotential::CoulombTruncationScheme &coulombTruncationScheme();
    PairPotential::ShortRangeTruncationScheme &shortRangeTruncationScheme();
};