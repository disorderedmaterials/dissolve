// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "classes/box.h"
#include "classes/isotopologue.h"
#include "classes/speciesAngle.h"
#include "classes/speciesAtom.h"
#include "classes/speciesBond.h"
#include "classes/speciesImproper.h"
#include "classes/speciesSite.h"
#include "classes/speciesTorsion.h"
#include "templates/keyedVector.h"
#include <memory>
#include <vector>

// Forward Declarations
class CommonBond;
class CommonAngle;
class CommonTorsion;
class CommonImproper;
class Forcefield;

// Species Definition
class Species : public Serialisable<>
{
    public:
    Species(std::string name = "Unnamed");
    ~Species() = default;
    // Clear Data
    void clear();
    // Copy basic information (atoms and intramolecular terms)
    void copyBasic(const Species *source, bool copyAtomTypes = false);

    /*
     * Basic Information
     */
    private:
    // Name of the Species
    std::string name_;

    public:
    // Set name of the Species
    void setName(std::string_view name);
    // Return the name of the Species
    std::string_view name() const;
    // Check set-up of Species
    bool checkSetUp() const;
    // Print Species information
    void print() const;

    /*
     * Atomic Information
     */
    private:
    // List of atoms in the Species
    std::vector<SpeciesAtom> atoms_;
    // Atom types for the species
    std::vector<std::shared_ptr<AtomType>> atomTypes_;

    private:
    // Recursively add atoms along any path from the specified one, ignoring the bond(s) provided
    void getIndicesRecursive(std::vector<int> &indices, int index, OptionalReferenceWrapper<SpeciesBond> exclude,
                             OptionalReferenceWrapper<SpeciesBond> excludeToo) const;

    public:
    // Add a new atom to the Species, returning its index
    int addAtom(Elements::Element Z, Vector3 r, double q = 0.0, const AtomType *atomType = nullptr);
    // Remove the specified atom from the species
    void removeAtom(int index);
    // Remove set of atom indices
    void removeAtoms(std::vector<int> indices);
    // Return the number of atoms in the species (or only those with the specified presence)
    int nAtoms(SpeciesAtom::Presence withPresence = SpeciesAtom::Presence::Any) const;
    // Renumber atoms so they are sequential in the list
    void renumberAtoms();
    // Return the nth atom in the Species
    SpeciesAtom &atom(int n);
    const SpeciesAtom &atom(int n) const;
    // Return a reference to the vector of atoms
    const std::vector<SpeciesAtom> &atoms() const;
    std::vector<SpeciesAtom> &atoms();
    // Transmute specified atom
    void transmuteAtom(int index, Elements::Element newZ);
    // Return the fragment (vector of indices) containing the specified atom, optionally ignoring paths along the bond(s)
    // provided
    std::vector<int> fragment(int startIndex, OptionalReferenceWrapper<SpeciesBond> exclude = std::nullopt,
                              OptionalReferenceWrapper<SpeciesBond> excludeToo = std::nullopt) const;
    // Return total atomic mass of Species
    double mass() const;
    // Add new atom type to atom types
    AtomType *addAtomType(Elements::Element Z, std::string_view name = "");
    // Find and return the named atom type
    AtomType *findAtomType(std::string_view name) const;
    // Return atom types in the species
    std::vector<std::shared_ptr<AtomType>> &atomTypes();
    const std::vector<std::shared_ptr<AtomType>> &atomTypes() const;
    std::vector<const AtomType *> atomTypesRaw() const;
    // Calculate and return atom type populations
    KeyedVector<const AtomType *, int> atomTypePopulations() const;
    // Clear AtomType assignments for all atoms
    void clearAtomTypes();
    // Simplify atom types, merging together those with identical parameters
    int simplifyAtomTypes();
    // Return total charge of species from local/atomtype atomic charges
    double totalCharge(bool useAtomTypes) const;
    // Apply random noise to atoms
    void randomiseCoordinates(double maxDisplacement);

    /*
     * Intramolecular Data
     */
    private:
    // Array of bonds between atoms in the Species
    std::vector<SpeciesBond> bonds_;
    // Array of angles between atoms in the Species
    std::vector<SpeciesAngle> angles_;
    // Array of torsions between atoms in the Species
    std::vector<SpeciesTorsion> torsions_;
    // Array of impropers between atoms in the Species
    std::vector<SpeciesImproper> impropers_;
    // Whether the attached atoms lists have been created
    bool attachedAtomListsGenerated_;

