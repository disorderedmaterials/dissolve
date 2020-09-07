/*
    *** Forcefield Data
    *** src/data/ff.h
    Copyright T. Youngs 2019-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "classes/speciesangle.h"
#include "classes/speciesbond.h"
#include "classes/speciesimproper.h"
#include "classes/speciestorsion.h"
#include "data/elements.h"
#include "data/ffangleterm.h"
#include "data/ffatomtype.h"
#include "data/ffbondterm.h"
#include "data/ffimproperterm.h"
#include "data/ffparameters.h"
#include "data/fftorsionterm.h"
#include "templates/optionalref.h"
#include <algorithm>
#include <functional>
#include <tuple>
#include <vector>

// Forward Declarations
class CoreData;
class Species;
class SpeciesAtom;

// Forcefield Base Class
class Forcefield : public Elements
{
    public:
    Forcefield() = default;
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
    // ShortRange Interaction Type
    enum ShortRangeType
    {
        UndefinedType,             /* Undefined short-range type */
        NoInteractionType,         /* No short-range dispersive forces */
        LennardJonesType,          /* Lennard-Jones 12-6 form with Lorentz-Berthelot combination rules */
        LennardJonesGeometricType, /* Lennard-Jones 12-6 form with Geometric combination rules */
        nShortRangeTypes           /* Number of short-range interaction types */
    };
    // Return enum options for ShortRangeType
    static EnumOptions<ShortRangeType> shortRangeTypes();
    // Return name of Forcefield
    virtual std::string_view name() const = 0;
    // Return description of Forcefield
    virtual std::string_view description() const = 0;
    // Return short-range interaction style for AtomTypes
    virtual ShortRangeType shortRangeType() const = 0;

    /*
     * Atom Type Data
     */
    protected:
    // Short-range parameter sets
    std::vector<ForcefieldParameters> shortRangeParameters_;
    // Atom type data
    std::vector<ForcefieldAtomType> atomTypes_;
    // Atom type data, grouped by element
    std::vector<std::vector<std::reference_wrapper<const ForcefieldAtomType>>> atomTypesByElementPrivate_;

    protected:
    // Add short-range parameters
    void addParameters(std::string_view name, double data0, double data1 = 0.0, double data2 = 0.0, double data3 = 0.0);
    // Add new atom type with its own parameters
    void addAtomType(int Z, int index, std::string_view name, std::string_view netaDefinition, std::string_view description,
                     double q, double data0, double data1, double data2 = 0.0, double data3 = 0.0);
    // Add new atom type referencing existing parameters by name
    void addAtomType(int Z, int index, std::string_view name, std::string_view netaDefinition, std::string_view description,
                     double q, std::string_view parameterReference);
    // Copy existing atom type
    bool copyAtomType(OptionalReferenceWrapper<const ForcefieldAtomType> sourceType, std::string_view description,
                      std::string_view netaDefinition = "", std::string_view equivalentName = "");
    // Determine and return atom type for specified SpeciesAtom from supplied Array of types
    static OptionalReferenceWrapper<const ForcefieldAtomType>
    determineAtomType(SpeciesAtom *i,
                      const std::vector<std::vector<std::reference_wrapper<const ForcefieldAtomType>>> &atomTypes);
    // Determine and return atom type for specified SpeciesAtom
    virtual OptionalReferenceWrapper<const ForcefieldAtomType> determineAtomType(SpeciesAtom *i) const;

    public:
    // Create NETA definitions for all atom types from stored defs
    bool createNETADefinitions();
    // Return named short-range parameters (if they exist)
    const OptionalReferenceWrapper<const ForcefieldParameters> shortRangeParameters(std::string_view name) const;
    // Return the named ForcefieldAtomType (if it exists)
    virtual OptionalReferenceWrapper<const ForcefieldAtomType> atomTypeByName(std::string_view name,
                                                                              Element *element = nullptr) const;
    // Return the ForcefieldAtomType with specified id (if it exists)
    virtual OptionalReferenceWrapper<const ForcefieldAtomType> atomTypeById(int id, Element *element = nullptr) const;

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
    void addBondTerm(std::string_view typeI, std::string_view typeJ, SpeciesBond::BondFunction form,
                     const std::vector<double> parameters = {});
    // Add angle term
    void addAngleTerm(std::string_view typeI, std::string_view typeJ, std::string_view typeK, SpeciesAngle::AngleFunction form,
                      const std::vector<double> parameters = {});
    // Add torsion term
    void addTorsionTerm(std::string_view typeI, std::string_view typeJ, std::string_view typeK, std::string_view typeL,
                        SpeciesTorsion::TorsionFunction form, const std::vector<double> parameters = {});
    // Add improper term
    void addImproperTerm(std::string_view typeI, std::string_view typeJ, std::string_view typeK, std::string_view typeL,
                         SpeciesImproper::ImproperFunction form, const std::vector<double> parameters = {});
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
    protected:
    // Assign suitable AtomType to the supplied atom
    virtual bool assignAtomType(SpeciesAtom *i, CoreData &coreData) const;

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
    int assignAtomTypes(Species *sp, CoreData &coreData, AtomTypeAssignmentStrategy strategy) const;
    // Assign intramolecular parameters to the supplied Species
    virtual bool assignIntramolecular(Species *sp, int flags = Forcefield::GenerateImpropersFlag) const;

    /*
     * Atom Environment Helpers
     */
    public:
    // Atom Geometry enum
    enum AtomGeometry
    {
        UnboundGeometry,
        TerminalGeometry,
        LinearGeometry,
        TShapeGeometry,
        TrigonalPlanarGeometry,
        TetrahedralGeometry,
        SquarePlanarGeometry,
        TrigonalBipyramidalGeometry,
        OctahedralGeometry,
        nAtomGeometries
    };

    protected:
    // Calculate and return the geometry of the specified SpeciesAtom
    AtomGeometry geometryOfAtom(SpeciesAtom *i) const;
    // Return whether the specified SpeciesAtom exists in the specified geometry
    bool isAtomGeometry(SpeciesAtom *i, AtomGeometry geom) const;
    // Return whether supplied bonding pattern around the SpeciesAtom matches *exactly*
    bool isBondPattern(const SpeciesAtom *i, const int nSingle, const int nDouble = 0, const int nTriple = 0,
                       const int nQuadruple = 0, const int nAromatic = 0) const;
    // Return whether the specified atom is bound to a specific element (and count thereof)
    bool isBoundTo(const SpeciesAtom *i, Element *element, const int count = 1, bool allowMoreThanCount = true) const;
    // Guess and return oxidation state for the specified SpeciesAtom
    int guessOxidationState(const SpeciesAtom *i) const;
};

template <class T, typename... Args>
OptionalReferenceWrapper<const T> Forcefield::termMatch_(const std::vector<T> &container, Args &&... args)
{
    auto it = std::find_if(container.begin(), container.end(),
                           [&](const T &item) { return item.isMatch(std::forward<Args>(args)...); });
    if (it == container.end())
        return {};
    return *it;
}
