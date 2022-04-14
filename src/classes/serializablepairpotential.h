#pragma once

#include "pairpotential.h"

#include <string>

class SerializablePairPotential
{
    private:
    double &pairPotentialRange_;
    double &pairPotentialDelta_;
    bool &atomTypeChargeSource_;

    PairPotential::CoulombTruncationScheme &coulombTruncationScheme_;
    PairPotential::ShortRangeTruncationScheme &shortRangeTruncationScheme_;

    // AtomTypes
    public:
    SerializablePairPotential();
    ~SerializablePairPotential();

    double &pairPotentialRange();
    void setPairPotentialRange(double &range);
    double &pairPotentialDelta();
    void setPairPotentialDelta(double &delta);
    bool &atomTypeChargeSource();
    void setAtomTypeChargeSource(bool &source);

    PairPotential::CoulombTruncationScheme &coulombTruncationScheme();
    void setCoulombTruncationScheme(PairPotential::CoulombTruncationScheme &scheme);
    PairPotential::ShortRangeTruncationScheme &shortRangeTruncationScheme();
    void setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme &scheme);
};