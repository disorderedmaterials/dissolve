/*
	*** Configuration
	*** src/classes/configuration.h
	Copyright T. Youngs 2012-2016

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
#include "classes/molecule.h"
#include "classes/histogram.h"
#include "classes/kvector.h"
#include "classes/braggpeak.h"
#include "classes/sample.h"
#include "modules/module.h"
#include "base/processpool.h"
#include "base/variablelist.h"
#include "templates/vector3.h"
#include "templates/orderedlist.h"
#include "templates/array.h"

// Forward Declarations
class Atom;
class Box;
class Grain;
class Cell;
class Species;

/*
 * Configuration
 */
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
	Dnchar name_;
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
	// File containing initial coordinates
	Dnchar initialCoordinatesFile_;
	// Temperature of this configuration (K)
	double temperature_;

	public:
	// Set name of the configuration
	void setName(const char* name);
	// Return name of the configuration
	const char* name();
	// Add Species to list of those used by the Configuration
	bool addUsedSpecies(Species* sp, double relativePopulation);
	// Return reference list of Species used by the Configuration and their relative populations
	RefListItem<Species,double>* usedSpecies();
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
	// Set file containing initial coordinates
	void setInitialCoordinatesFile(const char* fileName);
	// Return file containing initial coordinates
	const char* initialCoordinatesFile();
	// Set configuration temperature
	void setTemperature(double t);
	// Return configuration temperature
	double temperature();
	// Clear all data
	void clear();
	// Setup configuration
	bool setup(ProcessPool& procPool, const List< AtomType >& atomTypes, double pairPotentialRange, int boxNormalisationNPoints);


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
	// Return first AtomTypeData for this configuration
	AtomTypeData* usedAtomTypes();
	// Set global AtomType indices in Atoms from the map provided
	bool setGlobalAtomTypeIndices(const List<AtomType>& masterList);
	// Return current coordinate index
	int coordinateIndex();
	// Increment current coordinate index
	void incrementCoordinateIndex();
	// Load coordinates from specified file
	bool loadCoordinates(const char* filename);
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
	// Cell divisions along each axis
	Vec3<int> divisions_;
	// Fractional Cell size
	Vec3<double> cellSize_;
	// Real Cell size
	Vec3<double> realCellSize_;
	// Cell extents out from given central cell
	Vec3<int> cellExtents_;
	// List of Cell neighbour indices
	List< ListVec3<int> > cellNeighbourIndices_;
	// Total number of Cells in Box
	int nCells_;
	// Cell array (one-dimensional)
	Cell* cells_;
	// Cell modified/completed/calculated flag
	bool* cellFlag_;
	// Counter for distributed Cells
	int nCellsDistributed_;
	// Last Cell distributed
	int lastCellDistributed_;
	// Box normalisation array to load/save for this configuration
	Dnchar boxNormalisationFileName_;
	// RDF Normalisation function for Box shape/extent
	Data2D boxNormalisation_;
	
	private:
	// Clear Cell arrays
	void clearCells();
	// Return whether the contents of two Cells should be mim'd in calculations
	bool minimumImageRequired(Cell* a, Cell* b) const;

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
	// Return Box
	const Box* box() const;
	// Setup periodic Box
	bool setupBox(double ppRange);
	// Generate Cells for Box
	bool generateCells(double cellSize, double pairPotentialRange, double atomicDensity);
	// Return number of Cells for box
	int nCells() const;
	// Return real Cell dimensions
	Vec3<double> realCellSize() const;
	// Retrieve Cell with (wrapped) grid reference specified
	Cell* cell(int x, int y, int z) const;
	// Retrieve Cell with id specified
	Cell* cell(int id) const;
	// Return Cell which contains specified coordinate
	Cell* cell(const Vec3<double> r) const;
	// Return maximum number of atoms per cell
	int maxAtomsPerCell() const;
	// Return whether two Cells need minimum image calculation
	bool useMim(Cell* a, Cell* b) const;
	// Initialise Cells for distribution
	void initialiseCellDistribution();
	// Return next available Cell for calculation
	int nextAvailableCell(ProcessPool& procPool, bool willBeModified, bool allowRepeats);
	// Unlock Cell specified, once calculation is complete
	bool finishedWithCell(ProcessPool& procPool, bool willBeModified, int cellId);
	// Update cell locations of all atoms
	bool updateAtomsInCells();
	// Update cell locations of specified atom index, and update neighbour
	bool updateAtomInCell(int id);
	// Set box normalisation array to load/save for this configuration
	void setBoxNormalisationFile(const char* fileName);
	// Return box normalisation file to load/save for this configuration
	const char* boxNormalisationFileName() const;
	// Load Box normalisation array from specified file
	bool loadBoxNormalisationFile();
	// Return current Box normalisation array
	const Data2D& boxNormalisation() const;


	/*
	 * RDF / S(Q) Data
	 */
	private:
	// RDF bin width
	double rdfBinWidth_;
	// Maximum extent (actual) of calculated RDFs
	double rdfRange_;
	// Degree of smoothing to apply to calculated RDF data, if any
	int rdfSmoothing_;
	// Maximum extent (requested) of calculated RDFs
	double requestedRDFRange_;
	// Pair RDF matrix, containing full atom-atom RDFs
	Array2D<Histogram> pairRDFMatrix_;
	// Unbound RDF matrix, containing atom-atom RDFs of pairs not joined by bonds or angles
	Array2D<Histogram> unboundRDFMatrix_;
	// Bound RDF matrix, containing atom-atom RDFs of pairs joined by bonds or angles
	Array2D<Histogram> boundRDFMatrix_;
	// Pair correlation S(Q) matrix, derived from pairRDFMatrix_
	Array2D<Data2D> pairSQMatrix_;
	// Unbound S(Q) matrix, derived from unboundRDFMatrix_
	Array2D<Data2D> unboundSQMatrix_;
	// Bound S(Q) matrix, derived from boundRDFMatrix_
	Array2D<Data2D> boundSQMatrix_;
	// Bragg S(Q) matrix, derived from summation of HKL terms
	Array2D<Data2D> braggSQMatrix_;
	// Partial S(Q) matrix, containing full (pair + Bragg) contributions
	Array2D<Data2D> partialSQMatrix_;
	// Total RDF (unweighted)
	Data2D totalRDF_;
	// Total S(Q) (unweighted)
	Data2D totalFQ_;
	// Maximum Q value for Bragg calculation
	double braggMaximumQ_;
	// Maximal extent of hkl for Bragg calculation, based on braggMaximumQ_
	Vec3<int> braggMaximumHKL_;
	// Bragg calculation k-vector list
	List<KVector> braggKVectors_;
	// Bragg calculation peak list
	OrderedList<BraggPeak> braggPeaks_;
	// Bragg S(Q) working arrays
        Array2D<double> braggAtomVectorXCos_, braggAtomVectorYCos_, braggAtomVectorZCos_;
        Array2D<double> braggAtomVectorXSin_, braggAtomVectorYSin_, braggAtomVectorZSin_;
	// Configuration index at which partials were last calculated
	int partialsIndex_;
	// Configuration index at which Bragg peaks were last calculated
	int braggIndex_;

	private:
	// Calculate unweighted partials
	bool calculatePartialRDFs(ProcessPool& procPool);
	// Calculate unweighted intramolecular RDFs
	bool calculateIntramolecularRDFs(ProcessPool& procPool);
	// Calculate Bragg peak contributions
	bool calculateBraggContributions(ProcessPool& procPool);
	// Calculate weighted Bragg S(Q)
	bool calculateBraggSQ(const AtomTypeList& sampleAtomTypes, Array2D<Data2D>& braggMatrix, double broadening);
	
	public:
	// Set RDF bin width
	void setRDFBinWidth(double width);
	// Return RDF bin width
	double rdfBinWidth();
	// Return working RDF extent
	double rdfRange();
	// Set degree of smoothing to apply to calculated RDF data
	void setRDFSmoothing(int smoothingDegree);
	// Return degree of smoothing to apply to calculated RDF data
	int rdfSmoothing();
	// Set requested RDF extent
	void setRequestedRDFRange(double range);
	// Return requested RDF extent
	double requestedRDFRange();
	// Set maximum Q value for Bragg calculation
	void setBraggMaximumQ(double qMax);
	// Return maximum Q value for Bragg calculation
	double braggMaximumQ();
	// Return maximal extent of hkl for Bragg calculation
	Vec3<int> braggMaximumHKL();
	// Setup RDF and S(Q) storage
	bool setupPartials();
	// Reset partials
	void resetPartials();
	// Calculate unweighted partials
	bool calculatePartials(ProcessPool& procPool, double qDelta, double qMax, Data2D::WindowFunction windowFunction, bool braggOn);
	// Save RDFs
	void saveRDFs(const char* baseName);
	// Save S(Q)
	void saveSQ(const char* baseName);


	/*
	 * Modules
	 */
	private:
	// Modules associated to this Configuration
	RefList<Module,bool> modules_;
	// Variables set by Modules
	VariableList moduleVariables_;

	public:
	// Associate Module to Configuration
	Module* addModule(Module* module);
	// Find associated Module by name
	Module* findModule(const char* name);
	// Return number of Modules associated
	int nModules();
	// Return first Module of specified type
	RefList<Module,bool>& modules();
	// Set Module variable
	void setModuleVariable(const char* name, VariableValue value, const char* description, const char* source);
	// Return named Module variable
	Variable* moduleVariable(const char* name, const char* source);


	/*
	 * Total Energy  (TO ENERGY MODULE????)
	 */
	private:
	// Configuration energy data
	Data2D energyData_;
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
	// Update Grains
	void updateGrains();
	// Create cell atom neighbour lists
	void recreateCellAtomNeighbourLists(double pairPotentialRange);


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
	// Broadcast data
	bool broadcast(ProcessPool& procPool, const List<Species>& species, double pairPotentialRange, const RefList<Module,bool>& allModules);
	// Broadcast coordinate from specified root process
	bool broadcastCoordinates(ProcessPool& procPool, int rootRank);
};

#endif
