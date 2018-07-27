/*
	*** Species Definition
	*** src/classes/species.h
	Copyright T. Youngs 2012-2018

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

#include "templates/list.h"
#include "templates/array2d.h"
#include "classes/speciesangle.h"
#include "classes/speciesatom.h"
#include "classes/speciesbond.h"
#include "classes/speciestorsion.h"
#include "classes/speciesgrain.h"
#include "classes/isotopologue.h"
#include "base/charstring.h"

// Forward Declarations
class Box;

/*
 * Species Definition
 */
class Species : public ListItem<Species>
{
	public:
	// Constructor
	Species();
	// Destructor
	~Species();
	// Clear Data
	void clear();


	/*
	 * Basic Information
	 */
	private:
	// Name of the Species
	CharString name_;

	public:
	// Set name of the Species
	void setName(const char* name);
	// Return the name of the Species
	const char* name() const;
	// Check setup
	bool checkSetup(const List<AtomType>& atomTypes);
	// Print Species information
	void print();


	/*
	 * Atomic Information
	 */
	private:
	// List of Atoms in the Species
	List<SpeciesAtom> atoms_;
	// List of selected Atoms
	RefList<SpeciesAtom,int> selectedAtoms_;
	
	public:
	// Add a new Atom to the Species
	SpeciesAtom* addAtom(Element* element = NULL, double rx = 0.0, double ry = 0.0, double rz = 0.0);
	// Return the number of atoms in the species
	int nAtoms() const;
	// Return the first atom in the Species
	SpeciesAtom* firstAtom() const;
	// Return the nth atom in the Species
	SpeciesAtom* atom(int n);
	// Return the list of SpeciesAtoms
	List<SpeciesAtom>& atoms();
	// Clear current Atom selection
	void clearAtomSelection();
	// Add Atom to selection
	void selectAtom(SpeciesAtom* i);
	// Select Atoms along any path from the specified one
	void selectFromAtom(SpeciesAtom* i, SpeciesBond* exclude);
	// Return first selected Atom reference
	RefListItem<SpeciesAtom,int>* selectedAtoms() const;
	// Return nth selected Atom
	SpeciesAtom* selectedAtom(int n);
	// Return number of selected Atoms
	int nSelectedAtoms() const;
	// Return whether specified Atom is selected
	bool isAtomSelected(SpeciesAtom* i) const;
	// Change element of specified Atom
	void changeAtomElement(SpeciesAtom* i, int Z, AtomType* at);
	// Return total atomic mass of Species
	double mass() const;


	/*
	 * Intramolecular Data
	 */
	private:
	// List of Bonds between Atoms in the Species
	List<SpeciesBond> bonds_;
	// List of Angles between Atoms in the Species
	List<SpeciesAngle> angles_;
	// List of Torsions between Atoms in the Species
	List<SpeciesTorsion> torsions_;

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
	// Return whether SpeciesBond between SpeciesAtomss exists
	SpeciesBond* hasBond(SpeciesAtom* i, SpeciesAtom* j) const;
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
	bool hasAngle(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k) const;
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
	bool hasTorsion(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l) const;
	// Return index of specified SpeciesTorsion
	int torsionIndex(SpeciesTorsion* spt);


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
	void updateIsotopologues(const List<AtomType>& atomTypes);
	// Add a new Isotopologue to this Species
	Isotopologue* addIsotopologue(const char* baseName = "UnnamedIsotopologue");
	// Add natural isotopologue to this species, if it hasn't already been defined
	Isotopologue* addNaturalIsotopologue();
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
};

#endif