    public:
    // Add new SpeciesBond definition
    SpeciesBond &addBond(SpeciesAtom *i, SpeciesAtom *j);
    SpeciesBond &addBond(int i, int j);
    // Remove bond between specified SpeciesAtoms
    void removeBond(SpeciesAtom *i, SpeciesAtom *j);
    void removeBond(int i, int j);
    // Return number of SpeciesBonds defined
    int nBonds() const;
    // Return vector of SpeciesBond
    std::vector<SpeciesBond> &bonds();
    const std::vector<SpeciesBond> &bonds() const;
    // Return whether SpeciesBond between SpeciesAtoms exists
    bool hasBond(const SpeciesAtom *i, const SpeciesAtom *j) const;
    bool hasBond(int i, int j) const;
    // Return the SpeciesBond between the specified SpeciesAtoms
    OptionalReferenceWrapper<SpeciesBond> getBond(SpeciesAtom *i, SpeciesAtom *j);
    OptionalReferenceWrapper<const SpeciesBond> getBond(const SpeciesAtom *i, const SpeciesAtom *j) const;
    // Return the SpeciesBond between the specified SpeciesAtom indices
    OptionalReferenceWrapper<SpeciesBond> getBond(int i, int j);
    OptionalReferenceWrapper<const SpeciesBond> getBond(int i, int j) const;
    // Add missing bonds
    void addMissingBonds(double tolerance = 1.1, bool preventMetallic = false);
    // Remove bonds crossing periodic boundaries
    void removePeriodicBonds();
    // Remove all higher order intramolecular terms
    void removeHigherOrderIntramolecularTerms();
    // Clear and regenerate bonds based on tolerance
    void recalculateIntermolecularTerms(double tolerance = 1.1);
    // Add missing higher order intramolecular terms from current bond connectivity, and prune any that are now invalid
    void updateIntramolecularTerms();
    // Add new SpeciesAngle definition
    SpeciesAngle &addAngle(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k);
    SpeciesAngle &addAngle(int i, int j, int k);
    // Return number of SpeciesAngle defined
    int nAngles() const;
    // Return vector of SpeciesAngle
    std::vector<SpeciesAngle> &angles();
    const std::vector<SpeciesAngle> &angles() const;
    // Return whether SpeciesAngle between SpeciesAtoms exists
    bool hasAngle(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k) const;
    // Return the SpeciesAngle between the specified SpeciesAtoms
    OptionalReferenceWrapper<SpeciesAngle> getAngle(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k);
    OptionalReferenceWrapper<const SpeciesAngle> getAngle(const SpeciesAtom *i, const SpeciesAtom *j,
                                                          const SpeciesAtom *k) const;
    // Return the SpeciesAngle between the specified SpeciesAtom indices
    OptionalReferenceWrapper<SpeciesAngle> getAngle(int i, int j, int k);
    OptionalReferenceWrapper<const SpeciesAngle> getAngle(int i, int j, int k) const;
    // Add new SpeciesTorsion definition
    SpeciesTorsion &addTorsion(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l);
    SpeciesTorsion &addTorsion(int i, int j, int k, int l);
    // Return number of SpeciesTorsion defined
    int nTorsions() const;
    // Return vector of SpeciesTorsion
    std::vector<SpeciesTorsion> &torsions();
    const std::vector<SpeciesTorsion> &torsions() const;
    // Return whether SpeciesTorsion between SpeciesAtoms exists
    bool hasTorsion(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l) const;
    // Return the SpeciesTorsion between the specified SpeciesAtoms
    OptionalReferenceWrapper<SpeciesTorsion> getTorsion(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l);
    OptionalReferenceWrapper<const SpeciesTorsion> getTorsion(const SpeciesAtom *i, const SpeciesAtom *j, const SpeciesAtom *k,
                                                              const SpeciesAtom *l) const;
    // Return the SpeciesTorsion between the specified SpeciesAtom indices
    OptionalReferenceWrapper<SpeciesTorsion> getTorsion(int i, int j, int k, int l);
    OptionalReferenceWrapper<const SpeciesTorsion> getTorsion(int i, int j, int k, int l) const;
    // Add new SpeciesImproper definition
    SpeciesImproper &addImproper(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l);
    SpeciesImproper &addImproper(int i, int j, int k, int l);
    // Return number of SpeciesImproper defined
    int nImpropers() const;
    // Return vector of SpeciesImproper
    std::vector<SpeciesImproper> &impropers();
    const std::vector<SpeciesImproper> &impropers() const;
    // Return whether SpeciesImproper between SpeciesAtoms exists
    bool hasImproper(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l) const;
    // Return the SpeciesImproper between the specified SpeciesAtoms (if it exists)
    OptionalReferenceWrapper<SpeciesImproper> getImproper(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l);
    OptionalReferenceWrapper<const SpeciesImproper> getImproper(const SpeciesAtom *i, const SpeciesAtom *j,
                                                                const SpeciesAtom *k, const SpeciesAtom *l) const;
    // Return the SpeciesImproper between the specified SpeciesAtom indices
    OptionalReferenceWrapper<SpeciesImproper> getImproper(int i, int j, int k, int l);
    OptionalReferenceWrapper<const SpeciesImproper> getImproper(int i, int j, int k, int l) const;
    // Return whether the attached atoms lists have been created
    bool attachedAtomListsGenerated() const;
    // Finalise internal relationships related to geometry once it is defined
    void finaliseGeometry();

