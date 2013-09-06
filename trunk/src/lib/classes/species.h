/*
	*** Species Definition
	*** src/lib/classes/species.h
	Copyright T. Youngs 2012-2013

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_SPECIES_H
#define DUQ_SPECIES_H

#include "templates/list.h"
#include "templates/array.h"
#include "classes/speciesangle.h"
#include "classes/speciesatom.h"
#include "classes/speciesbond.h"
#include "classes/speciesgrain.h"
#include "classes/isotopologue.h"
#include "base/dnchar.h"

// Forward Declarations
/* None */

/*!
 * \brief Species Definition
 * \details A Species is a distinct molecule type, ion, or fragment present in the real system. For instance, a water molecule, sodium ion,
 * a protein, or a single atom species such as Si (as might be used, e.g. for simulations of solid frameworks). The underlying Model for
 * the system is constructed from these species, mixed in the correct proportion (i.e. molar ratios). The basic Species does not define
 * the isotopic make-up of its atoms - these are added as Isotopologues within the Species.
 * 
 * <b>Granularity</b>
 * 
 * At its simplest, the Species can  be treated as a single Grain - that is, dUQ considers it is a single collection of atoms
 * when performing the main calculation. However, in many circumstances it is desirable to define separate parts of the molecule as
 * distinct Grains, since this allows many aspects of the calculation to be optimised.  For instance, a water molecule should be 
 * treated as a single Grain (since it contains only three atoms as well as being pseudospherical), but a larger, more flexible
 * species (e.g. a long alkane, drug molecule, or protein chain) will benefit from being split up into smaller Grains.
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


	/*!
	 * \name Basic Information
	 */
	///@{
	private:
	// Name of the Species
	Dnchar name_;
	// Relative population in the system
	double relativePopulation_;

	public:
	// Set name of the Species
	void setName(const char* name);
	// Return the name of the Species
	const char* name() const;
	// Set relative population in the system
	void setRelativePopulation(double relPop);
	// Return relative population in the system
	double relativePopulation();
	// Check setup
	int checkSetup(const List<AtomType>& atomTypes);
	///@}


	/*!
	 * \name Atomic Information
	*/
	///@{
	private:
	// List of Atoms in the Species
	List<SpeciesAtom> atoms_;
	// Selected Atoms (for GUI-based manipulation)
	RefList<SpeciesAtom,int> selectedAtoms_;
	
	public:
	// Add a new Atom to the Species
	SpeciesAtom* addAtom(int element = 0, double rx = 0.0, double ry = 0.0, double rz = 0.0);
	// Return the number of atoms in the species
	int nAtoms() const;
	// Return the first atom in the Species
	SpeciesAtom* atoms() const;
	// Return the nth atom in the Species
	SpeciesAtom* atom(int n);
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
	void changeAtomElement(SpeciesAtom* i, int el, AtomType* at);
	// Return total atomic mass of Species
	double mass() const;
	// Centre coordinates at origin
	void centreAtOrigin();
	///@}


	/*!
	 * \name Intramolecular Data
	 */
	///@{
	private:
	// List of Bonds between Atoms in the Species
	List<SpeciesBond> bonds_;
	// List of Angles between Atoms in the Species
	List<SpeciesAngle> angles_;
	// Scaling matrix for intramolecular interactions
	Array2D<double> scalingMatrix_;
	
	public:
	// Add new Bond definition (from SpeciesAtom*)
	SpeciesBond* addBond(SpeciesAtom* i, SpeciesAtom* j);
	// Add new Bond definition
	SpeciesBond* addBond(int i, int j);
	// Return number of Bonds in list
	int nBonds() const;
	// Return list of Bonds
	SpeciesBond* bonds() const;
	// Return nth Bond
	SpeciesBond* bond(int n);
	// Return whether Bond between Atoms exists
	SpeciesBond* hasBond(SpeciesAtom* i, SpeciesAtom* j) const;
	// Add new Angle definition (from Atoms*)
	SpeciesAngle* addAngle(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k);
	// Add new Angle definition
	SpeciesAngle* addAngle(int i, int j, int k);
	// Return number of Angles in list
	int nAngles() const;
	// Return list of Angles
	SpeciesAngle* angles() const;
	// Return nth Angle
	SpeciesAngle* angle(int n);
	// Return whether Angle between Atoms exists
	bool hasAngle(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k) const;
	// Recalculate intramolecular terms between atoms in the Species
	void recalculateIntramolecular();
	// Calculate local Atom index lists for interactions
	bool calculateIndexLists();
	// Create scaling matrix
	void createScalingMatrix();
	// Return scaling factor for supplied indices
	double scaling(int indexI, int indexJ);
	// Identify inter-Grain terms
	void identifyInterGrainTerms();
	///@}


	/*!
	 * \name Grains
	 */
	///@{
	private:
	// List of grain, dividing the Atoms of this Species into individual groups
	List<SpeciesGrain> grains_;
	// Highlighted grain (in Viewer)
	SpeciesGrain* highlightedGrain_;
	
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
	// Set highlighted grain
	void setHighlightedGrain(SpeciesGrain* gd);
	// Return highlighted grain
	SpeciesGrain* highlightedGrain();
	///@}


	/*!
	 * \name Isotopologues
	 */
	///@{
	private:
	// List of isotopic variants defined for this species
	List<Isotopologue> isotopologues_;
	// Highlighted Isotopologue (in Viewer)
	Isotopologue* highlightedIsotopologue_;

	public:
	// Update current Isotopologues
	void updateIsotopologues(const List<AtomType>& atomTypes);
	// Add a new Isotopologue to this Species
	Isotopologue* addIsotopologue(const char* baseName = "UnnamedIsotopologue");
	// Remove specified Isotopologue from this Species
	void removeIsotopologue(Isotopologue* iso);
	// Return number of defined Isotopologues
	int nIsotopologues() const;
	// Return first Isotopologue defined
	Isotopologue* isotopologues() const;
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
	// Set highlighted Isotopologue
	void setHighlightedIsotopologue(Isotopologue* iso);
	// Return highlighted Isotopologue
	Isotopologue* highlightedIsotopologue();
	///@}


	/*!
	 * \name File Input / Output
	 */
	///@{
	public:
	// Load Species information from XYZ file
	bool loadFromXYZ(const char* fileName);
	// Load Species from file
	bool load(const char* fileName);
	///@}


	/*!
	 * \name Parallel Comms
	 */
	///@{
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(const List<AtomType>& atomTypes);
	///@}
};

#endif
