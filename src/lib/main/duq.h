/*
	*** dUQ Main Structure
	*** src/lib/main/duq.h
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

#ifndef DUQ_DUQ_H
#define DUQ_DUQ_H

#include "main/command.h"
#include "base/ptable.h"
#include "classes/configuration.h"
#include "classes/pairpotential.h"
#include "classes/potentialmap.h"
#include "classes/sample.h"
#include "base/comms.h"
#include "base/mutex.h"
#include "templates/vector3.h"
#include "templates/list.h"
#include "templates/checkpoint.h"

// Forward Declarations
class Atom;
class Box;
class Cell;
class Grain;
class Molecule;
class ChangeStore;

/*!
 * \short dUQ
 * \details The main dUQ class contains several sets of data, from which the system to refine is constructed.
 * 
 * <b>AtomTypes</b>
 * TODO
 * 
 * <b>Species</b>
 * A list of Species (which may be of any atomic/molecular/ionic type) is first defined, including basic coordinates, bond and
 * angle information, sets of Grain definitions (which split the constituent Atoms into collective fragments), and lists
 * of isotopologues (i.e. isotopic substitutions) for each.
 * 
 * <b>System Composition</b>
 * The System defines the actual composition of the experimental system, as well as its size when instantiated as a Model.
 * It is defined from a list of involved Species, and a relative population of each. An integer multiplier is used to transform
 * these values into the actual (whole) numbers of Molecules that will be used as the Model system.
 * 
 * <b>Samples</b>
 * 
 *
 * 
 * TODO The System usually contains external experimental data which are tied to a specific set of isotopologues of the component
 * Species.
 */
class DUQ
{
	public:
	// Constructor
	DUQ();
	// Destructor
	~DUQ();


	/*!
	 * \name Data
	 */
	public:
	// Clear all data
	void clear();


	/*!
	 * \name Atom Types
	 */
	///@{
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
	///@}


	/*!
	 * \name Species Definitions
	 */
	///@{
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
	// Generate unique Species name with base name provided
	const char* uniqueSpeciesName(const char* baseName, Species* exclude = NULL) const;
	// Search for Species by name
	Species* findSpecies(const char* name) const;
	// Update Species (or all) Isotopologues (or specified)
	void updateIsotopologues(Species* species = NULL, Isotopologue* iso = NULL);
	// Remove Isotopologue from Species
	void removeSpeciesIsotopologue(Species* species, Isotopologue* iso);
	///@}


	/*!
	 * \name System Composition
	 */
	///@{
	private:
	// Integer multiplier of relative populations
	int multiplier_;
	// Density of the system
	double density_;
	// Whether the density is in atomic units (atoms/A3) or chemistry units (g/cm3)
	bool densityIsAtomic_;
	// Relative Box lengths
	Vec3<double> relativeBoxLengths_;
	// Box angles
	Vec3<double> boxAngles_;
	// Flag to request a non-periodic box
	bool nonPeriodic_;
	// Flag specifying that a random config should be generated (as opposed to loading from a file)
	bool randomConfiguration_;
	// File containing initial coordinates
	Dnchar initialCoordinatesFile_;

	public:
	// Return total relative population of Species
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
	// Set relative box lengths
	void setRelativeBoxLengths(const Vec3<double> lengths);
	// Return relative box lengths
	Vec3<double> relativeBoxLengths() const;
	// Set box angles
	void setBoxAngles(const Vec3<double> angles);
	// Return box angles
	Vec3<double> boxAngles() const;
	///@}


	/*!
	 * \name Samples
	 */
	///@{
	private:
	// List of Samples
	List<Sample> samples_;

	public:
	// Update current Samples
	void updateSamples();
	// Add Sample
	Sample* addSample(const char *baseName = "NewSample");
	// Remove Sample
	void removeSample(Sample* sample);
	// Return first Sample
	Sample* samples() const;
	// Return nth Sample
	Sample* sample(int n);
	// Generate unique Sample name with base name provided
	const char* uniqueSampleName(const char* baseName, Sample* exclude = NULL) const;
	// Set up Sample data
	bool setupSamples();
	///@}


