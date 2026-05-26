// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "classes/atomType.h"
#include "classes/pairPotentialOverride.h"
#include "classes/speciesAngle.h"
#include "classes/speciesBond.h"
#include "classes/speciesImproper.h"
#include "classes/speciesTorsion.h"
#include "data/elements.h"
#include "data/ff/angleTerm.h"
#include "data/ff/atomType.h"
#include "data/ff/bondTerm.h"
#include "data/ff/improperTerm.h"
#include "data/ff/torsionTerm.h"
#include "templates/optionalRef.h"
#include <algorithm>
#include <functional>
#include <tuple>
#include <vector>

// Forward Declarations
class Species;
class SpeciesAtom;

// Forcefield Base Class
class Forcefield
{
    public:
    Forcefield() = default;
    Forcefield(std::vector<ForcefieldAtomType> &atoms, std::vector<ForcefieldBondTerm> &bonds,
               std::vector<ForcefieldAngleTerm> &angles, std::vector<ForcefieldTorsionTerm> &torsions,
               std::vector<ForcefieldImproperTerm> &impropers);
    virtual ~Forcefield() = default;

    /*
     * Set Up
     */
    protected:
    // Set up / create all forcefield terms
    virtual bool setUp() = 0;

    public:
    // Prepare forcefield for use
    bool prepare();

    /*
     * Definition
     */
    public:
    // Return name of Forcefield
    virtual std::string_view name() const = 0;
    // Return description of Forcefield
    virtual std::string_view description() const = 0;
    // Return short-range interaction style for AtomTypes
    virtual ShortRangeFunctions::Form shortRangeForm() const = 0;

    /*
     * Atom Type Data
     */
    protected:
    // Named short-range parameter sets
    std::vector<std::pair<std::string, std::string>> shortRangeParameters_;
    // Atom type data
    std::vector<ForcefieldAtomType> atomTypes_;
    // Atom type data, grouped by element
    std::map<Elements::Element, std::vector<std::reference_wrapper<const ForcefieldAtomType>>> atomTypesByElement_;

    protected:
    // Add short-range parameters
    void addParameters(std::string_view name, const std::string_view parameterString);
    // Add new atom type with specified parameters
    void addAtomType(Elements::Element Z, int index, std::string_view name, std::string_view netaDefinition,
                     std::string_view description, double q, std::string_view parameterString);
    void addAtomType(Elements::Element Z, int index, std::string_view name, std::string_view netaDefinition,
                     std::string_view description, double q, const std::vector<double> &parameters);
    // Copy existing atom type
    bool copyAtomType(OptionalReferenceWrapper<const ForcefieldAtomType> sourceType, std::string_view description,
                      std::string_view netaDefinition = "", std::string_view equivalentName = "");

    public:
    // Create NETA definitions for all atom types from stored defs
    bool createNETADefinitions();
    // Return named short-range parameters (if they exist)
    std::optional<std::string> shortRangeParameters(std::string_view name) const;
    // Determine and return atom type for specified SpeciesAtom
    virtual OptionalReferenceWrapper<const ForcefieldAtomType> determineAtomType(const SpeciesAtom &i) const;
    // Return the named ForcefieldAtomType (if it exists)
    virtual OptionalReferenceWrapper<const ForcefieldAtomType> atomTypeByName(std::string_view name) const;
    // Return the ForcefieldAtomType with specified id (if it exists)
    virtual OptionalReferenceWrapper<const ForcefieldAtomType> atomTypeById(int id) const;

    /*
     * Intramolecular Term Data
     */
    private:
    // Bond terms of the Forcefield
    std::vector<ForcefieldBondTerm> bondTerms_;
    // Angle terms of the Forcefield
    std::vector<ForcefieldAngleTerm> angleTerms_;
    // Torsion terms of the Forcefield
    std::vector<ForcefieldTorsionTerm> torsionTerms_;
    // Improper terms of the Forcefield
    std::vector<ForcefieldImproperTerm> improperTerms_;

    protected:
    // Add bond term
    void addBondTerm(std::string_view typeI, std::string_view typeJ, BondFunctions::Form form,
                     std::string_view parameterString);
    // Add angle term
    void addAngleTerm(std::string_view typeI, std::string_view typeJ, std::string_view typeK, AngleFunctions::Form form,
                      std::string_view parameterString = "");
    // Add torsion term
    void addTorsionTerm(std::string_view typeI, std::string_view typeJ, std::string_view typeK, std::string_view typeL,
                        TorsionFunctions::Form form, std::string_view parameterString = "");
    // Add improper term
    void addImproperTerm(std::string_view typeI, std::string_view typeJ, std::string_view typeK, std::string_view typeL,
                         TorsionFunctions::Form form, std::string_view parameterString = "");
    // Match any kind of term
    template <class T, typename... Args>
    static OptionalReferenceWrapper<const T> termMatch_(const std::vector<T> &, Args &&...);

    public:
    // Return bond term for the supplied atom type pair (if it exists)
    virtual std::optional<const ForcefieldBondTerm> getBondTerm(const ForcefieldAtomType &i,
                                                                           const ForcefieldAtomType &j, OptionalReferenceWrapper<SpeciesBond> bond) const;
    // Return angle term for the supplied atom type trio (if it exists)
    virtual std::optional<const ForcefieldAngleTerm>
    getAngleTerm(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k, OptionalReferenceWrapper<SpeciesAngle> angle) const;
    // Return torsion term for the supplied atom type quartet (if it exists)
    virtual std::optional<const ForcefieldTorsionTerm> getTorsionTerm(const ForcefieldAtomType &i,
                                                                                 const ForcefieldAtomType &j,
                                                                                 const ForcefieldAtomType &k,
                                                                                 const ForcefieldAtomType &l,
                                                                                 OptionalReferenceWrapper<SpeciesTorsion> torsion) const;
    // Return improper term for the supplied atom type quartet (if it exists)
    virtual std::optional<ForcefieldImproperTerm> getImproperTerm(const ForcefieldAtomType &i, const ForcefieldAtomType &j,
                                                                  const ForcefieldAtomType &k,
                                                                  const ForcefieldAtomType &l) const;

    /*
     * Pair Potential Overrides
     */
    public:
    // Return available pair potential overrides
    virtual std::vector<PairPotentialOverride> pairPotentialOverrides();
};

template <class T, typename... Args>
OptionalReferenceWrapper<const T> Forcefield::termMatch_(const std::vector<T> &container, Args &&...args)
{
    auto it = std::find_if(container.begin(), container.end(),
                           [&](const T &item) { return item.isMatch(std::forward<Args>(args)...); });
    if (it == container.end())
        return {};
    return *it;
}
