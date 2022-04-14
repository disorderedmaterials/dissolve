#include "serializablepairpotential.h"

SerializablePairPotential::SerializablePairPotential(double &range, double &delta, bool &source,
                                                     std::vector<std::shared_ptr<AtomType>> &types,
                                                     PairPotential::CoulombTruncationScheme &coulombScheme,
                                                     PairPotential::ShortRangeTruncationScheme &shortRangeScheme)
    : range_(range), delta_(delta), atomTypeChargeSource_(source), atomTypes_(types), coulombTruncationScheme_(coulombScheme),
      shortRangeTruncationScheme_(shortRangeScheme){};

double &SerializablePairPotential::range() { return range_; }
double &SerializablePairPotential::delta() { return delta_; }
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