	/*!
	 * \name Pair Potentials
	 */
	///@{
	private:
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
	///@}


	/*!
	 * \name Master RDF / S(Q) Data
	 */
	///@{
	public:
	/// Partial RDF Calculation Style
	enum RDFMethod
	{
		NoMethod,		/**> 'None' - No calculation of partial RDFs */
		SimpleMethod,		/**> 'Simple' - Use a simple double-loop to calculate atomic partials */
		nRDFMethods		/**> Number of RDFMethods */
	};
	// Convert text string to RDFMethod
	static RDFMethod rdfMethod(const char* s);
	// Convert RDFMethod to text string
	static const char* rdfMethod(RDFMethod rm);

	private:
	// Partial RDF matrix
	Array2D<Histogram> partialRDFMatrix_;
	// Unbound RDF matrix
	Array2D<Histogram> unboundRDFMatrix_;
	// Bound RDF matrix
	Array2D<Histogram> boundRDFMatrix_;
	// RDF Normalisation for Box shape/extent
	Data2D boxNormalisation_;
	// Partial S(Q) matrix
	Array2D<Data2D> partialSQMatrix_;
	// Unbound S(Q) matrix
	Array2D<Data2D> unboundSQMatrix_;
	// Bound S(Q) matrix
	Array2D<Data2D> boundSQMatrix_;
	// Working S(Q) matrices
	Array2D<Data2D> workingSQMatrixA_, workingSQMatrixB_;
	// Configuration change count at which partials were last calculated
	int partialsChangeCount_;
	// Total RDF (unweighted)
	Data2D totalRDF_;
	// Total S(Q) (unweighted)
	Data2D totalFQ_;
	// Partial RDF calculation style
	RDFMethod rdfMethod_;
	// Total RMSE between calculated and reference F(Q) over all Samples
	double totalRMSE_;

	private:
	// Calculate partials using simple double Atom loop
	bool calculatePartialsSimple(Configuration& cfg);
	// Calculate partial RDFs (main routine)
	bool calculatePartialRDFs(Configuration& cfg);
	// Calculate intramolecular RDFs
	bool calculateIntramolecularRDFs(Configuration& cfg);

	public:
	// Setup RDF and S(Q) storage
	bool setupPartials();
	// Reset partials
	void resetPairCorrelations();
	// Calculate all partial RDFs and S(Q)
	CommandReturnValue calculatePairCorrelations(Configuration& cfg);
	// Save RDFs
	void saveRDFs(const char* baseName);
	// Save S(Q)
	void saveSQ(const char* baseName);
	///@}


	/*!
	 * \name Setup
	 */
	///@{
	private:
	// Flag count at last succesful setup
	int setupFlagCount_;
	// Molecule/Grain/Atom Configuration
	Configuration configuration_;
	// Master AtomType index, containing unique (non-isotopic) atom types
	AtomTypeIndex typeIndex_;
	// Map for PairPotentials
	PotentialMap potentialMap_;
	// Box normalisation array to load (otherwise calculate it)
	Dnchar boxNormalisationFileName_;
	// Number of test points to use when calculating Box normalisation array
	int boxNormalisationPoints_;
	// Density multiplier for cells when creating local atom arrays
	double cellDensityMultiplier_;
	// FWHM of Gaussian for Q-dependent instrument broadening function
	double qDependentFWHM_;
	// FWHM of Gaussian for Q-independent instrument broadening function
	double qIndependentFWHM_;
	// RDF bin width
	double rdfBinWidth_;
	// Working RDF extent
	double rdfRange_;
	// X value to extend RDF up to (filling with zeroes)
	double rdfExtensionLimit_;
	// Degree of smoothing to apply to calculated RDF data
	int rdfSmoothing_;
	// Requested RDF extent
	double requestedRDFRange_;
	// Bin width to use when calculating RMSE between Sample F(Q) and reference data
	double rmseDeltaQ_;
	// Random seed
	int seed_;
	// Simulation temperature (K)
	double temperature_;
	// Window function to use for Fourier transforms
	Data2D::WindowFunction windowFunction_;

