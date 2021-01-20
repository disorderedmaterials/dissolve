// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "data/ff.h"
#include "templates/optionalref.h"
#include <pugixml.hpp>
#include <string_view>

// Forward Declarations
/* none */

// OPLS-AA (2005) Base Forcefield
class XMLBaseForcefield : public Forcefield
{
    public:
    XMLBaseForcefield(std::string_view);
    XMLBaseForcefield(pugi::xml_node doc);
    ~XMLBaseForcefield() = default;

    /*
     * Definition
     */
    public:
    // Prepare the forcefield
    bool setUp();
    // The name of the forcefield
    std::string_view name() const;
    // A description of the forcefield
    std::string_view description() const;
    // Return formatted publication references
    std::string_view publicationReferences() const;
    // Return short-range interaction style for AtomTypes
    Forcefield::ShortRangeType shortRangeType() const;

    /*
     * Atom Type Data
     */
    public:
    // Return the base ForcefieldAtomType with specified id (if it exists)
    OptionalReferenceWrapper<const ForcefieldAtomType> oplsAtomTypeById(int id) const;

    /*
     * Term Data
     */
    public:
    // Return bond term for the supplied atom type pair (if it exists)
    OptionalReferenceWrapper<const ForcefieldBondTerm> getBondTerm(const ForcefieldAtomType &i,
                                                                   const ForcefieldAtomType &j) const;
    // Return angle term for the supplied atom type trio (if it exists)
    OptionalReferenceWrapper<const ForcefieldAngleTerm> getAngleTerm(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k) const;
    // Return torsion term for the supplied atom type quartet (if it exists)
    OptionalReferenceWrapper<const ForcefieldTorsionTerm> getTorsionTerm(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k, const ForcefieldAtomType &l) const;
    // Return improper term for the supplied atom type quartet (if it exists)
    OptionalReferenceWrapper<const ForcefieldImproperTerm> getImproperTerm(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k, const ForcefieldAtomType &l) const;

    private:
    std::vector<ForcefieldBondTerm> bondTerms_;
    std::vector<ForcefieldAngleTerm> angleTerms_;
    std::vector<ForcefieldTorsionTerm> torsionTerms_;
    std::vector<ForcefieldImproperTerm> improperTerms_;
    pugi::xml_node doc_;
};
