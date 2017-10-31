/*
	*** dUQ Main Structure
	*** src/main/duq.h
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

#ifndef DUQ_DUQ_H
#define DUQ_DUQ_H

#include "base/ptable.h"
#include "module/module.h"
#include "classes/configuration.h"
#include "classes/data.h"
#include "classes/pairpotential.h"
#include "classes/potentialmap.h"

// Forward Declarations
class Atom;
class Box;
class Cell;
class Grain;
class Isotopologue;
class Molecule;
class ChangeStore;
class ModuleProxyBase;

// dUQ Main Class
class DUQ
{
	public:
	// Constructor
	DUQ();
	// Destructor
	~DUQ();
	// Clear all data
	void clear();


	/*
	 * Atom Types
	 */
	private:
	// Master list of allowed AtomTypes for all Species
	List<AtomType> atomTypes_;
	
	public:
	// Add AtomType
	AtomType* addAtomType(int el);
	// Return number of AtomTypes in list
	int nAtomTypes() const;
	// Return first AtomType in list
	AtomType* atomTypes() const;
	// Return AtomTypes list
	const List<AtomType>& atomTypeList() const;
	// Return nth AtomType in list
	AtomType* atomType(int n);
	// Generate unique AtomType name with base name provided
	const char* uniqueAtomTypeName(const char* base, AtomType* exclude = 0) const;
	// Search for AtomType by name
	AtomType* findAtomType(const char* name) const;


	/*
	 * Species Definitions
	 */
	private:
	// List of defined Species
	List<Species> species_;
	
	public:
	// Add a new Species to the list
	Species* addSpecies();
	// Remove the specified Species from the list
	void removeSpecies(Species* sp);
	// Return number of defined Species
	int nSpecies() const;
	// Return first Species in the list
	Species* species() const;
	// Return nth Species in the list
	Species* species(int n);
	// Search for Species by name
	Species* findSpecies(const char* name) const;
	// Update Species (or all) Isotopologues (or specified)
	void updateIsotopologues(Species* species = NULL, Isotopologue* iso = NULL);
	// Remove Isotopologue from Species
	void removeSpeciesIsotopologue(Species* species, Isotopologue* iso);


	/*
	 * Pair Potentials
	 */
	private:
	// Maximum distance for tabulated PairPotentials
	double pairPotentialRange_;
	// Maximum squared distance for tabulated PairPotentials
	double pairPotentialRangeSquared_;
	// Width of PairPotential tail over which to truncate
	double pairPotentialTruncationWidth_;
	// Delta to use in tabulation
	double pairPotentialDelta_;
	// Whether Coulomb terms are included in generated PairPotentials
	bool pairPotentialsIncludeCoulomb_;
	// Simulation PairPotentials
	List<PairPotential> pairPotentials_;
	// Map for PairPotentials
	PotentialMap potentialMap_;

	public:
	// Set maximum distance for tabulated PairPotentials
	void setPairPotentialRange(double range);
	// Return maximum distance for tabulated PairPotentials
	double pairPotentialRange() const;
	// Return maximum squared distance for tabulated PairPotentials
	double pairPotentialRangeSquared() const;
	// Set width of PairPotential tail over which to truncate
	void setPairPotentialTruncationWidth(double width);
	// Return width of PairPotential tail over which to truncate
	double pairPotentialTruncationWidth() const;
	// Set delta to use in tabulations
	void setPairPotentialDelta(double delta);
	// Return delta to use in tabulations
	double pairPotentialDelta() const;
	// Set whether Coulomb term should be included in generated PairPotentials
	void setPairPotentialsIncludeCoulomb(bool b);
	// Return whether Coulomb term should be included in generated PairPotentials
	bool pairPotentialsIncludeCoulomb();
	// Return index of specified PairPotential
	int indexOf(PairPotential* pp);
	// Return number of defined PairPotentials
	int nPairPotentials() const;
	// Add new pair potential to list
	PairPotential* addPairPotential(AtomType* at1, AtomType* at2);
	// Return first PairPotential in list
	PairPotential* pairPotentials() const;
	// Return nth PairPotential in list
	PairPotential* pairPotential(int n);
	// Return whether specified PairPotential is defined
	PairPotential* pairPotential(AtomType* at1, AtomType* at2) const;
	// Save all PairPotentials
	bool savePairPotentials(const char* baseName) const;
	// Return map for PairPotentials
	const PotentialMap& potentialMap();


	/*
	 * Configurations
	 */
	private:
	// List of all atomic configurations
	List<Configuration> configurations_;

	public:
	// Add new Configuration
	Configuration* addConfiguration();
	// Find configuration by name
	Configuration* findConfiguration(const char* name, bool useNiceName = false) const;


	/*
	 * Data
	 */
	private:
	List<Data> data_;

	public:
	// Add new Data
	Data* addData();
	// Find Data by name
	Data* findData(const char* name, bool useNiceName = false) const;


	/*
	 * Simulation
	 */
	private:
	// List of Modules with pre-processing tasks to perform
	RefList<Module,bool> preProcessingTasks_;
	// List of main processing Modules to run
	ModuleList processingModules_;
	// Data associated with main processing Modules
	GenericList processingModuleData_;
	// List of Modules with post-processing tasks to perform
	RefList<Module,bool> postProcessingTasks_;
	// Current simulation step
	int iteration_;

	private:
	// Find first occurrence of named Module in pre-processing tasks
	Module* findPreProcessingTask(const char* name);
	// Find first occurrence of named Module in post-processing tasks
	Module* findPostProcessingTask(const char* name);

	public:
	// Run main simulation
	bool go(bool singleIteration);
	// Return current simulation step
	int iteration();
	// Return data associated with main processing Modules
	GenericList& processingModuleData();


	/*
	 * Simulation Set up
	 */
	private:
	// Number of test points to use when calculating Box normalisation arrays
	int nBoxNormalisationPoints_;
	// Random seed
	int seed_;
	// Maximum number of main loop iterations to perform
	int maxIterations_;
	// Window function to use for all Fourier transforms
	XYData::WindowFunction windowFunction_;
	// Whether to write various data after each iteration
	bool writeIterationData_;
	// Whether all intramolecular interactions contribute to intra partials
	bool allIntra_;

	public:
	// Set number of test points to use when calculating Box normalisation arrays
	void setNBoxNormalisationPoints(int nPoints);
	// Return number of test points to use when calculating Box normalisation arrays
	int nBoxNormalisationPoints() const;
	// Set random seed
	void setSeed(int seed);
	// Return random seed
	int seed();
	// Set maximum number of main loop iterations to perform
	void setMaxIterations(int nIterations);
	// Return maximum number of main loop iterations to perform
	int maxIterations();
	// Set window function to use for all Fourier transforms
	void setWindowFunction(XYData::WindowFunction wf);
	// Return window function to use for all Fourier transforms
	XYData::WindowFunction windowFunction();
	// Set whether to write various data after each iteration
	void setWriteIterationData(bool b);
	// Return whether to write various data after each iteration
	bool writeIterationData();
	// Set up all simulation data, checking it as we go
	bool setUpSimulation();


	/*
	 * Energy Methods
	 */
	public:
	// Return total intramolecular energy
	double intramolecularEnergy(ProcessPool& procPool, Configuration* cfg);
	// Return total interatomic energy
	double interatomicEnergy(ProcessPool& procPool, Configuration* cfg);
	// Test - Return total energy of the system using Molecules
	double totalEnergyTestMolecules(Configuration* cfg);


	/*
	 * Force Methods
	 */
	public:
	// Calculate total intramolecular forces
	void intramolecularForces(ProcessPool& procPool, Configuration* cfg, Array<double>& fx, Array<double>& fy, Array<double>& fz);
	// Calculate interatomic forces within the specified Configuration
	void interatomicForces(ProcessPool& procPool, Configuration* cfg, Array<double>& fx, Array<double>& fy, Array<double>& fz);
	// Calculate total forces within the system
	void totalForces(ProcessPool& procPool, Configuration* cfg, Array<double>& fx, Array<double>& fy, Array<double>& fz);


	/*
	 * I/O
	 */
	private:
	// Filename of current input file
	CharString filename_;
	// Whether to automatically add dependent Modules if they have not been defined
	bool autoAddDependentModules_;

	public:
	// Load datafiles
	bool loadDataFiles();
	// Load Species from specified file
	bool loadSpecies(const char* filename);
	// Load input file
	bool loadInput(const char* filename);
	// Save input file
	bool saveInput(const char* filename);
	// Load restart file
	bool loadRestart(const char* filename);
	// Save restart file
	bool saveRestart(const char* filename);
	// Dump additional data
	void dump();
	// Return whether a filename has been set
	bool hasFileName() const;
	// Return filename of current input file
	const char* filename() const;
	// Set whether to automatically add dependent Modules if they have not been defined
	void setAutoAddDependentModules(bool b);
	// Return whether to automatically add dependent Modules if they have not been defined
	bool autoAddDependentModules();


	/*
	 * Parallel Comms
	 */
	public:
	// Parallel Strategy
	enum ParallelStrategy { SequentialConfigStrategy, EvenStrategy, nParallelStrategies };
	// Convert string to ParallelStrategy
	static ParallelStrategy parallelStrategy(const char* s);

	private:
	// Parallel strategy for Configuration work
	ParallelStrategy parallelStrategy_;
	// Pool containing all available processes
	ProcessPool worldPool_;

	public:
	// Set parallel strategy for Configuration work
	void setParallelStrategy(ParallelStrategy ps);
	// Return parallel strategy for Configuration work
	ParallelStrategy parallelStrategy();
	// Return world process pool
	ProcessPool& worldPool();
	// Set up local MPI pools
	bool setUpMPIPools();
};

#endif