	private:
	// Clear all model data
	void clearModel();
	// Load Box normalisation array
	bool loadBoxNormalisationFile(const char *fileName);
	// Setup pairpotentials
	bool setupPotentials();
	// Finalise and print setup variables
	bool printSetup();

	public:
	// Check current setup
	bool checkSetup();
	// Return whether current setup is valid
	bool setupIsValid();
	// Setup simulation
	bool setupSimulation();
	// Return random seed
	int seed();
	// Return configuration pointer
	Configuration* configuration();
	///@}


	/*!
	 * \name Energy Methods
	 */
	///@{
	private:
	// Return total intramolecular energy
	double intramolecularEnergy(Configuration& cfg);
	// Return total atom energy
	double interatomicEnergy(Configuration& cfg);
	// Return total grain energy
	double intergrainEnergy(Configuration& cfg);
	// Return total energy of the system
	double totalEnergy(Configuration& cfg);
	// Test - Return total intramolecular (+correction) energy
	double intramolecularEnergyTest(Configuration& cfg);
	// Test - Return total energy of the system
	double totalEnergyTest(Configuration& cfg);
	// Test - Return total energy of the system using Cells
	double totalEnergyTestCells(Configuration& cfg);
	// Test - Return total energy of the system using Molecules
	double totalEnergyTestMolecules(Configuration& cfg);
	///@}


	/*!
	 * \name Force Methods
	 */
	///@{
	private:
	// Calculate total intramolecular forces
	void intramolecularForces(Configuration& cfg, double* fx, double* fy, double* fz, DUQComm::CommGroup group = DUQComm::World);
	// Calculate total Grain forces
	void grainForces(Configuration& cfg, double* fx, double* fy, double* fz, double cutoffSq, DUQComm::CommGroup group);
	// Calculate total forces within the system
	void totalForces(Configuration& cfg, double* fx, double* fy, double* fz, double cutoffSq, DUQComm::CommGroup group = DUQComm::World);
	///@}


	/*!
	 * \name Command Definitions
	 */
	///@{
	private:
	// List of registered Commands
	List<Command> commands_;
	// Register a new Command in the library
	Command* registerCommand(const char* name, const char* shortNames, CommandPointer ptr);
	// Add an argument to a Command
	Argument* addCommandArgument(const char* commandName, const char* argumentName, const char* description, const char* defaultValue);
	// Add a parameter to a Command
	Argument* addCommandParameter(const char* commandName, const char* parameterName, const char* description, const char* startingValue);
	// Return first Command in list
	Command* commands();
	// Find and return named Command
	Command* command(const char* name);
	// Return specified Command argument
	Argument* commandArgument(const char* commandName, const char* argumentName);
	// Return Command argument specified as const char*
	const char* commandArgumentAsConstChar(const char* commandName, const char* argumentName, bool& result);
	// Return Command argument specified as integer
	int commandArgumentAsInteger(const char* commandName, const char* argumentName, bool& result);
	// Return Command argument specified as double
	double commandArgumentAsDouble(const char* commandName, const char* argumentName, bool& result);
	// Return parameter for specified Command
	Argument* commandParameter(const char* commandName, const char* parameterName);
	// Return Command parameter specified as const char*
	const char* commandParameterAsConstChar(const char* commandName, const char* parameterName, bool& result);
	// Return Command parameter specified as integer
	int commandParameterAsInteger(const char* commandName, const char* parameterName, bool& result);
	// Return Command parameter specified as double
	double commandParameterAsDouble(const char* commandName, const char* parameterName, bool& result);

	public:
	// Register basic/method commands
	bool registerCommands();


