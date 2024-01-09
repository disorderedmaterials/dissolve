// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "classes/atomType.h"
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
class CoreData;
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
    std::vector<std::vector<std::reference_wrapper<const ForcefieldAtomType>>> atomTypesByElementPrivate_;

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
    // Determine and return atom type for specified SpeciesAtom from supplied Array of types
    static OptionalReferenceWrapper<const ForcefieldAtomType>
    determineAtomType(const SpeciesAtom &i,
                      const std::vector<std::vector<std::reference_wrapper<const ForcefieldAtomType>>> &atomTypes);
    // Determine and return atom type for specified SpeciesAtom
    virtual OptionalReferenceWrapper<const ForcefieldAtomType> determineAtomType(const SpeciesAtom &i) const;

    public:
    // Create NETA definitions for all atom types from stored defs
    bool createNETADefinitions();
    // Return named short-range parameters (if they exist)
    std::optional<std::string> shortRangeParameters(std::string_view name) const;
    // Return the named ForcefieldAtomType (if it exists)
    virtual OptionalReferenceWrapper<const ForcefieldAtomType>
    atomTypeByName(std::string_view name, Elements::Element onlyZ = Elements::Unknown) const;
    // Return the ForcefieldAtomType with specified id (if it exists)
    virtual OptionalReferenceWrapper<const ForcefieldAtomType> atomTypeById(int id,
                                                                            Elements::Element onlyZ = Elements::Unknown) const;

    /*
     * Term Data
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
    virtual OptionalReferenceWrapper<const ForcefieldBondTerm> getBondTerm(const ForcefieldAtomType &i,
                                                                           const ForcefieldAtomType &j) const;
    // Return angle term for the supplied atom type trio (if it exists)
    virtual OptionalReferenceWrapper<const ForcefieldAngleTerm>
    getAngleTerm(const ForcefieldAtomType &i, const ForcefieldAtomType &j, const ForcefieldAtomType &k) const;
    // Return torsion term for the supplied atom type quartet (if it exists)
    virtual OptionalReferenceWrapper<const ForcefieldTorsionTerm> getTorsionTerm(const ForcefieldAtomType &i,
                                                                                 const ForcefieldAtomType &j,
                                                                                 const ForcefieldAtomType &k,
                                                                                 const ForcefieldAtomType &l) const;
    // Return improper term for the supplied atom type quartet (if it exists)
    virtual OptionalReferenceWrapper<const ForcefieldImproperTerm> getImproperTerm(const ForcefieldAtomType &i,
                                                                                   const ForcefieldAtomType &j,
                                                                                   const ForcefieldAtomType &k,
                                                                                   const ForcefieldAtomType &l) const;

    /*
     * Term Assignment
     */
    private:
    // Find / determine atom type(s) for the specified atom(s)
    std::vector<std::reference_wrapper<const ForcefieldAtomType>> getAtomTypes(const std::vector<const SpeciesAtom *> &atoms,
                                                                               bool determineType) const;

    protected:
    // Assign suitable AtomType to the supplied atom
    bool assignAtomType(SpeciesAtom &i, CoreData &coreData, bool setSpeciesAtomCharges) const;
    // Assign / generate bond term parameters
    virtual bool assignBondTermParameters(const Species *parent, SpeciesBond &bond, bool determineTypes) const;
    // Assign / generate angle term parameters
    virtual bool assignAngleTermParameters(const Species *parent, SpeciesAngle &angle, bool determineTypes) const;
    // Assign / generate torsion term parameters
    virtual bool assignTorsionTermParameters(const Species *parent, SpeciesTorsion &torsion, bool determineTypes) const;
    // Assign / generate improper term parameters
    virtual bool assignImproperTermParameters(ForcefieldImproperTerm &improper, SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k,
                                              SpeciesAtom *l, bool determineTypes) const;

    public:
    // AtomType Assignment Strategy
    enum AtomTypeAssignmentStrategy
    {
        TypeAll,      /* Assign atom types to all atoms, overwriting any that exist */
        TypeMissing,  /* Assign atom types to all atoms that do not currently have a type assigned */
        TypeSelection /* Assign atom types to the current selection, overwriting any types on the atoms that already
                 exist */
    };
    // Intramolecular Term Assignment Flags
    enum IntramolecularTermAssignmentFlags
    {
        DetermineTypesFlag = 1,    /* Attempt to determine atom types on-the-fly, rather than use those existing on the atoms */
        GenerateImpropersFlag = 2, /* Generate improper terms where available */
        SelectionOnlyFlag = 4      /* Only assign terms where all atoms are in the current selection */
    };
    // Assign suitable AtomTypes to the supplied Species, returning the number of failures
    // Returns any elements that were unassigned
    std::vector<int> assignAtomTypes(Species *sp, CoreData &coreData, AtomTypeAssignmentStrategy strategy,
                                     bool setSpeciesAtomCharges) const;
    // Assign specific AtomType to the supplied atom
    void assignAtomType(const ForcefieldAtomType &ffa, SpeciesAtom &i, CoreData &coreData, bool setSpeciesAtomCharges) const;
    // Assign intramolecular parameters to the supplied Species
    bool assignIntramolecular(Species *sp, int flags = Forcefield::GenerateImpropersFlag) const;
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
