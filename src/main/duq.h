/*
	*** dUQ Main Structure
	*** src/main/duq.h
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

#ifndef DUQ_DUQ_H
#define DUQ_DUQ_H

#include "base/ptable.h"
#include "modules/module.h"
#include "classes/configuration.h"
#include "classes/braggpeak.h"
#include "classes/kvector.h"
#include "classes/pairpotential.h"
#include "classes/potentialmap.h"
#include "classes/sample.h"
#include "base/processpool.h"
#include "templates/vector3.h"
#include "templates/list.h"
#include "templates/orderedlist.h"
#include "templates/orderedpointerlist.h"

// Forward Declarations
class Atom;
class Box;
class Cell;
class Grain;
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


	/*
	 * Data
	 */
	public:
	// Clear all data
	void clear();


	/*
	 * Atom Types
	 */
	private:
	// List of allowed AtomTypes for all Species
	List<AtomType> atomTypes_;
	
	public:
	// Add AtomType
	AtomType* addAtomType(int el);
	// Remove AtomType
	void removeAtomType( AtomType* at);
	// Return number of AtomTypes in list
	int nAtomTypes() const;
	// Return first AtomType in list
	AtomType* atomTypes() const;
	// Return nth AtomType in list
	AtomType* atomType(int n);
	// Return first AtomType for element specified
	AtomType* atomTypeForElement(int el) const;
	// Update AtomTypes definitions
	void updateAtomTypes();
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
	 * Sample Definitions
	 */
	private:
	// List of Samples
	List<Sample> samples_;

	public:
	// Update current Samples
	void updateSamples();
	// Add Sample
	Sample* addSample(const char* baseName = "NewSample");
	// Remove Sample
	void removeSample(Sample* sample);
	// Return first Sample
	Sample* samples() const;
	// Return nth Sample
	Sample* sample(int n);
	// Search for Sample by name
	Sample* findSample(const char* name) const;


	/*
	 * Pair Potentials
	 */
	private:
	// All distinct AtomTypes (non-isotopic) in all Configurations for which we need PairPotentials
	AtomTypeIndex pairPotentialAtomTypeIndex_;
	// Maximum distance for tabulated PairPotentials
	double pairPotentialRange_;
	// Maximum squared distance for tabulated PairPotentials
	double pairPotentialRangeSquared_;
	// Width of PairPotential tail over which to truncate
	double pairPotentialTruncationWidth_;
	// Delta to use in tabulation
	double pairPotentialDelta_;
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
	// Add missing pair PairPotentials to lists
	bool addMissingPairPotentials();
	// Remove specified PairPotential from the list
	void removePairPotential(PairPotential* pot);
	// Return index of specified PairPotential
	int indexOf(PairPotential* pp);
	// Return number of defined PairPotentials
	int nPairPotentials() const;
	// Add new pair potential to list
	PairPotential* addPairPotential();
	// Return first PairPotential in list
	PairPotential* pairPotentials() const;
	// Return nth PairPotential in list
	PairPotential* pairPotential(int n);
	// Return whether specified PairPotential is defined
	PairPotential* hasPairPotential(AtomType* at1, AtomType* at2) const;
	// Regenerate all currently-defined PairPotentials
	void regeneratePairPotentials();
	// Regenerate specific PairPotential
	void regeneratePairPotential(PairPotential* pp);
	// Save all PairPotentials
	bool savePairPotentials(const char* baseName) const;
	// Return map for PairPotentials
	const PotentialMap& potentialMap();


	/*
	 * Modules
	 */
	private:
	// Reference list of all unique modules
	RefList<Module,bool> allModules_;
	// RefLists of unique modules, sorted by type
	RefList<Module,bool> modules_[Module::nModuleTypes];

	public:
	// Register all Modules
	bool registerModules();
	// Find Module by name
	Module* findModule(const char* name);


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
	Configuration* findConfiguration(const char* name) const;


	/*
	 * Simulation
	 */
	private:
	// Number of test points to use when calculating Box normalisation arrays
	int boxNormalisationPoints_;
	// Bin width to use when calculating RMSE between Sample F(Q) and reference data
	double rmseDeltaQ_;
	// Random seed
	int seed_;
	// Window function to use for all Fourier transforms
	Data2D::WindowFunction windowFunction_;

	public:
	// Set number of test points to use when calculating Box normalisation arrays
	void setBoxNormalisationPoints(int nPoints);
	// Bin width to use when calculating RMSE between Sample F(Q) and reference data
	void setRMSEDeltaQ(double dq);
	// Set random seed
	void setSeed(int seed);
	// Return random seed
	int seed();
	// Set window function to use for all Fourier transforms
	void setWindowFunction(Data2D::WindowFunction wf);
	// Setup all simulation data, checking it as we go
	bool setupSimulation();
	// Run main simulation
	bool go();


	/*
	 * Setup Dump
	 */
	public:
	// Dump full system setup
	void dumpSystemSetup(bool includeData);


	/*
	 * Energy Methods
	 */
	public:
	// Return total intramolecular energy
	double intramolecularEnergy(ProcessPool& procPool, Configuration* cfg);
	// Return total atom energy
	double interatomicEnergy(ProcessPool& procPool, Configuration* cfg);
	// Return total grain energy
	double intergrainEnergy(ProcessPool& procPool, Configuration* cfg);
	// Return total energy of the system
	double totalEnergy(ProcessPool& procPool, Configuration* cfg);
	// Test - Return total intramolecular (+correction) energy
	double intramolecularEnergyTest(Configuration* cfg);
	// Test - Return total energy of the system
	double totalEnergyTest(Configuration* cfg);
	// Test - Return total energy of the system using Molecules
	double totalEnergyTestMolecules(Configuration* cfg);


	/*
	 * Force Methods
	 */
	public:
	// Calculate total intramolecular forces
	void intramolecularForces(ProcessPool& procPool, Configuration* cfg, double* fx, double* fy, double* fz, ProcessPool::CommGroup group = ProcessPool::Pool);
	// Calculate total Grain forces
	void grainForces(ProcessPool& procPool, Configuration* cfg, double* fx, double* fy, double* fz, double cutoffSq, ProcessPool::CommGroup group);
	// Calculate total forces within the system
	void totalForces(ProcessPool& procPool, Configuration* cfg, double* fx, double* fy, double* fz, double cutoffSq, ProcessPool::CommGroup group = ProcessPool::Pool);


	/*
	 * Simulation Methods
	 */
	public:
	// Perform an Atom shake