	/*!
	 * \name Basic Commands
	 */
	///@{
	public:
	// Mark END of loop in simulation steps
	CommandReturnValue commandEndLoop(Configuration& cfg);
	// Calculate and display total energy
	CommandReturnValue commandEnergy(Configuration& cfg);
	// Display help
	CommandReturnValue commandHelp(Configuration& cfg);
	// Load configuration from file
	CommandReturnValue commandLoadConfig(Configuration& cfg);
	// Mark start of loop in simulation steps
	CommandReturnValue commandLoop(Configuration& cfg);
	// Adjust system multiplier
	CommandReturnValue commandMultiplier(Configuration& cfg);
	// Quit program
	CommandReturnValue commandQuit(Configuration& cfg);
	// Calculate current RMSE
	CommandReturnValue commandRMSE(Configuration& cfg);
	// Save DLPOLY config of configuration
	CommandReturnValue commandSaveDLPOLY(Configuration& cfg);
	// Save RDF data
	CommandReturnValue commandSaveRDFs(Configuration& cfg);
	// Save current PairPotentials
	CommandReturnValue commandSavePairPotentials(Configuration& cfg);
	// Save current partial S(Q)
	CommandReturnValue commandSaveSQ(Configuration& cfg);
	// Save XYZ of configuration
	CommandReturnValue commandSaveXYZ(Configuration& cfg);
	// Set command parameter
	CommandReturnValue commandSet(Configuration& cfg);
	// Shake system, with optional loop
	CommandReturnValue commandShake(Configuration& cfg);
	// Set temperature
	CommandReturnValue commandTemperature(Configuration& cfg);
	// Test Routine
	CommandReturnValue commandTest(Configuration& cfg);
	///@}


	/*!
	 * \name Simulation Commands (Methods)
	 */
	///@{
	public:
	// Upkeep - Update Grains
	void updateGrains(Configuration& cfg);
	// Perform an Atom shake
	CommandReturnValue atomShake(Configuration& cfg);
	// Perform a Grain shake
	CommandReturnValue grainShake(Configuration& cfg);
	// Shake intramolecular terms within Grains
	CommandReturnValue intraShake(Configuration& cfg, int nShakesPerTerm);
	// Shake intermolecular terms between Grains
	CommandReturnValue interShake(Configuration& cfg);
	// Individually Shake all Intramolecular Terms
	CommandReturnValue termShake(Configuration& cfg, int nShakesPerTerm);
	// Twist molecules around bonds
	CommandReturnValue twist(Configuration& cfg);
	// Perform some MD
	CommandReturnValue md(Configuration& cfg);
	///@}


	/*!
	 * \name Simulation
	 */
	///@{
	private:
	// Equilibration steps to perform before running main simulation
	List<Step> equilibrationSteps_;
	// Shake steps to perform (when requested)
	List<Step> shakeSteps_;
	// Main simulation procedure
	List<Step> strategySteps_;
	// System energy data
	Data2D energyData_;
	// Energy change since last point added to energyData_
	double energyChange_;
	// Whether energy has changed since the last point was added
	bool energyChanged_;

	private:
	// Attempt to parse supplied line into a Step
	bool parseStep(Step* step, const char* line);
	// Execute Step list
	CommandReturnValue executeSteps(Step* firstStep);
	// Register a change in the total energy of the system
	void registerEnergyChange(double deltaE);
	// Accumulate current energy change into energyData_
	void accumulateEnergyChange();
	// Set absolute energy of system, after total energy calculation
	void setAbsoluteEnergy(double energy);

	public:
	// Execute provided simulation steps
	bool runSimulation();
	// Enter interactive mode
	bool goInteractive();
	// Execute single command
	bool runCommand(const char* commandString);
	///@}


	/*!
	 * \name Potential Perturbation Routines
	 */
	///@{
	private:
	// Maximum moves in Simplex minimisation (per cycle per Q bin)
	int simplexNMoves_;
	// Number of cycles in Simplex minimisation (per Q bin)
	int simplexNCycles_;
	// Temperature in Simplex minimisation (arbitrary units)
	double simplexTemperature_;
	// Convergence tolerance in Simplex minimisation
	double simplexTolerance_;
	// Parameter value offset to use in Simplex fitting procedure
	double simplexParameterOffset_;

	private:
	// Cost function callback (passed to Simplex on construction)
	double simplexCost(Array<double>& alpha);
	// Perturb potential
	CommandReturnValue perturb(Configuration& cfg);
	///@}


