#include "serializablepairpotential.h"

//SerializablePairPotential::SerializablePairPotential() {}
//SerializablePairPotential::~SerializablePairPotential() {}

double &SerializablePairPotential::pairPotentialRange() { return pairPotentialRange_; }
void SerializablePairPotential::setPairPotentialRange(double &range) { pairPotentialRange_ = range; }
double &SerializablePairPotential::pairPotentialDelta() { return pairPotentialDelta_; }
void SerializablePairPotential::setPairPotentialDelta(double &delta) { pairPotentialDelta_ = delta; }
bool &SerializablePairPotential::atomTypeChargeSource() { return atomTypeChargeSource_; }
void SerializablePairPotential::setAtomTypeChargeSource(bool &source) { atomTypeChargeSource_ = source; }

PairPotential::CoulombTruncationScheme &SerializablePairPotential::coulombTruncationScheme()
{
    return coulombTruncationScheme_;
}
void SerializablePairPotential::setCoulombTruncationScheme(PairPotential::CoulombTruncationScheme &scheme)
{
    coulombTruncationScheme_ = scheme;
}
PairPotential::ShortRangeTruncationScheme &SerializablePairPotential::shortRangeTruncationScheme()
{
    return shortRangeTruncationScheme_;
}
void SerializablePairPotential::setShortRangeTruncationScheme(PairPotential::ShortRangeTruncationScheme &scheme)
{
    shortRangeTruncationScheme_ = scheme;
}