#include "serializablepairpotential.h"

SerializablePairPotential::SerializablePairPotential(double &range, double &delta, bool &source,
                                                     std::vector<std::shared_ptr<AtomType>> &types,
                                                     PairPotential::CoulombTruncationScheme &coulombScheme,
                                                     PairPotential::ShortRangeTruncationScheme &shortRangeScheme)
    : pairPotentialRange_(range), pairPotentialDelta_(delta), atomTypeChargeSource_(source), atomTypes_(types),
      coulombTruncationScheme_(coulombScheme), shortRangeTruncationScheme_(shortRangeScheme){};

double &SerializablePairPotential::pairPotentialRange() { return pairPotentialRange_; }
double &SerializablePairPotential::pairPotentialDelta() { return pairPotentialDelta_; }
bool &SerializablePairPotential::atomTypeChargeSource() { return atomTypeChargeSource_; }

std::vector<std::shared_ptr<AtomType>> &SerializablePairPotential::atomTypes() { return atomTypes_; }

PairPotential::CoulombTruncationScheme &SerializablePairPotential::coulombTruncationScheme()
{
    return coulombTruncationScheme_;
}
PairPotential::ShortRangeTruncationScheme &SerializablePairPotential::shortRangeTruncationScheme()
{
    return shortRangeTruncationScheme_;
}