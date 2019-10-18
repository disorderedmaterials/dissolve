/*
	*** Species Definition
	*** src/classes/species.h
	Copyright T. Youngs 2012-2019

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

#ifndef DISSOLVE_SPECIES_H
#define DISSOLVE_SPECIES_H

#include "classes/atomtypelist.h"
#include "classes/speciesangle.h"
#include "classes/speciesatom.h"
#include "classes/speciesbond.h"
#include "classes/speciestorsion.h"
#include "classes/speciesgrain.h"
#include "classes/speciessite.h"
#include "classes/isotopologue.h"
#include "base/charstring.h"
#include "base/version.h"
#include "templates/objectstore.h"

// Forward Declarations
class Box;
class Forcefield;

//Species Definition
class Species : public ListItem<Species>, public ObjectStore<Species>
{
	public:
	// Constructor / Destructor
	Species();
	~Species();
	// Clear Data
	void clear();


	/*
	 * Basic Information
	 */
	private:
	// Name of the Species
	CharString name_;
	// Version of the Species
	VersionCounter version_;

	public:
	// Set name of the Species
	void setName(const char* name);
	// Return the name of the Species
	const char* name() const;
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
	// List of selected Atoms
	RefList<SpeciesAtom> selectedAtoms_;
	// List of AtomTypes, and their populations, used in the Species
	AtomTypeList usedAtomTypes_;
	// Version of the atom selection
	VersionCounter atomSelectionVersion_;
	
	public:
	// Add a new atom to the Species
	SpeciesAtom* addAtom(Element* element, Vec3<double> r);
	// Return the number of atoms in the species
	int nAtoms() const;
	// Return the first atom in the Species
	SpeciesAtom* firstAtom() const;
	// Return the nth atom in the Species
	SpeciesAtom* atom(int n);
	// Return the list of atoms
	const List<SpeciesAtom>& atoms() const;
	// Set coordinates of specified atom
	void setAtomCoordinates(SpeciesAtom* i, Vec3<double> r);
	// Transmute specified atom
	void transmuteAtom(SpeciesAtom* i, Element* el);
	// Clear current atom selection
	void clearAtomSelection();
	// Add atom to selection
	void selectAtom(SpeciesAtom* i);
	// Remove atom from selection
	void deselectAtom(SpeciesAtom* i);
	// Toggle selection state of specified atom
	void toggleAtomSelection(SpeciesAtom* i);
	// Select atoms along any path from the specified one, ignoring the bond(s) provided
	void selectFromAtom(SpeciesAtom* i, SpeciesBond* exclude, SpeciesBond* excludeToo = NULL);
	// Return current atom selection
	const RefList<SpeciesAtom>& selectedAtoms() const;
	// Return nth selected atom
	SpeciesAtom* selectedAtom(int n);
	// Return number of selected atoms
	int nSelectedAtoms() const;
	// Return whether specified atom is selected
	bool isAtomSelected(SpeciesAtom* i) const;
	// Return version of the atom selection
	const int atomSelectionVersion() const;
	// Return total atomic mass of Species
	double mass() const;
	// Update used AtomTypeList
	void updateUsedAtomTypes();
	// Return used AtomTypesList
	const AtomTypeList& usedAtomTypes();
	// Clear AtomType assignments for all atoms
	void clearAtomTypes();


	/*
	 * Intramolecular Data
	 */
	private:
	// List of bonds between atoms in the Species
	List<SpeciesBond> bonds_;
	// List of angles between atoms in the Species
	List<SpeciesAngle> angles_;
	// List of torsions between atoms in the Species
	List<SpeciesTorsion> torsions_;
	// Whether the attached atoms lists have been created
	bool attachedAtomListsGenerated_;

	public:
	// Add new SpeciesBond definition (from SpeciesAtom*)
	SpeciesBond* addBond(SpeciesAtom* i, SpeciesAtom* j);
	// Add new SpeciesBond definition
	SpeciesBond* addBond(int i, int j);
	// Reconnect existing SpeciesBond
	bool reconnectBond(SpeciesBond* bond, SpeciesAtom* i, SpeciesAtom* j);
	// Reconnect existing SpeciesBond
	bool reconnectBond(SpeciesBond* bond, int i, int j);
	// Return number of SpeciesBonds defined
	int nBonds() const;
	// Return list of SpeciesBond
	const List<SpeciesBond>& bonds() const;
	// Return nth SpeciesBond
	SpeciesBond* bond(int n);
	// Return whether SpeciesBond between SpeciesAtoms exists
	SpeciesBond* hasBond(SpeciesAtom* i, SpeciesAtom* j) const;
	// Return whether SpeciesBond between specified atom indices exists
	SpeciesBond* hasBond(int i, int j);
	// Return index of specified SpeciesBond
	int bondIndex(SpeciesBond* spb);
	// Add new SpeciesAngle definition
	SpeciesAngle* addAngle(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k);
	// Add new SpeciesAngle definition
	SpeciesAngle* addAngle(int i, int j, int k);
	// Reconnect existing SpeciesAngle
	bool reconnectAngle(SpeciesAngle* angle, SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k);
	// Reconnect existing SpeciesAngle
	bool reconnectAngle(SpeciesAngle* angle, int i, int j, int k);
	// Return number of SpeciesAngle defined
	int nAngles() const;
	// Return list of SpeciesAngle
	const List<SpeciesAngle>& angles() const;
	// Return nth SpeciesAngle
	SpeciesAngle* angle(int n);
	// Return whether SpeciesAngle between SpeciesAtoms exists
	SpeciesAngle* hasAngle(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k) const;
	// Return index of specified SpeciesAngle
	int angleIndex(SpeciesAngle* spa);
	// Add new SpeciesTorsion definition (from SpeciesAtom*)
	SpeciesTorsion* addTorsion(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l);
	// Add new SpeciesTorsion definition
	SpeciesTorsion* addTorsion(int i, int j, int k, int l);
	// Reconnect existing SpeciesTorsion
	bool reconnectTorsion(SpeciesTorsion* torsion, SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l);
	// Reconnect existing SpeciesTorsion
	bool reconnectTorsion(SpeciesTorsion* torsion, int i, int j, int k, int l);
	// Return number of SpeciesTorsion defined
	int nTorsions() const;
	// Return list of SpeciesTorsion
	const List<SpeciesTorsion>& torsions() const;
	// Return nth SpeciesTorsion
	SpeciesTorsion* torsion(int n);
	// Return whether SpeciesTorsion between SpeciesAtoms exists
	SpeciesTorsion* hasTorsion(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l) const;
	// Return index of specified SpeciesTorsion
	int torsionIndex(SpeciesTorsion* spt);
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
	Forcefield* forcefield_;

	private:
	// Add missing higher order intramolecular terms from current bond connectivity
	void completeIntramolecularTerms();

	public:
	// Set Forcefield to source terms from
	void setForcefield(Forcefield* ff);
	// Return Forcefield to source terms from
	Forcefield* forcefield() const;
	// Apply terms from source Forcefield
	bool applyForcefieldTerms(CoreData& coreData);


	/*
	 * Grains
	 */
	private:
	// List of Grains, dividing the Atoms of this Species into individual groups
	List<SpeciesGrain> grains_;
	
	public:
	// Update SpeciesGrains after change
	void updateGrains();
	// Add default grain definition (i.e. one which contains all atoms) for this Species 
	void addDefaultGrain();
	// Automatically determine Grains based on chemical connectivity
	void autoAddGrains();
	// Add new grain for this Species
	SpeciesGrain* addGrain();
	// Remove grain with ID specified
	void removeGrain(SpeciesGrain* sg);
	// Return number of grain present for this Species
	int nGrains() const;
	// Return first grain in list
	SpeciesGrain* grains() const;
	// Return nth grain in list
	SpeciesGrain* grain(int n);
	// Add Atom to grain
	void addAtomToGrain(SpeciesAtom* i, SpeciesGrain* gd);
	// Generate unique grain name with base name provided
	const char* uniqueGrainName(const char* baseName, SpeciesGrain* exclude = NULL) const;
	// Order atoms within grains
	void orderAtomsWithinGrains();


	/*
	 * Isotopologues
	 */
	private:
	// List of isotopic variants defined for this species
	List<Isotopologue> isotopologues_;

	public:
	// Update current Isotopologues
	void updateIsotopologues();
	// Add a new Isotopologue to this Species
	Isotopologue* addIsotopologue(const char* baseName);
	// Remove specified Isotopologue from this Species
	void removeIsotopologue(Isotopologue* iso);
	// Return number of defined Isotopologues
	int nIsotopologues() const;
	// Return Isotopologue List
	const List<Isotopologue>& isotopologues() const;
	// Return nth Isotopologue defined
	Isotopologue* isotopologue(int n);
	// Return whether the specified Isotopologue exists
	bool hasIsotopologue(Isotopologue* iso) const;
	// Generate unique Isotopologue name with base name provided
	const char* uniqueIsotopologueName(const char* baseName, Isotopologue* exclude = NULL) const;
	// Search for Isotopologue by name
	Isotopologue* findIsotopologue(const char* name) const;
	// Return index of specified Isotopologue
	int indexOfIsotopologue(Isotopologue* iso) const;


	/*
	 * Site
	 */
	private:
	// List of defined sites
	List<SpeciesSite> sites_;

	public:
	// Add a new SpeciesSite to this Species
	SpeciesSite* addSite(const char* name = NULL);
	// Remove specified SpeciesSite
	void removeSite(SpeciesSite* site);
	// Return number of defined SpeciesSites
	int nSites() const;
	// Return SpeciesSite List
	const List<SpeciesSite>& sites() const;
	// Return nth SpeciesSite defined
	SpeciesSite* site(int n);
	// Generate unique site name with base name provided
	const char* uniqueSiteName(const char* baseName, SpeciesSite* exclude = NULL) const;
	// Search for SpeciesSite by name
	SpeciesSite* findSite(const char* name) const;


	/*
	 * Transforms
	 */
	public:
	// Calculate and return centre of geometry
	Vec3<double> centreOfGeometry(const Box* box) const;
	// Set centre of geometry
	void setCentre(const Box* box, const Vec3<double> newCentre);
	// Centre coordinates at origin
	void centreAtOrigin();


	/*
	 * File Input / Output
	 */
	public:
	// Load Species information from XYZ file
	bool loadFromXYZ(const char* filename);
	// Load Species from file
	bool load(const char* filename);


	/*
	 * Read / Write
	 */
	public:
	// Species Block Keyword Enum
	enum SpeciesKeyword
	{
		AngleKeyword,			/* 'Angle' - Defines an Angle joining three atoms */
		AtomKeyword,			/* 'Atom' - Specifies an Atom in the Species */
		AutoAddGrainsKeyword,		/* 'AutoAddGrains' - Automatically add Grains to cover all atoms in the Species */
		BondKeyword,			/* 'Bond' - Defines a Bond joining two atoms */
		BondTypeKeyword,		/* 'BondType' - Sets the type of a specific bond */
		ChargeKeyword,			/* 'Charge' - Specifies the atomic charge for an individual atom */
		EndSpeciesKeyword,		/* 'EndSpecies' - Signals the end of the current Species */
		ForcefieldKeyword,		/* 'Forcefield' - Sets the Forcefield from which to (re)generate or set terms */
		GrainKeyword,			/* 'Grain' - Defines a Grain containing a number of Atoms */
		IsotopologueKeyword,		/* 'Isotopologue' - Add an isotopologue to the Species */
		SiteKeyword,			/* 'Site' - Define an analysis site within the Species */
		TorsionKeyword,			/* 'Torsion' - Define a Torsion interaction between four atoms */
		nSpeciesKeywords		/* Number of keywords defined for this block */
	};
	// Return enum option info for SpeciesKeyword
	static EnumOptions<Species::SpeciesKeyword> keywords();
	// Read Species definition from specified LineParser
	bool read(LineParser& parser, CoreData& coreData);
	// Write Species definition to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif
