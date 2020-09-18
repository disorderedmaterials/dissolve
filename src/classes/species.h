/*
    *** Species Definition
    *** src/classes/species.h
    Copyright T. Youngs 2012-2020

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

#include "classes/atomtypelist.h"
#include "classes/coordinateset.h"
#include "classes/isotopologue.h"
#include "classes/speciesangle.h"
#include "classes/speciesatom.h"
#include "classes/speciesbond.h"
#include "classes/speciesimproper.h"
#include "classes/speciessite.h"
#include "classes/speciestorsion.h"
#include "io/import/coordinates.h"
#include "templates/objectstore.h"
#include <memory>

// Forward Declarations
class Box;
class Forcefield;

// Species Definition
class Species : public ListItem<Species>, public ObjectStore<Species>
{
    public:
    Species();
    ~Species();
    // Clear Data
    void clear();

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
    bool checkSetUp();
    // Print Species information
    void print();
    // Return version
    int version() const;

    /*
     * Atomic Information
     */
    private:
    // List of Atoms in the Species
    List<SpeciesAtom> atoms_;
    // Version of the atom types
    VersionCounter atomTypesVersion_;
    // List of selected Atoms
    RefList<SpeciesAtom> selectedAtoms_;
    // Version of the atom selection
    VersionCounter atomSelectionVersion_;
    // List of AtomTypes, and their populations, used in the Species
    AtomTypeList usedAtomTypes_;
    // Point at which the used atom types list was last update
    int usedAtomTypesPoint_;

    public:
    // Add a new atom to the Species
    SpeciesAtom *addAtom(Element *element, Vec3<double> r, double q = 0.0);
    // Remove the specified atom from the species
    void removeAtom(SpeciesAtom *i);
    // Return the number of atoms in the species
    int nAtoms() const;
    // Return the first atom in the Species
    SpeciesAtom *firstAtom() const;
    // Return the nth atom in the Species
    SpeciesAtom *atom(int n);
    // Return the list of atoms
    const List<SpeciesAtom> &atoms() const;
    // Set coordinates of specified atom
    void setAtomCoordinates(SpeciesAtom *i, Vec3<double> r);
    // Set coordinates of specified atom (by index and individual coordinates)
    void setAtomCoordinates(int id, double x, double y, double z);
    // Transmute specified atom
    void transmuteAtom(SpeciesAtom *i, Element *el);
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
    const RefList<SpeciesAtom> &selectedAtoms() const;
    // Return nth selected atom
    SpeciesAtom *selectedAtom(int n);
    // Return number of selected atoms
    int nSelectedAtoms() const;
    // Return whether specified atom is selected
    bool isAtomSelected(SpeciesAtom *i) const;
    // Return version of the atom selection
    int atomSelectionVersion() const;
    // Return total atomic mass of Species
    double mass() const;
    // Bump AtomTypes version
    void bumpAtomTypesVersion();
    // Update and return used AtomTypesList
    const AtomTypeList &usedAtomTypes();
    // Clear AtomType assignments for all atoms
    void clearAtomTypes();
    // Return total charge of species from local atomic charges
    double totalChargeOnAtoms();

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

    private:
    // Add missing higher order intramolecular terms from current bond connectivity, and prune any that are now invalid
    void updateIntramolecularTerms();

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
    // Return array of SpeciesBonds (const)
    const std::vector<SpeciesBond> &constBonds() const;
    // Return whether SpeciesBond between SpeciesAtoms exists
    bool hasBond(SpeciesAtom *i, SpeciesAtom *j) const;
    // Return the SpeciesBond between the specified SpeciesAtoms
    OptionalReferenceWrapper<SpeciesBond> getBond(SpeciesAtom *i, SpeciesAtom *j);
    // Return the SpeciesBond between the specified SpeciesAtom indices
    OptionalReferenceWrapper<SpeciesBond> getBond(int i, int j);
    // Return the SpeciesBond between the specified SpeciesAtoms (const)
    OptionalReferenceWrapper<const SpeciesBond> getConstBond(SpeciesAtom *i, SpeciesAtom *j) const;
    // Add missing bonds
    void addMissingBonds(double tolerance = 1.1);
    // Add new SpeciesAngle definition
    SpeciesAngle &addAngle(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k);
    // Add new SpeciesAngle dedefinitionfinition
    SpeciesAngle &addAngle(int i, int j, int k);
    // Return number of SpeciesAngle defined
    int nAngles() const;
    // Return array of SpeciesAngle
    std::vector<SpeciesAngle> &angles();
    // Return array of SpeciesAngle (const)
    const std::vector<SpeciesAngle> &constAngles() const;
    // Return whether SpeciesAngle between SpeciesAtoms exists
    bool hasAngle(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k) const;
    // Return the SpeciesAngle between the specified SpeciesAtoms
    OptionalReferenceWrapper<SpeciesAngle> getAngle(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k);
    // Return the SpeciesAngle between the specified SpeciesAtom indices
    OptionalReferenceWrapper<SpeciesAngle> getAngle(int i, int j, int k);
    // Add new SpeciesTorsion definition (from SpeciesAtom*)
    SpeciesTorsion &addTorsion(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l);
    // Add new SpeciesTorsion definition
    SpeciesTorsion &addTorsion(int i, int j, int k, int l);
    // Return number of SpeciesTorsion defined
    int nTorsions() const;
    // Return array of SpeciesTorsion
    std::vector<SpeciesTorsion> &torsions();
    // Return array of SpeciesTorsion (const)
    const std::vector<SpeciesTorsion> &constTorsions() const;
    // Return whether SpeciesTorsion between SpeciesAtoms exists
    bool hasTorsion(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l) const;
    // Return the SpeciesTorsion between the specified SpeciesAtoms
    OptionalReferenceWrapper<SpeciesTorsion> getTorsion(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l);
    // Return the SpeciesTorsion between the specified SpeciesAtom indices
    OptionalReferenceWrapper<SpeciesTorsion> getTorsion(int i, int j, int k, int l);
    // Add new SpeciesImproper definition (from SpeciesAtom*)
    SpeciesImproper &addImproper(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l);
    // Add new SpeciesImproper definition
    SpeciesImproper &addImproper(int i, int j, int k, int l);
    // Return number of SpeciesImproper defined
    int nImpropers() const;
    // Return array of SpeciesImproper
    std::vector<SpeciesImproper> &impropers();
    // Return array of SpeciesImproper (const)
    const std::vector<SpeciesImproper> &constImpropers() const;
    // Return whether SpeciesImproper between SpeciesAtoms exists
    bool hasImproper(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l) const;
    // Return the SpeciesImproper between the specified SpeciesAtoms (if it exists)
    OptionalReferenceWrapper<SpeciesImproper> getImproper(SpeciesAtom *i, SpeciesAtom *j, SpeciesAtom *k, SpeciesAtom *l);
    // Return the SpeciesImproper between the specified SpeciesAtom indices
    OptionalReferenceWrapper<SpeciesImproper> getImproper(int i, int j, int k, int l);
    // Return whether the attached atoms lists have been created
    bool attachedAtomListsGenerated() const;
    // Generate attached Atom lists for all intramolecular terms
    void generateAttachedAtomLists();
    // Detach master term links for all interaction types, copying parameters to local SpeciesIntra
    void detachFromMasterTerms();

    /*
     * Source Forcefield (if any)
     */
    private:
    // Forcefield to source terms from
    std::shared_ptr<Forcefield> forcefield_;
    // Auto-generate missing intramolecular terms, and remove invalid ones
    bool autoUpdateIntramolecularTerms_;

    public:
    // Set Forcefield to source terms from
    void setForcefield(std::shared_ptr<Forcefield> ff);
    // Return Forcefield to source terms from
    std::shared_ptr<Forcefield> forcefield() const;
    // Set whether to auto-generate missing intramolecular terms, and remove invalid ones
    void setAutoUpdateIntramolecularTerms(bool b);
    // Return whether to auto-generate missing intramolecular terms, and remove invalid ones
    bool autoUpdateIntramolecularTerms() const;
    // Apply terms from source Forcefield
    bool applyForcefieldTerms(CoreData &coreData);

    /*
     * Isotopologues
     */
    private:
    // Natural Isotopologue
    Isotopologue naturalIsotopologue_;
    // Point at which natural Isotopologue was last updated
    int naturalIsotopologuePoint_;
    // List of isotopic variants defined for this species
    List<Isotopologue> isotopologues_;

    public:
    // Update current Isotopologues
    void updateIsotopologues();
    // Update and return natural isotopologue
    Isotopologue *naturalIsotopologue();
    // Add a new Isotopologue to this Species
    Isotopologue *addIsotopologue(std::string_view baseName);
    // Remove specified Isotopologue from this Species
    void removeIsotopologue(Isotopologue *iso);
    // Return number of defined Isotopologues
    int nIsotopologues() const;
    // Return nth Isotopologue in the list
    Isotopologue *isotopologue(int n);
    // Return Isotopologue List
    const List<Isotopologue> &isotopologues() const;
    // Return whether the specified Isotopologue exists
    bool hasIsotopologue(const Isotopologue *iso) const;
    // Generate unique Isotopologue name with base name provided
    std::string uniqueIsotopologueName(std::string_view baseName, const Isotopologue *exclude = nullptr);
    // Search for Isotopologue by name
    Isotopologue *findIsotopologue(std::string_view name, const Isotopologue *exclude = nullptr);
    // Return index of specified Isotopologue
    int indexOfIsotopologue(const Isotopologue *iso) const;

    /*
     * Site
     */
    private:
    // List of defined sites
    List<SpeciesSite> sites_;

    public:
    // Add a new SpeciesSite to this Species
    SpeciesSite *addSite(std::string_view name = "");
    // Remove specified SpeciesSite
    void removeSite(SpeciesSite *site);
    // Return number of defined SpeciesSites
    int nSites() const;
    // Return SpeciesSite List
    const List<SpeciesSite> &sites() const;
    // Return nth SpeciesSite defined
    SpeciesSite *site(int n);
    // Generate unique site name with base name provided
    std::string uniqueSiteName(std::string_view base, const SpeciesSite *exclude = nullptr) const;
    // Search for SpeciesSite by name
    SpeciesSite *findSite(std::string_view name, const SpeciesSite *exclude = nullptr) const;

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
    List<CoordinateSet> coordinateSets_;
    // File / format of coordinate sets file, if provided
    CoordinateImportFileFormat coordinateSetInputCoordinates_;

    public:
    // Clear coordinate sets
    void clearCoordinateSets();
    // Add new coordinate set
    CoordinateSet *addCoordinateSet();
    // Return number of defined coordinate sets
    int nCoordinateSets() const;
    // Return coordinates sets
    const List<CoordinateSet> &coordinateSets() const;

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
    enum SpeciesKeyword
    {
        AngleKeyword,          /* 'Angle' - Defines an Angle joining three atoms */
        AtomKeyword,           /* 'Atom' - Specifies an Atom in the Species */
        BondKeyword,           /* 'Bond' - Defines a Bond joining two atoms */
        BondTypeKeyword,       /* 'BondType' - Sets the type of a specific bond */
        ChargeKeyword,         /* 'Charge' - Specifies the atomic charge for an individual atom */
        CoordinateSetsKeyword, /* 'CoordinateSets' - File and format for any associated coordinate sets */
        EndSpeciesKeyword,     /* 'EndSpecies' - Signals the end of the current Species */
        ForcefieldKeyword,     /* 'Forcefield' - Sets the Forcefield from which to (re)generate or set terms */
        ImproperKeyword,       /* 'Improper' - Define an Improper interaction between four atoms */
        IsotopologueKeyword,   /* 'Isotopologue' - Add an isotopologue to the Species */
        SiteKeyword,           /* 'Site' - Define an analysis site within the Species */
        TorsionKeyword,        /* 'Torsion' - Define a Torsion interaction between four atoms */
        nSpeciesKeywords       /* Number of keywords defined for this block */
    };
    // Return enum option info for SpeciesKeyword
    static EnumOptions<Species::SpeciesKeyword> keywords();
    // Read Species definition from specified LineParser
    bool read(LineParser &parser, CoreData &coreData);
    // Write Species definition to specified LineParser
    bool write(LineParser &parser, std::string_view prefix);
};
