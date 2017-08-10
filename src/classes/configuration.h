/*
	*** Configuration
	*** src/classes/configuration.h
	Copyright T. Youngs 2012-2017

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

#ifndef DUQ_CONFIGURATION_H
#define DUQ_CONFIGURATION_H

#include "classes/atomtypelist.h"
#include "classes/cellarray.h"
#include "classes/molecule.h"
#include "classes/histogram.h"
#include "modules/modulelist.h"
#include "base/xydata.h"
#include "base/processpool.h"
#include "templates/genericlist.h"
#include "templates/vector3.h"
#include "templates/orderedlist.h"
#include "templates/array.h"

// Forward Declarations
class Atom;
class Box;
class Grain;
class Cell;
class Species;

// Configuration
class Configuration : public ListItem<Configuration>
{
	public:
	// Constructor
	Configuration();
	// Destructor
	~Configuration();
	// Assignment operator
	void operator=(Configuration& source);


	/*
	 * Composition
	 */
	private:
	// Name of the configuration
	CharString name_;
	// Nice name (generated from name_) used for output files
	CharString niceName_;
	// Reference list of Species used by the Configuration and their relative populations
	RefList<Species,double> usedSpecies_;
	// Integer multiplier of used relative species populations
	int multiplier_;
	// Density of the configuration
	double density_;
	// Whether the density is in atomic units (atoms/A3) or chemistry units (g/cm3)
	bool densityIsAtomic_;
	// Whether a random config should be generated (as opposed to loading one from a file)
	bool randomConfiguration_;
	// File containing input coordinates
	CharString inputCoordinatesFile_;
	// Format of input coordinates file
	CharString inputCoordinatesFormat_;
	// File containing output coordinates
	CharString outputCoordinatesFile_;
	// Flag specifying to use output coordinates file as input coordinates (if it exists)
	bool useOutputCoordinatesAsInput_;
	// Frequency with which to write output coordinates
	int coordinatesOutputFrequency_;
	// Temperature of this configuration (K)
	double temperature_;

	public:
	// Set name of the configuration
	void setName(const char* name);
	// Return name of the configuration
	const char* name();
	// Return nice name of the configuration
	const char* niceName();
	// Add Species to list of those used by the Configuration
	bool addUsedSpecies(Species* sp, double relativePopulation);
	// Return reference list of Species used by the Configuration and their relative populations
	RefList<Species,double>& usedSpecies();
	// Return total relative population of Species used by this Configuration
	double totalRelative() const;
	// Set multiplier for System components
	void setMultiplier(int mult);
	// Return multiplier for System components
	int multiplier() const;
	// Set the atomic density of the system (atoms/A3)
	void setAtomicDensity(double density);
	// Set the chemical density of the system (g/cm3)
	void setChemicalDensity(double density);
	// Return the density of the system
	double density() const;
	// Return whether the density is in atomic units (atoms/A3) or chemistry units (g/cm3)
	bool densityIsAtomic() const;
	// Return the atomic density of the system
	double atomicDensity() const;
	// Set whether a random config should be generated (as opposed to loading one from a file)
	void setRandomConfiguration(bool b);
	// Return whether a random config should be generated (as opposed to loading one from a file)
	bool randomConfiguration();
	// Set file containing input coordinates
	void setInputCoordinatesFile(const char* filename);
	// Return file containing input coordinates
	const char* inputCoordinatesFile();
	// Set input coordinates file format
	void setInputCoordinatesFormat(const char* format);
	// Return input coordinates file format
	const char* inputCoordinatesFormat();
	// Set file containing output coordinates
	void setOutputCoordinatesFile(const char* filename);
	// Return file containing output coordinates
	const char* outputCoordinatesFile();
	// Set whether to use output coordinates file as input coordinates (if it exists)
	void setUseOutputCoordinatesAsInput(bool b);
	// Return whether to use output coordinates file as input coordinates (if it exists)
	bool useOutputCoordinatesAsInput();
	// Set frequency with which to write output coordinates
	void setCoordinatesOutputFrequency(int freq);
	// Return frequency with which to write output coordinates
	int coordinatesOutputFrequency();
	// Set configuration temperature
	void setTemperature(double t);
	// Return configuration temperature
	double temperature();
	// Clear all data
	void clear();
	// Setup configuration
	bool setup(ProcessPool& procPool, const List<AtomType>& atomTypes, double pairPotentialRange, int boxNormalisationNPoints);


	/*
	 * Content
	 */
	private:
	// Molecule list
	List<Molecule> molecules_;
	// Number of grains in array
	int nGrains_;
	// Grain array
	Grain* grains_;
	// Number of atoms in configuration / reference array
	int nAtoms_;
	// Array of atoms
	Atom* atoms_;
	// AtomType list, containing unique (non-isotopic) atom types over all Species used in this configuration
	AtomTypeList usedAtomTypes_;
	// Coordinate index, incremented whenever Atom positions change
	int coordinateIndex_;

	public:
	// Add space for a molecule of the Species provided
	void addMolecule(Species* sp);
	// Return number of molecules
	int nMolecules() const;
	// Return first molecule
	Molecule* molecules();
	// Return specified molecule
	Molecule* molecule(int id);
	// Return number of grains
	int nGrains() const;
	// Return grain array
	Grain* grains();
	// Return nth grain
	Grain& grain(int n);
	// Return number of atoms
	int nAtoms() const;
	// Return atom array
	Atom* atoms();
	// Return nth atom
	Atom* atom(int n);
	// Create Atom and Grain arrays
	bool createArrays();
	// Setup Molecule information
	bool setupMolecules();
	// Return specified type
	AtomType* type(int index);
	// Return first AtomTypeData for this Configuration
	AtomTypeData* usedAtomTypes();
	// Return AtomTypeList for this Configuration
	const AtomTypeList& usedAtomTypesList() const;
	// Return number of atom types used in this Configuration
	int nUsedAtomTypes();
	// Set master AtomType indices in Atoms from the map provided
	bool setMasterAtomTypeIndices(const List<AtomType>& masterList);
	// Return current coordinate index
	int coordinateIndex();
	// Increment current coordinate index
	void incrementCoordinateIndex();
	// Load coordinates from specified parser
	bool loadCoordinates(LineParser& parser, const char* format);
	// Create random configuration
	bool createRandom(ProcessPool& procPool);


	/*
	 * Periodic Box and Cells
	 */
	private:
	// Relative Box lengths
	Vec3<double> relativeBoxLengths_;
	// Box angles
	Vec3<double> boxAngles_;
	// Whether the configuration is non-periodic
	bool nonPeriodic_;
	// Periodic Box definition for the Model
	Box* box_;
	// Requested side length for individual Cell
	double requestedCellDivisionLength_;
	// Box normalisation array to load/save for this configuration
	CharString boxNormalisationFileName_;
	// RDF Normalisation function for Box shape/extent
	XYData boxNormalisation_;
	// Cell array
	CellArray cells_;

	public:
	// Set relative box lengths
	void setRelativeBoxLengths(const Vec3<double> lengths);
	// Return relative box lengths
	Vec3<double> relativeBoxLengths() const;
	// Set box angles
	void setBoxAngles(const Vec3<double> angles);
	// Return box angles
	Vec3<double> boxAngles() const;
	// Set whether the box is non-periodic
	void setNonPeriodic(bool b);
	// Return whether the box is non-periodic
	bool nonPeriodic() const;
	// Set requested side length for individual Cell
	void setRequestedCellDivisionLength(double a);
	// Return requested side length for individual Cell
	double requestedCellDivisionLength();
	// Return Box
	const Box* box() const;
	// Setup periodic Box
	bool setupBox(double ppRange);
	// Generate Cells for Box
	bool generateCells(double cellSize, double pairPotentialRange, double atomicDensity);
	// Set box normalisation array to load/save for this configuration
	void setBoxNormalisationFile(const char* filename);
	// Return box normalisation file to load/save for this configuration
	const char* boxNormalisationFileName() const;
	// Load Box normalisation array from specified file
	bool loadBoxNormalisationFile();
	// Return current Box normalisation array
	const XYData& boxNormalisation() const;
	// Return cell array
	CellArray& cells();


	/*
	 * Calculation Limits
	 */
	private:
	// RDF bin width
	double rdfBinWidth_;
	// Maximum extent (actual) of calculated RDFs
	double rdfRange_;
	// Maximum extent (requested) of calculated RDFs
	double requestedRDFRange_;
	// Minimum Q value for Bragg calculation
	double braggQMin_;
	// Maximum Q value for Bragg calculation
	double braggQMax_;
	// Multiplicities reflecting any crystal supercell
	Vec3<int> braggMultiplicity_;

	public:
	// Set RDF bin width
	void setRDFBinWidth(double width);
	// Return RDF bin width
	double rdfBinWidth();
	// Return working RDF extent
	double rdfRange();
	// Set requested RDF extent
	void setRequestedRDFRange(double range);
	// Return requested RDF extent
	double requestedRDFRange();
	// Set minimum Q value for Bragg calculation
	void setBraggQMin(double qMin);
	// Return minimum Q value for Bragg calculation
	double braggQMin();
	// Set maximum Q value for Bragg calculation
	void setBraggQMax(double qMax);
	// Return maximum Q value for Bragg calculation
	double braggQMax();
	// Set multiplicities reflecting any crystal supercell
	void setBraggMultiplicity(Vec3<int> mult);
	// Return multiplicities reflecting any crystal supercell
	Vec3<int> braggMultiplicity();


	/*
	 * Total Energy  (TO ENERGY MODULE????)
	 */
	private:
	// Configuration energy data
	XYData energyData_;
	// Energy change since last point added to energyData_
	double energyChange_;
	// Whether energy has changed since the last point was added
	bool energyChanged_;

	public:
	// Register a change in the total energy of the configuration
	void registerEnergyChange(double deltaE);
	// Accumulate current energy change into energyData_
	void accumulateEnergyChange();
	// Set absolute energy of system, after total energy calculation
	void setAbsoluteEnergy(double energy);


	/*
	 * Upkeep
	 */
	public:
	// Update cell locations of all atoms
	bool updateAtomsInCells();
	// Update cell locations of specified atom index, and update neighbour
	bool updateAtomInCell(int id);
	// Create cell atom neighbour lists
	void recreateCellAtomNeighbourLists(double pairPotentialRange);


	/*
	 * Modules
	 */
	private:
	// List of Modules associated to this Configuration
	ModuleList modules_;
	// Variables set by Modules
	GenericList moduleData_;

	public:
	// Add Module (or an instance of it) to the Configuration
	Module* addModule(Module* masterInstance, bool autoAddDependents);
	// Return number of Modules associated to this Configuration
	int nModules() const;
	// Return list of Modules associated to this Configuration
	RefList<Module,bool>& modules();
	// Return list of variables set by Modules
	GenericList& moduleData();


	/*
	 * Ensemble
	 */
	private:
	// Whether ensemble file is to be appended
	bool appendEnsemble_;
	// Frequency at which to append ensemble
	int ensembleFrequency_;

	public:
	// Set whether ensemble file is to be appended
	void setAppendEnsemble(bool b);
	// Return whether ensemble file is to be appended
	bool appendEnsemble();
	// Return frequency at which to append ensemble
	int setEnsembleFrequency(int frequency);
	// Return frequency at which to append ensemble
	int ensembleFrequency();


	/*
	 * Parallel Comms
	 */
	private:
	// Process pool for this Configuration
	ProcessPool processPool_;

	public:
	// Setup process pool for this Configuration
	bool setupProcessPool(Array<int> worldRanks);
	// Return process pool for this Configuration
	ProcessPool& processPool();
	// Broadcast coordinate from specified root process
	bool broadcastCoordinates(ProcessPool& procPool, int rootRank);
};

#endif