    /*
     * Intramolecular Common Terms
     */
    private:
    // Common bond terms
    std::vector<std::shared_ptr<CommonBond>> commonBonds_;
    // Common angle terms
    std::vector<std::shared_ptr<CommonAngle>> commonAngles_;
    // Common torsion terms
    std::vector<std::shared_ptr<CommonTorsion>> commonTorsions_;
    // Common improper terms
    std::vector<std::shared_ptr<CommonImproper>> commonImpropers_;

    public:
    // Add new common Bond parameters
    CommonBond &addCommonBond(std::string_view name, std::optional<int> insertAtIndex = {});
    // Remove specified common Bond
    void removeCommonBond(const std::shared_ptr<CommonBond> &bond);
    // Return list of common Bond parameters
    std::vector<std::shared_ptr<CommonBond>> &commonBonds();
    const std::vector<std::shared_ptr<CommonBond>> &commonBonds() const;
    // Return whether named common Bond parameters exist
    OptionalReferenceWrapper<CommonBond> getCommonBond(std::string_view name);
    OptionalReferenceWrapper<const CommonBond> getCommonBond(std::string_view name) const;
    // Add new common Angle parameters
    CommonAngle &addCommonAngle(std::string_view name);
    // Remove specified common Angle
    void removeCommonAngle(const std::shared_ptr<CommonAngle> &angle);
    // Return list of common Angle parameters
    std::vector<std::shared_ptr<CommonAngle>> &commonAngles();
    const std::vector<std::shared_ptr<CommonAngle>> &commonAngles() const;
    // Return whether named common Angle parameters exist
    OptionalReferenceWrapper<CommonAngle> getCommonAngle(std::string_view name);
    OptionalReferenceWrapper<const CommonAngle> getCommonAngle(std::string_view name) const;
    // Add new common Torsion parameters
    CommonTorsion &addCommonTorsion(std::string_view name);
    // Remove specified common Torsion
    void removeCommonTorsion(const std::shared_ptr<CommonTorsion> &torsion);
    // Return list of common Torsion parameters
    std::vector<std::shared_ptr<CommonTorsion>> &commonTorsions();
    const std::vector<std::shared_ptr<CommonTorsion>> &commonTorsions() const;
    // Return whether named common Torsion parameters exist
    OptionalReferenceWrapper<CommonTorsion> getCommonTorsion(std::string_view name);
    OptionalReferenceWrapper<const CommonTorsion> getCommonTorsion(std::string_view name) const;
    // Add new common Improper parameters
    CommonImproper &addCommonImproper(std::string_view name);
    // Remove specified common Impropers
    void removeCommonImproper(const std::shared_ptr<CommonImproper> &improper);
    // Return list of common Improper parameters
    std::vector<std::shared_ptr<CommonImproper>> &commonImpropers();
    const std::vector<std::shared_ptr<CommonImproper>> &commonImpropers() const;
    // Return whether named common Improper parameters exist
    OptionalReferenceWrapper<CommonImproper> getCommonImproper(std::string_view name);
    OptionalReferenceWrapper<const CommonImproper> getCommonImproper(std::string_view name) const;
    // Clear all common terms
    void clearCommonTerms();
    // Detach common term links for all interaction types, copying parameters to local SpeciesIntra
    void detachFromCommonTerms();
    // Reduce intramolecular terms to common terms
    void reduceToCommonTerms();

    /*
     * Box Definition (if any)
     */
    private:
    // Periodic Box
    std::unique_ptr<Box> box_;

    public:
    // Return periodic box
    const Box *box() const;
    // Remove Box definition and revert to single image
    void removeBox();
    // Create Box definition with specified lengths and angles
    void createBox(const Vector3 lengths, const Vector3 angles, bool nonPeriodic = false);

    /*
     * Forcefield
     */
    public:
    // Apply terms from specified Forcefield
    bool applyForcefieldTerms(std::shared_ptr<Forcefield> ff);
    // Clear forcefield terms
    void clearForcefieldTerms(bool nullifyAtomTypes = true);

    /*
     * Isotopologues
     */
    private:
    // Natural Isotopologue
    Isotopologue naturalIsotopologue_;
    // List of isotopic variants defined for this species
    std::vector<std::unique_ptr<Isotopologue>> isotopologues_;

