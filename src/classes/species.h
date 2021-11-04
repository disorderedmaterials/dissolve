// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/atomtypemix.h"
#include "classes/box.h"
#include "classes/cellarray.h"
#include "classes/isotopologue.h"
#include "classes/speciesangle.h"
#include "classes/speciesatom.h"
#include "classes/speciesbond.h"
#include "classes/speciesimproper.h"
#include "classes/speciessite.h"
#include "classes/speciestorsion.h"
#include "io/import/coordinates.h"
#include <list>
#include <memory>
#include <variant>

// Forward Declarations
class Forcefield;

// Species Definition
class Species
{
    public:
    Species();
    ~Species() = default;
    // Clear Data
    void clear();
    // Copy basic information (atoms and intramolecular terms)
    void copyBasic(const Species *source);

    /*
     * Basic Information
     */
    private:
    // Name of the Species
    std::string name_;
    // Version of the Species
    VersionCounter version_;

    public:
    // Set name of the Species
    void setName(std::string_view name);
    // Return the name of the Species
    std::string_view name() const;
    // Check set-up of Species
    bool checkSetUp() const;
    // Print Species information
    void print() const;
    // Return version
    int version() const;

    /*
     * Atomic Information
     */
    private:
    // List of atoms in the Species
    std::list<SpeciesAtom> atoms_;
    // Vector of selected atoms
    std::vector<SpeciesAtom *> selectedAtoms_;
    // Version of the atom selection
    VersionCounter atomSelectionVersion_;
    // AtomType mixture present in the Species
    AtomTypeMix atomTypes_;

    public:
    // Add a new atom to the Species
    SpeciesAtom &addAtom(Elements::Element Z, Vec3<double> r, double q = 0.0);
    // Remove the specified atom from the species
    void removeAtom(SpeciesAtom *i);
    // Return the number of atoms in the species
    int nAtoms() const;
    // Renumber atoms so they are sequential in the list
    void renumberAtoms();
    // Return the first atom in the Species
    const SpeciesAtom &firstAtom() const;
    // Return the nth atom in the Species
    SpeciesAtom &atom(int n);
    const SpeciesAtom &atom(int n) const;
    // Return a reference to the vector of atoms
    const std::list<SpeciesAtom> &atoms() const;
    std::list<SpeciesAtom> &atoms();
    // Set coordinates of specified atom
    void setAtomCoordinates(SpeciesAtom *i, Vec3<double> r);
    // Set coordinates of specified atom (by index and individual coordinates)
    void setAtomCoordinates(int id, double x, double y, double z);
    // Transmute specified atom
    void transmuteAtom(SpeciesAtom *i, Elements::Element newZ);
    // Clear current atom selection
    void clearAtomSelection();
    // Add atom to selection
    void selectAtom(SpeciesAtom *i);
    // Remove atom from selection
    void deselectAtom(SpeciesAtom *i);
    // Toggle selection state of specified atom
    void toggleAtomSelection(SpeciesAtom *i);
    // Select atoms along any path from the specified one, ignoring the bond(s) provided
    void selectFromAtom(SpeciesAtom *i, SpeciesBond &exclude, OptionalReferenceWrapper<SpeciesBond> excludeToo = std::nullopt);
    // Return current atom selection
    const std::vector<SpeciesAtom *> &selectedAtoms() const;
    // Return number of selected atoms
    int nSelectedAtoms() const;
    // Return whether the current selection comprises atoms of a single element
    bool isSelectionSingleElement() const;
    // Return version of the atom selection
    int atomSelectionVersion() const;
    // Return total atomic mass of Species
    double mass() const;
    // Update used atom types
    void updateAtomTypes();
    // Return used atom types
    const AtomTypeMix &atomTypes() const;
    // Clear AtomType assignments for all atoms
    void clearAtomTypes();
    // Simplify atom types, merging together those with identical parameters
    int simplifyAtomTypes();
    // Return total charge of species from local/atomtype atomic charges
    double totalCharge(bool useAtomTypes) const;

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
    // Add new SpeciesBond definition (from SpeciesAtom*)
    SpeciesBond &addBond(SpeciesAtom *i, SpeciesAtom *j);
    // Add new SpeciesBond definition
    SpeciesBond &addBond(int i, int j);
    // Remove bond between specified SpeciesAtoms*
    void removeBond(SpeciesAtom *i, SpeciesAtom *j);
    // Return number of SpeciesBonds defined
    int nBonds() const;
    // Return array of SpeciesBond
    std::vector<SpeciesBond> &bonds();
    const std::vector<SpeciesBond> &bonds() const;
    // Return whether SpeciesBond between SpeciesAtoms exists
    bool hasBond(const SpeciesAtom *i, const SpeciesAtom *j) const;
    // Return whether SpeciesBond between specified atom indices exists
    bool hasBond(int i, int j) const;
    // Return the SpeciesBond between the specified SpeciesAtoms
    OptionalReferenceWrapper<SpeciesBond> getBond(SpeciesAtom *i, SpeciesAtom *j);
    OptionalReferenceWrapper<const SpeciesBond> getBond(const SpeciesAtom *i, const SpeciesAtom *j) const;
    // Return the SpeciesBond between the specified SpeciesAtom indices
    OptionalReferenceWrapper<SpeciesBond> getBond(int i, int j);
    OptionalReferenceWrapper<const SpeciesBond> getBond(int i, int j) const;
    // Add missing bonds
    void addMissingBonds(double tolerance = 1.1);
    // Add missing higher order intramolecular terms from current bond connectivity, and prune any that are now invalid
    void updateIntramolecularTerms();
    // Add new SpeciesAngle definition
    SpeciesAngle &addAngle(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k);
    // Add new SpeciesAngle dedefinitionfinition
    SpeciesAngle &addAngle(int i, int j, int k);
    // Return number of SpeciesAngle defined
    int nAngles() const;
    // Return array of SpeciesAngle
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
    // Add new SpeciesTorsion definition (from SpeciesAtom*)
    SpeciesTorsion &addTorsion(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l);
    // Add new SpeciesTorsion definition
    SpeciesTorsion &addTorsion(int i, int j, int k, int l);
    // Return number of SpeciesTorsion defined
    int nTorsions() const;
    // Return array of SpeciesTorsion
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
    // Add new SpeciesImproper definition (from SpeciesAtom*)
    SpeciesImproper &addImproper(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l);
    // Add new SpeciesImproper definition
    SpeciesImproper &addImproper(int i, int j, int k, int l);
    // Return number of SpeciesImproper defined
    int nImpropers() const;
    // Return array of SpeciesImproper
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
    // Generate attached Atom lists for all intramolecular terms
    void generateAttachedAtomLists();
    // Detach master term links for all interaction types, copying parameters to local SpeciesIntra
    void detachFromMasterTerms();
    // Reduce intramolecular terms to master terms
    void reduceToMasterTerms(CoreData &coreData, bool selectionOnly = false);

