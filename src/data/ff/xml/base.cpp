// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "data/ff/xml/base.h"
#include <iostream>

XMLBaseForcefield::XMLBaseForcefield(std::string_view source) {
  pugi::xml_document doc;
  auto result = doc.load_file(source.data());
  if (result) {
    doc_ = doc.root();
  }
  else {
    //Throw a fit
  }
  setUp();
}

XMLBaseForcefield::XMLBaseForcefield(pugi::xml_node doc) {
  doc_ = doc;
  setUp();
}

/*
 * Definition
 */

bool XMLBaseForcefield::setUp() {
  for (auto &b : doc_.select_nodes("/ForceField/HarmonicBondForce/Bond")) {
    std::cout <<
      b.node().attribute("class1").as_string() << " x " <<
      b.node().attribute("class2").as_string() << ": " <<
      b.node().attribute("k").as_double() << " over " <<
      b.node().attribute("length").as_double() <<
      std::endl;
    bondTerms_.emplace_back(b.node().attribute("class1").as_string(),
			    b.node().attribute("class2").as_string(),
			    SpeciesBond::HarmonicForm,
			    std::vector<double>(
			    {b.node().attribute("length").as_double(),
			     b.node().attribute("k").as_double()}));
  }
  for (auto &a : doc_.select_nodes("/ForceField/HarmonicAngleForce/Angle")) {
    std::cout <<
      a.node().attribute("class1").as_string() << " x " <<
      a.node().attribute("class2").as_string() << " x " <<
      a.node().attribute("class3").as_string() << ": " <<
      a.node().attribute("k").as_double() << " over " <<
      a.node().attribute("angle").as_double() << " degrees" <<
      std::endl;
    angleTerms_.emplace_back(a.node().attribute("class1").as_string(),
			     a.node().attribute("class2").as_string(),
			     a.node().attribute("class3").as_string(),
			     SpeciesAngle::HarmonicForm,
			     std::vector<double>(
						 {a.node().attribute("angle").as_double(),
						  a.node().attribute("k").as_double()}));
  }
  for (auto &a : doc_.select_nodes("/ForceField/PeriodicTorsionForce/Proper")) {
    std::cout <<
      a.node().attribute("class1").as_string() << " x " <<
      a.node().attribute("class2").as_string() << " x " <<
      a.node().attribute("class3").as_string() << " x " <<
      a.node().attribute("class4").as_string() << ": " <<
      a.node().attribute("k1").as_double() << " x " <<
      a.node().attribute("k2").as_double() << " x " <<
      a.node().attribute("k3").as_double() << " x " <<
      a.node().attribute("k4").as_double() <<
      std::endl;
    //TODO: Get correct force function
    torsionTerms_.emplace_back(a.node().attribute("class1").as_string(),
			       a.node().attribute("class2").as_string(),
			       a.node().attribute("class3").as_string(),
			       a.node().attribute("class4").as_string(),
			       SpeciesTorsion::Cos4Form,
			       std::vector<double>(
						   {a.node().attribute("k1").as_double(),
						    a.node().attribute("k2").as_double(),
						    a.node().attribute("k3").as_double(),
						    a.node().attribute("k4").as_double()}));
  }
  for (auto &a : doc_.select_nodes("/ForceField/PeriodicTorsionForce/Improper")) {
    std::cout <<
      a.node().attribute("class1").as_string() << " x " <<
      a.node().attribute("class2").as_string() << " x " <<
      a.node().attribute("class3").as_string() << " x " <<
      a.node().attribute("class4").as_string() << ": " <<
      a.node().attribute("k1").as_double() << " x " <<
      a.node().attribute("k2").as_double() << " x " <<
      a.node().attribute("k3").as_double() << " x " <<
      a.node().attribute("k4").as_double() <<
      std::endl;
    //TODO: Get correct force function
    improperTerms_.emplace_back(a.node().attribute("class1").as_string(),
				a.node().attribute("class2").as_string(),
				a.node().attribute("class3").as_string(),
				a.node().attribute("class4").as_string(),
				SpeciesImproper::Cos4Form,
				std::vector<double>(
						    {a.node().attribute("k1").as_double(),
						     a.node().attribute("k2").as_double(),
						     a.node().attribute("k3").as_double(),
						     a.node().attribute("k4").as_double()}));
  }
  return true;
};

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
