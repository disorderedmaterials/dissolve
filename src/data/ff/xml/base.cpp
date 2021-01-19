// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/ff/xml/base.h"
#include <pugixml.hpp>
#include <iostream>

XMLBaseForcefield::XMLBaseForcefield(std::string_view source) {
  pugi::xml_document doc;
  auto result = doc.load_file(source.data());
  std::cout << "Load result: " << result.description() << std::endl;
}

/*
 * Definition
 */

bool XMLBaseForcefield::setUp() {return true;};

std::string_view XMLBaseForcefield::name() const {
  return "XML";
}

std::string_view XMLBaseForcefield::description() const {
  return "Whatever forcefield you supplied";
}


std::string_view XMLBaseForcefield::publicationReferences() const {
  return "I have no idea";
}


// Return short-range interaction style for AtomTypes
Forcefield::ShortRangeType XMLBaseForcefield::shortRangeType() const { return Forcefield::LennardJonesGeometricType; }



OptionalReferenceWrapper<const ForcefieldBondTerm> XMLBaseForcefield::getBondTerm(const ForcefieldAtomType &i,
                                                                   const ForcefieldAtomType &j) const {
    return Forcefield::termMatch_(bondTerms_, i, j);
}

OptionalReferenceWrapper<const ForcefieldAngleTerm> XMLBaseForcefield::getAngleTerm(const ForcefieldAtomType &i,
                                                                   const ForcefieldAtomType &j,
								   const ForcefieldAtomType &k) const {
    return Forcefield::termMatch_(angleTerms_, i, j, k);
}

OptionalReferenceWrapper<const ForcefieldTorsionTerm> XMLBaseForcefield::getTorsionTerm(const ForcefieldAtomType &i,
                                                                   const ForcefieldAtomType &j,
								   const ForcefieldAtomType &k,
								   const ForcefieldAtomType &l) const {
    return Forcefield::termMatch_(torsionTerms_, i, j, k, l);
}

OptionalReferenceWrapper<const ForcefieldImproperTerm> XMLBaseForcefield::getImproperTerm(const ForcefieldAtomType &i,
                                                                   const ForcefieldAtomType &j,
								   const ForcefieldAtomType &k,
								   const ForcefieldAtomType &l) const {
    return Forcefield::termMatch_(improperTerms_, i, j, k, l);
}
