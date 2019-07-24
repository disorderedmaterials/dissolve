/*
	*** Configuration
	*** src/classes/configuration.h
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

#ifndef DISSOLVE_CONFIGURATION_H
#define DISSOLVE_CONFIGURATION_H

#include "classes/atom.h"
#include "classes/angle.h"
#include "classes/bond.h"
#include "classes/torsion.h"
#include "classes/atomtypelist.h"
#include "classes/cellarray.h"
#include "classes/molecule.h"
#include "classes/speciesinfo.h"
#include "classes/sitestack.h"
#include "module/layer.h"
#include "io/import/coordinates.h"
#include "procedure/procedure.h"
#include "math/histogram1d.h"
#include "math/interpolator.h"
#include "math/data1d.h"
#include "base/processpool.h"
#include "genericitems/list.h"
#include "base/version.h"
#include "templates/vector3.h"
#include "templates/objectstore.h"
#include "templates/orderedlist.h"
#include "templates/array.h"
#include "templates/dynamicarray.h"

// Forward Declarations
class Atom;
class Box;
class Cell;
class Grain;
class PotentialMap;
class Species;

// Configuration
class Configuration : public ListItem<Configuration>, public ObjectStore<Configuration>
{
	public:
	// Constructor
	Configuration();
	// Destructor
	~Configuration();
	// Assignment operator
	void operator=(Configuration& source);
	// Clear all data
	void clear();


	/*
	 * Basic Information
	 */
	private:
	// Name of the Configuration
	CharString name_;
	// Nice name (generated from name_) used for output files
	CharString niceName_;

	public:
	// Set name of the Configuration
	void setName(const char* name);
	// Return name of the Configuration
	const char* name();
	// Return nice name of the Configuration
	const char* niceName();



	/*
	 * Composition
	 */
	private:
	// List of Species used by the Configuration and their populations
	List<SpeciesInfo> usedSpecies_;
	// File / format of input coordinates file
	CoordinateImportFileFormat inputCoordinates_;
	// Temperature of this configuration (K)
	double temperature_;

	public:
	// Add Species to list of those used by the Configuration, setting/adding the population specified
	SpeciesInfo* addUsedSpecies(Species* sp, int population);
	// Return SpeciesInfo for specified Species
	SpeciesInfo* usedSpeciesInfo(Species* sp);
	// Return list of SpeciesInfo for the Configuration
	List<SpeciesInfo>& usedSpecies();
	// Return if the specified Species is present in the usedSpecies list
	bool hasUsedSpecies(Species* sp);
	// Return the atomic density of the Configuration
	double atomicDensity() const;
	// Return import coordinates file / format
	CoordinateImportFileFormat& inputCoordinates();
	// Set configuration temperature
	void setTemperature(double t);
	// Return configuration temperature
	double temperature();


	/*
	 * Content
	 */
	private:
	// Procedure to generate the Configuration
	Procedure generator_;
	// Array of Molecules
	DynamicArray<Molecule> molecules_;
	// Array of Grains
	DynamicArray<Grain> grains_;
	// Array of Atoms
	DynamicArray<Atom> atoms_;
	// Array of Bonds between Atoms
	DynamicArray<Bond> bonds_;
	// Array of Angles between Atoms
	DynamicArray<Angle> angles_;
	// Array of Torsions between Atoms
	DynamicArray<Torsion> torsions_;
	// AtomType list, containing unique (non-isotopic) atom types over all Species used in this configuration
	AtomTypeList usedAtomTypes_;
	// Contents version, incremented whenever Configuration content or Atom positions change
	VersionCounter contentsVersion_;

	public:
	// Empty contents of Configuration, leaving core definitions intact
	void empty();
	// Initialise content arrays
	void initialiseArrays(int nMolecules, int nGrains);
	// Read generator from supplied parser
	bool readGenerator(LineParser& parser, const CoreData& coreData);
	// Write generator to supplied parser
	bool writeGenerator(LineParser& parser, const char* prefix);
	// Generate the Configuration ready for use, including Box and associated Cells
	bool generate(ProcessPool& procPool, double pairPotentialRange);
	// Finalise Configuration after loading contents from restart file
	bool finaliseAfterLoad(ProcessPool& procPool, double pairPotentialRange);
	// Add Molecule to Configuration based on the supplied Species
	Molecule* addMolecule(Species* sp);
	// Return number of Molecules in Configuration
	int nMolecules() const;
	// Return array of Molecules
	DynamicArray<Molecule>& molecules();
	// Return nth Molecule
	Molecule* molecule(int n);
	// Add new Grain to Configuration, with Molecule parent specified
	Grain* addGrain(Molecule* molecule);
	// Return number of Grains in Configuration
	int nGrains() const;
	// Return Grain array
	DynamicArray<Grain>& grains();
	// Return nth Grain
	Grain* grain(int n);
	// Add new Atom to Configuration, with Molecule and Grain parents specified
	Atom* addAtom(Molecule* molecule, Grain* grain = NULL);
	// Add new Atom with full data
	Atom* addAtom(Molecule* molecule, Grain* grain, AtomType* atomType, Vec3<double> r, double charge);
	// Return number of Atoms in Configuration
	int nAtoms() const;
	// Return Atom array
	DynamicArray<Atom>& atoms();
	// Return Atom array (const)
	const DynamicArray<Atom>& constAtoms() const;
	// Return nth Atom
	Atom* atom(int n);
	// Add new Bond to Configuration, with Molecule parent specified
	Bond* addBond(Molecule* molecule, Atom* i, Atom* j);
	// Add new Bond to Configuration, with Molecule parent specified, from Atom indices
	Bond* addBond(Molecule* molecule, int i, int j);
	// Return number of Bonds in Configuration
	int nBonds() const;
	// Return Bond array
	DynamicArray<Bond>& bonds();
	// Return Bond array (const)
	const DynamicArray<Bond>& constBonds() const;
	// Return nth Bond
	Bond* bond(int n);
	// Add new Angle to Configuration, with Molecule parent specified
	Angle* addAngle(Molecule* molecule, Atom* i, Atom* j, Atom* k);
	// Add new Angle to Configuration, with Molecule parent specified, from Atom indices
	Angle* addAngle(Molecule* molecule, int i, int j, int k);
	// Return number of Angles in Configuration
	int nAngles() const;
	// Return Angle array
	DynamicArray<Angle>& angles();
	// Return nth Angle
	Angle* angle(int n);
	// Add new Torsion to Configuration, with Molecule parent specified
	Torsion* addTorsion(Molecule* molecule, Atom* i, Atom* j, Atom* k, Atom* l);
	// Add new Torsion to Configuration, with Molecule parent specified, from Atom indices
	Torsion* addTorsion(Molecule* molecule, int i, int j, int k, int l);
	// Return number of Torsions in Configuration
	int nTorsions() const;
	// Return Torsion array
	DynamicArray<Torsion>& torsions();
	// Return nth Torsion
	Torsion* torsion(int n);
	// Return specified used type
	AtomType* usedAtomType(int index);
	// Return specified used type data
	AtomTypeData* usedAtomTypeData(int index);
	// Return first AtomTypeData for this Configuration
	AtomTypeData* usedAtomTypes();
	// Return AtomTypeList for this Configuration
	const AtomTypeList& usedAtomTypesList() const;
	// Return number of atom types used in this Configuration
	int nUsedAtomTypes() const;
	// Return version of current contents
	int contentsVersion() const;
	// Increment version of current contents
	void incrementContentsVersion();
	// Load coordinates from specified parser
	bool loadCoordinates(LineParser& parser, CoordinateImportFileFormat::CoordinateImportFormat format);


	/*
	 * Periodic Box and Cells
	 */
	private:
	// Requested size factor for Box
	double requestedSizeFactor_;
	// Size factor currently applied to Box / Cells
	double appliedSizeFactor_;
	// Periodic Box definition for the Configuration
	Box* box_;
	// Requested side length for individual Cell
	double requestedCellDivisionLength_;
	// Cell array
	CellArray cells_;

	public:
	// Create Box definition with specified lengths and angles
	bool createBox(const Vec3<double> lengths, const Vec3<double> angles, bool nonPeriodic = false);
	// Return Box
	const Box* box() const;
	// Scale Box (and associated Cells) by specified factor
	void scaleBox(double factor);
	// Set requested size factor for Box
	void setRequestedSizeFactor(double factor);
	// Return requested size factor for Box
	double requestedSizeFactor();
	// Return last size factor applied to Box / Cells
	double appliedSizeFactor();
	// Set requested side length for individual Cell
	void setRequestedCellDivisionLength(double a);
	// Return requested side length for individual Cell
	double requestedCellDivisionLength() const;
	// Return cell array
	CellArray& cells();
	// Return cell array
	const CellArray& constCells() const;


	/*
	 * Upkeep
	 */
	public:
	// Update Cell contents
	void updateCellContents();
	// Update Cell location of specified Atom
	void updateCellLocation(Atom* i);
	// Update Cell location of specified Molecule
	void updateCellLocation(Molecule* mol);
	// Update Cell location of specified Atom indices (in array)
	void updateCellLocation(int nIndices, int* atomIndices, int indexOffset);


	/*
	 * Modules
	 */
	private:
	// Module layer associated to this Configuration
	ModuleLayer moduleLayer_;
	// Variables set by Modules
	GenericList moduleData_;

	public:
	// Add Module to the Configuration
	bool addModule(Module* module);
	// Return number of Modules associated to this Configuration
	int nModules() const;
	// Return Module layer for this Configuration
	ModuleLayer& moduleLayer();
	// Return list of Modules associated to this Configuration
	ModuleList& modules();
	// Return list of variables set by Modules
	GenericList& moduleData();


	/*
	 * Preparation
	 */
	public:
	// Perform any preparative tasks for the Configuration, before Module processing begins
	bool prepare(const PotentialMap& potentialMap);


	/*
	 * Site Stacks
	 */
	private:
	// List of current SiteStacks
	List<SiteStack> siteStacks_;

	public:
	// Calculate / retrieve stack of sites for specified SpeciesSite
	const SiteStack* siteStack(SpeciesSite* site);


	/*
	 * Parallel Comms
	 */
	private:
	// Process pool for this Configuration
	ProcessPool processPool_;

	public:
	// Set up process pool for this Configuration
	bool setUpProcessPool(Array<int> worldRanks, int groupPopulation);
	// Return process pool for this Configuration
	ProcessPool& processPool();
	// Broadcast coordinate from specified root process
	bool broadcastCoordinates(ProcessPool& procPool, int rootRank);
};

#endif