    /*
     * Box Definition (if any)
     */
    private:
    // Periodic Box
    std::unique_ptr<Box> box_;

    public:
    // Return periodic box
    const Box *box() const;
    // Create Box definition with specified lengths and angles
    void createBox(const Vec3<double> lengths, const Vec3<double> angles, bool nonPeriodic = false);

    /*
     * Source Forcefield (if any)
     */
    private:
    // Forcefield to source terms from
    std::shared_ptr<Forcefield> forcefield_;

    public:
    // Set Forcefield to source terms from
    void setForcefield(std::shared_ptr<Forcefield> ff);
    // Return Forcefield to source terms from
    std::shared_ptr<Forcefield> forcefield() const;
    // Apply terms from source Forcefield
    bool applyForcefieldTerms(CoreData &coreData);

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
    void updateIsotopologues(OptionalReferenceWrapper<const std::vector<std::shared_ptr<AtomType>>> atomTypes = std::nullopt);
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
    // Generate unique Isotopologue name with base name provided
    std::string uniqueIsotopologueName(std::string_view baseName, const Isotopologue *exclude = nullptr);
    // Search for Isotopologue by name
    const Isotopologue *findIsotopologue(std::string_view name, const Isotopologue *exclude = nullptr) const;
    // Return index of specified Isotopologue
    int indexOfIsotopologue(const Isotopologue *iso) const;

    /*
     * Site
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
    Vec3<double> centreOfGeometry(const Box *box) const;
    // Set centre of geometry
    void setCentre(const Box *box, const Vec3<double> newCentre);
    // Centre coordinates at origin
    void centreAtOrigin();

    /*
     * Coordinate Sets
     */
    private:
    // Available coordinate sets representing conformers, symmetry copies etc.
    std::vector<std::vector<Vec3<double>>> coordinateSets_;
    // File / format of coordinate sets file, if provided
    CoordinateImportFileFormat coordinateSetInputCoordinates_;

    public:
    // Clear coordinate sets
    void clearCoordinateSets();
    // Add new coordinate set
    std::vector<Vec3<double>> &addCoordinateSet();
    // Return number of defined coordinate sets
    int nCoordinateSets() const;
    // Return coordinates sets
    const std::vector<std::vector<Vec3<double>>> &coordinateSets() const;

    /*
     * File Input / Output
     */
    public:
    // Load Species information from XYZ file
    bool loadFromXYZ(std::string_view filename);
    // Load Species from file
    bool load(std::string_view filename);

    /*
     * Read / Write
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
        CoordinateSets, /* 'CoordinateSets' - File and format for any associated coordinate sets */
        EndSpecies,     /* 'EndSpecies' - Signals the end of the current Species */
        Forcefield,     /* 'Forcefield' - Sets the Forcefield from which to (re)generate or set terms */
        Improper,       /* 'Improper' - Define an Improper interaction between four atoms */
        Isotopologue,   /* 'Isotopologue' - Add an isotopologue to the Species */
        Site,           /* 'Site' - Define an analysis site within the Species */
        Torsion         /* 'Torsion' - Define a Torsion interaction between four atoms */
    };
    // Return enum option info for SpeciesKeyword
    static EnumOptions<Species::SpeciesKeyword> keywords();
    // Read Species definition from specified LineParser
    bool read(LineParser &parser, CoreData &coreData);
    // Write Species definition to specified LineParser
    bool write(LineParser &parser, std::string_view prefix);
};