	/*!
	 * \name Input File I/O
	 */
	///@{
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
	///@}


	/*!
	 * \name Configuration File I/O
	 */
	///@{
	// Save Configuration as XYZ
	bool saveConfigurationXYZ(Configuration& cfg, const char* fileName);
	// Save Configuration as DL_POLY CONFIG
	bool saveConfigurationDLPOLY(Configuration& cfg, const char* fileName);
	///@}


	/*!
	 * \name CheckPoint / RealTime Access Data
	 */
	///@{
	public:
	/// CheckPoint Data2D types
	enum CheckPointData2D
	{
		CheckPointUnweightedSQ,		/**> Simulated, unweighted S(Q) data */
		CheckPointOriginalU,		/**> Original, unmodified PairPotentials */
		CheckPointU,			/**> Current PairPotentials, including modifications */
		CheckPointDU,			/**> Current PairPotential forces, including modifications */
		CheckPointV,			/**> Current modifications to PairPotentials */
		CheckPointFQ,			/**> Total F(Q) data, simulated and reference */
		CheckPointUnweightedGR,		/**> Partial g(r), unweighted, simulated */
		CheckPointTotalGR,		/**> Total g(r) data, neutron weighted, simulated */
		CheckPointEnergy,		/**> System energy data */
		nCheckPointData2DTypes
	};
	private:
	// General CheckPoint access mutex
	dUQMutex checkPointMutex_;
	// List of Data2D CheckPoint references
	List< CheckPoint<Data2D> > checkPointData2D_[nCheckPointData2DTypes];
	// Configuration CheckPoint data
	CheckPoint<Configuration> checkPointConfiguration_;
	
	public:
	// Setup CheckPoint data/lists
	void setupCheckPointData();
	// Update CheckPoint data for specified type
	void updateCheckPointData2D(DUQ::CheckPointData2D type);
	// Return number of CheckPoint<Data2D> items in specified list
	int nCheckPointData2D(DUQ::CheckPointData2D type);
	// Return pointer to nth CheckPoint<Data2D> of specified type
	CheckPoint<Data2D>* checkPointData2D(DUQ::CheckPointData2D type, int n);
	// Return pointer to Configuration CheckPoint data
	CheckPoint<Configuration>* checkPointConfiguration();
	///@}


	/*!
	 * \name Signalling
	 */
	///@{
	public:
	/// Signal List
	enum Signal
	{
		ConfigurationUpdatedSignal,	/**> Configuration data has been updated and stored in CheckPoint area */
		EnergyUpdatedSignal,		/**> Total energy data has been updated and stored in CheckPoing area */
		PairPotentialsUpdatedSignal,	/**> PairPotentials have been updated and stored in CheckPoint area */
		PairCorrelationsUpdatedSignal,	/**> Pair correlation data (g(r), S(Q) etc.) has been updated and stored in CheckPoint area */
		TerminateSignal,		/**> Simulation should be terminated as soon as possible */
		nSignals
	};

	private:
	// List of signals received
	List< Pair<DUQ::Signal,int> > receivedSignals_;
	// Received signals mutex
	dUQMutex receivedSignalsMutex_;
	
	private:
	// Act on signal provided
	CommandReturnValue processSignal(DUQ::Signal signal, int data);
	// Process any received signals
	CommandReturnValue processSignals();

	public:
	// Send signal
	virtual void sendSignal(DUQ::Signal signal, int data = 0);
	// Receive signal
	virtual void receiveSignal(DUQ::Signal signal, int data);
	///@}


	/*!
	 * \name Parallel Comms
	 */
	///@{
	public:
	// Messages
	enum Message { BroadcastSetupMessage, SetupSimulationMessage, nMessages };
	// Enter message loop (slaves only)
	void enterMessageLoop();
	// Pass message to slaves, forcing them to perform an action
	void sendMessage(DUQ::Message message);
	// Check on success of last message
	bool messageResult(bool rootResult);
	// Broadcast system setup data
	bool broadcastSetup();
	///@}
};

#endif
