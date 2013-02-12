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
#include "classes/angle.h"
#include "classes/atom.h"
#include "classes/bond.h"
#include "classes/isotopologue.h"
#include "classes/grain.h"
#include "classes/graindefinition.h"
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
	List<Atom> atoms_;
	// Selected Atoms (for GUI-based manipulation)
	RefList<Atom,int> selectedAtoms_;
	
	public:
	// Add a new Atom to the Species
	Atom* addAtom(int element = 0, double rx = 0.0, double ry = 0.0, double rz = 0.0);
	// Return the number of atoms in the species
	int nAtoms() const;
	// Return the first atom in the Species
	Atom* atoms() const;
	// Return the nth atom in the Species
	Atom* atom(int n);
	// Clear current Atom selection
	void clearAtomSelection();
	// Add Atom to selection
	void selectAtom(Atom* i);
	// Select Atoms along any path from the specified one
	void selectFromAtom(Atom* i, Bond* exclude = NULL);
	// Return first selected Atom reference
	RefListItem<Atom,int>* selectedAtoms() const;
	// Return nth selected Atom
	Atom* selectedAtom(int n);
	// Return number of selected Atoms
	int nSelectedAtoms() const;
	// Return whether specified Atom is selected
	bool isAtomSelected(Atom* i) const;
	// Change element of specified Atom
	void changeAtomElement(Atom* i, int el, AtomType* at);
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
	List<Bond> bonds_;
	// List of Angles between Atoms in the Species
	List<Angle> angles_;

	public:
	// Add new Bond definition (from Atoms*)
	Bond* addBond(Atom* i, Atom* j);
	// Add new Bond definition
	Bond* addBond(int i, int j);
	// Return number of Bonds in list
	int nBonds() const;
	// Return list of Bonds
	Bond* bonds() const;
	// Return nth Bond
	Bond* bond(int n);
	// Return whether Bond between Atoms exists
	Bond* hasBond(Atom* i, Atom* j) const;
	// Add new Angle definition (from Atoms*)
	Angle* addAngle(Atom* i, Atom* j, Atom* k);
	// Add new Angle definition
	Angle* addAngle(int i, int j, int k);
	// Return number of Angles in list
	int nAngles() const;
	// Return list of Angles
	Angle* angles() const;
	// Return nth Angle
	Angle* angle(int n);
	// Return whether Angle between Atoms exists
	bool hasAngle(Atom* i, Atom* j, Atom* k) const;
	// Recalculate intramolecular terms between atoms in the Species
	void recalculateIntramolecular();
	// Calculate local Atom index lists for interactions
	bool calculateIndexLists();
	///@}


	/*!
	 * \name GrainDefinitions
	 */
	///@{
	private:
	// List of GrainDefinitions, dividing the Atoms of this Species into individual groups
	List<GrainDefinition> grainDefinitions_;
	// Highlighted GrainDefinition (in Viewer)
	GrainDefinition* highlightedGrainDefinition_;
	
	public:
	// Update GrainDefinitions after change
	void updateGrainDefinitions();
	// Add default GrainDefinition (i.e. one which contains all atoms) for this Species 
	void addDefaultGrainDefinition();
	// Automatically determine Grains based on chemical connectivity
	void autoAddGrains();
	// Add new GrainDefinition for this Species
	GrainDefinition* addGrainDefinition();
	// Remove GrainDefinition with ID specified
	void removeGrainDefinition(GrainDefinition* gd);
	// Return number of GrainDefinitions present for this Species
	int nGrainDefinitions() const;
	// Return first GrainDefinition in list
	GrainDefinition* grainDefinitions() const;
	// Return nth GrainDefinition in list
	GrainDefinition* grainDefinition(int n);
	// Add Atom to GrainDefinition
	void addAtomToGrainDefinition(Atom* i, GrainDefinition* gd);
	// Generate unique GrainDefinition name with base name provided
	const char* uniqueGrainDefinitionName(const char* baseName, GrainDefinition* exclude = NULL) const;
	// Order Atoms within Grains
	void orderAtomsWithinGrains();
	// Set highlighted GrainDefinition
	void setHighlightedGrainDefinition(GrainDefinition *gd);
	// Return highlighted GrainDefinition
	GrainDefinition* highlightedGrainDefinition();
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