    public:
    // Update current Isotopologues
    void updateIsotopologues();
    // Return natural (empty) Isotopologue
    const Isotopologue *naturalIsotopologue() const;
    // Add a new Isotopologue to this Species
    Isotopologue *addIsotopologue(std::string_view baseName);
    // Remove specified Isotopologue from this Species
    void removeIsotopologue(Isotopologue *iso);
    // Return number of defined Isotopologues
    int nIsotopologues() const;
    // Return nth Isotopologue in the list
    Isotopologue *isotopologue(int n);
    // Return Isotopologue List
    const std::vector<std::unique_ptr<Isotopologue>> &isotopologues() const;
    // Return whether the specified Isotopologue exists
    bool hasIsotopologue(const Isotopologue *iso) const;
    // Search for Isotopologue by name
    const Isotopologue *findIsotopologue(std::string_view name) const;

    /*
     * Sites
     */
    private:
    // Defined sites
    std::vector<std::unique_ptr<SpeciesSite>> sites_;

    public:
    // Add a new SpeciesSite to this Species
    SpeciesSite *addSite(std::string_view name = "");
    // Remove specified SpeciesSite
    void removeSite(SpeciesSite *site);
    // Return number of defined SpeciesSites
    int nSites() const;
    // Return SpeciesSite List
    const std::vector<std::unique_ptr<SpeciesSite>> &sites() const;
    std::vector<std::unique_ptr<SpeciesSite>> &sites();
    // Generate unique site name with base name provided
    std::string uniqueSiteName(std::string_view base, const SpeciesSite *exclude = nullptr) const;
    // Search for SpeciesSite by name
    const SpeciesSite *findSite(std::string_view name, const SpeciesSite *exclude = nullptr) const;
    SpeciesSite *findSite(std::string_view name, const SpeciesSite *exclude = nullptr);

    /*
     * Transforms
     */
    public:
    // Calculate and return centre of geometry
    Vector3 centreOfGeometry(const Box *box) const;
    // Set centre of geometry
    void setCentre(const Box *box, const Vector3 newCentre);
    // Centre coordinates at origin
    void centreAtOrigin();

    /*
     * Serialisation
     */
    public:
    // Species Block Keyword Enum
    enum class SpeciesKeyword
    {
        Angle,          /* 'Angle' - Defines an Angle joining three atoms */
        Atom,           /* 'Atom' - Specifies an Atom in the Species */
        Bond,           /* 'Bond' - Defines a Bond joining two atoms */
        BondType,       /* 'BondType' - Sets the type of a specific bond */
        BoxAngles,      /* 'BoxAngles' - Specify unit cell angles for the species */
        BoxLengths,     /* 'BoxLengths' - Specify unit cell lengths for the species */
        Charge,         /* 'Charge' - Specifies the atomic charge for an individual atom */
        CommonAngle,    /* 'CommonAngle' - Define a common angle (added for transition to Dissolve2) */
        CommonBond,     /* 'CommonBond' - Define a common angle (added for transition to Dissolve2) */
        CommonImproper, /* 'CommonImproper' - Define a common angle (added for transition to Dissolve2) */
        CommonTorsion,  /* 'CommonTorsion' - Define a common angle (added for transition to Dissolve2) */
        EndSpecies,     /* 'EndSpecies' - Signals the end of the current Species */
        Forcefield,     /* 'Forcefield' - Sets the Forcefield from which to (re)generate or set terms */
        Improper,       /* 'Improper' - Define an Improper interaction between four atoms */
        Isotopologue,   /* 'Isotopologue' - Add an isotopologue to the Species */
        NAngles,        /* 'NAngles' - Hint at the total number of angles in the Species */
        NAtoms,         /* 'NAtoms' - Hint at the total number of atoms in the Species */
        NBonds,         /* 'NBonds' - Hint at the total number of bonds in the Species */
        NImpropers,     /* 'NImpropers' - Hint at the total number of impropers in the Species */
        NTorsions,      /* 'NTorsions' - Hint at the total number of torsions in the Species */
        Scaling14,      /* 'Scaling14' - Specify 1-4 scaling factors for torsion terms */
        Site,           /* 'Site' - Define an analysis site within the Species */
        Torsion         /* 'Torsion' - Define a Torsion interaction between four atoms */
    };
    // Return enum option info for SpeciesKeyword
    static EnumOptions<Species::SpeciesKeyword> keywords();
    // Read Species definition from specified LineParser
    bool read(LineParser &parser, CoreData &coreData);
    // Write Species definition to specified LineParser
    bool write(LineParser &parser, std::string_view prefix);

    // Express as a serialisable value
    void serialise(std::string tag, SerialisedValue &target) const override;
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node);
    // Load from specified TOML file
    void load(std::string_view tomlFile);
};