// 	bool atomShake(Configuration& cfg, double cutoffDistance = -1.0, int nShakesPerAtom = 1, double targetAcceptanceRate = 0.33, double translationStepSize = 0.1);
	// Perform a Grain shake
// 	bool grainShake(Configuration& cfg, double cutoffDistance = -1.0, int nShakesPerGrain = 1, double targetAcceptanceRate = 0.33, double translationStepSize = 0.1, double rotationStepSize = 10.0);
// 	// Shake intramolecular terms within Grains
// 	bool intraShake(Configuration& cfg, int nShakesPerTerm = 1);
// 	// Shake intermolecular terms between Grains
// 	bool interShake(Configuration& cfg);
// 	// Individually Shake all Intramolecular Terms
// 	bool termShake(Configuration& cfg, int nShakesPerTerm = 1);
// 	// Twist molecules around bonds
// 	bool twist(Configuration& cfg, double cutoffDistance = -1.0, int nTwistsPerTerm = 1);
// 	// Perform some MD
// 	bool md(Configuration& cfg, double cutoffDistance = -1.0, int nSteps = 1, double deltaT = 0.0001);
// 	// Perform a world atom shake
// 	bool worldAtomShake(Configuration& cfg, double cutoffDistance = -1.0, int nShakes = 1, double targetAcceptanceRate = 0.33, double translationStepSize = 0.1);


	/*
	 * Input File I/O
	 */
	private:
	// Filename of current input file
	Dnchar fileName_;

	public:
	// Load datafiles
	bool loadDataFiles();
	// Load Species from specified file
	bool loadSpecies(const char* fileName);
	// Load input file
	bool loadInput(const char* fileName);
	// Save input file
	bool saveInput(const char* fileName);
	// Return whether a fileName has been set
	bool hasFileName() const;
	// Return filename of current input file
	const char* fileName() const;


	/*
	 * Configuration File I/O
	 */
	public:
	// Save Configuration as XYZ
	bool saveConfigurationXYZ(Configuration& cfg, const char* fileName);
	// Save Configuration as DL_POLY CONFIG
	bool saveConfigurationDLPOLY(Configuration& cfg, const char* fileName);


	/*
	 * Parallel Comms
	 */
	private:
	// Pool containing all available processes
	ProcessPool worldPool_;

	public:
	// Setup world parallelism
	void setupWorldPool();
	// Return world process pool
	ProcessPool& worldPool();
	// Setup local MPI pools
	bool setupMPIPools();
	// Broadcast complete system setup data
	bool broadcastSetup();
};

#endif
