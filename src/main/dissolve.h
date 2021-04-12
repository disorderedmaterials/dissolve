// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/pairpotential.h"
#include "classes/potentialmap.h"
#include "data/elements.h"
#include "module/layer.h"
#include "module/module.h"

// Forward Declarations
class Atom;
class Box;
class Cell;
class Isotopologue;
class Molecule;

// Dissolve Main Class
class Dissolve
{
    public:
    Dissolve(CoreData &coreData);
    ~Dissolve();

    /*
     * Core
     */
    private:
    // Reference to CoreData
    CoreData &coreData_;

    public:
    // Return reference to CoreData
    CoreData &coreData();
    const CoreData &coreData() const;
    // Clear all data
    void clear();

    /*
     * Atom Types
     * (Exposes std::vector<std::shared_ptr<AtomType>> in coreData_)
     */
    public:
    // Add AtomType with specified Element
    std::shared_ptr<AtomType> addAtomType(Elements::Element Z);
    // Return number of AtomTypes in list
    int nAtomTypes() const;
    // Return AtomTypes list
    std::vector<std::shared_ptr<AtomType>> &atomTypes();
    // Return nth AtomType in list
    std::shared_ptr<AtomType> atomType(int n);
    // Search for AtomType by name
    std::shared_ptr<AtomType> findAtomType(std::string_view name) const;
    // Clear all AtomTypes
    void clearAtomTypes();

    /*
     * Master Terms
     * (Exposes lists in coreData_)
     */
    public:
    // Return list of master Bond parameters
    const std::list<MasterIntra> &masterBonds() const;
    // Return list of master Angle parameters
    const std::list<MasterIntra> &masterAngles() const;
    // Return list of master Torsion parameters
    const std::list<MasterIntra> &masterTorsions() const;
    // Return list of master Improper parameters
    const std::list<MasterIntra> &masterImpropers() const;
    // Check and print MasterTerm usage
    void checkMasterTermUsage() const;

    /*
     * Species Definitions
     * (Exposes List<Species> in coreData_)
     */
    public:
    // Add a new Species to the list
    Species *addSpecies();
    // Remove the specified Species from the list
    void removeSpecies(Species *sp);
    // Return number of defined Species
    int nSpecies() const;
    // Return Species list
    std::vector<std::unique_ptr<Species>> &species();
    // Search for Species by name
    Species *findSpecies(std::string_view name) const;
    // Copy AtomType, creating a new one if necessary
    void copyAtomType(const SpeciesAtom *sourceAtom, SpeciesAtom *destAtom);
    // Copy intramolecular interaction parameters, adding MasterIntra if necessary
    void copySpeciesIntra(const SpeciesIntra &sourceIntra, SpeciesIntra &destIntra);
    // Copy Species
    Species *copySpecies(const Species *species);

    /*
     * Pair Potentials
     */
    private:
    // Maximum distance for tabulated PairPotentials
    double pairPotentialRange_;
    // Maximum squared distance for tabulated PairPotentials
    double pairPotentialRangeSquared_;
    // Delta to use in tabulation
    double pairPotentialDelta_;
    // Whether Coulomb terms are included in generated PairPotentials
    bool pairPotentialsIncludeCoulomb_;
    // Simulation PairPotentials
    List<PairPotential> pairPotentials_;
    // Version of AtomTypes at which PairPotential were last generated
    int pairPotentialAtomTypeVersion_;
    // Map for PairPotentials
    PotentialMap potentialMap_;

    public:
    // Set maximum distance for tabulated PairPotentials
    void setPairPotentialRange(double range);
    // Return maximum distance for tabulated PairPotentials
    double pairPotentialRange() const;
    // Return maximum squared distance for tabulated PairPotentials
    double pairPotentialRangeSquared() const;
    // Set delta to use in tabulations
    void setPairPotentialDelta(double delta);
    // Return delta to use in tabulations
    double pairPotentialDelta() const;
    // Set whether Coulomb term should be included in generated PairPotentials
    void setPairPotentialsIncludeCoulomb(bool b);
    // Return whether Coulomb term should be included in generated PairPotentials
    bool pairPotentialsIncludeCoulomb();
    // Return index of specified PairPotential
    int indexOf(PairPotential *pp);
    // Return number of defined PairPotentials
    int nPairPotentials() const;
    // Add new pair potential to list
    PairPotential *addPairPotential(std::shared_ptr<AtomType> at1, std::shared_ptr<AtomType> at2);
    // Return PairPotentials list
    const List<PairPotential> &pairPotentials() const;
    // Return nth PairPotential in list
    PairPotential *pairPotential(int n);
    // Return whether specified PairPotential is defined
    PairPotential *pairPotential(std::shared_ptr<AtomType> at1, std::shared_ptr<AtomType> at2) const;
    // Return whether specified PairPotential is defined
    PairPotential *pairPotential(std::string_view at1, std::string_view at2) const;
    // Return map for PairPotentials
    const PotentialMap &potentialMap();
    // Clear and regenerate all PairPotentials, replacing those currently defined
    void regeneratePairPotentials();
    // Generate all necessary PairPotentials, adding missing terms where necessary
    bool generatePairPotentials(std::shared_ptr<AtomType> onlyInvolving = nullptr);

    /*
     * Configurations
     * (Exposes List<Configuration> in coreData_)
     */
    public:
    // Add new Configuration
    Configuration *addConfiguration();
    // Own the specified Configuration
    bool ownConfiguration(Configuration *cfg);
    // Remove specified Configuration
    void removeConfiguration(Configuration *cfg);
    // Return number of defined Configurations
    int nConfigurations() const;
    // Return Configuration list
    List<Configuration> &configurations();
    const List<Configuration> &configurations() const;
    // Find configuration by name
    Configuration *findConfiguration(std::string_view name) const;
    // Find configuration by 'nice' name
    Configuration *findConfigurationByNiceName(std::string_view name) const;

    /*
     * Modules
     */
    private:
    // List of all instances of all used Modules
    RefList<Module> moduleInstances_;
    // List of master Module instances
    List<Module> masterModules_;

    private:
    // Register master Module
    bool registerMasterModule(Module *masterInstance);

    public:
    // Register master instances for all Modules
    bool registerMasterModules();
    // Return master Module instances
    const List<Module> &masterModules() const;
    // Search for master Module of the named type
    Module *findMasterModule(std::string_view moduleType) const;
    // Create a Module instance for the named Module type
    Module *createModuleInstance(std::string_view moduleType);
    // Create a Module instance for the named Module type, and add it to the specified layer
    Module *createModuleInstance(std::string_view moduleType, ModuleLayer *destinationLayer, bool configurationLocal = false);
    // Search for any instance of any Module with the specified unique name
    Module *findModuleInstance(std::string_view uniqueName);
    // Search for any instance of any Module with the specified Module type
    std::vector<Module *> findModuleInstances(std::string_view moduleType);
    // Generate unique Module name with base name provided
    std::string uniqueModuleName(std::string_view name, Module *excludeThis = nullptr);
    // Delete specified Module instance
    bool deleteModuleInstance(Module *instance);

    /*
     * Layers
     */
    private:
    // List of defined processing layers
    List<ModuleLayer> processingLayers_;
    // Data associated with processing Modules
    GenericList processingModuleData_;

    public:
    // Add new processing layer
    ModuleLayer *addProcessingLayer();
    // Remove specified processing layer
    void removeProcessingLayer(ModuleLayer *layer);
    // Find named processing layer
    ModuleLayer *findProcessingLayer(std::string_view name) const;
    // Own the specified processing layer
    bool ownProcessingLayer(ModuleLayer *layer);
    // Return number of defined processing layers
    int nProcessingLayers() const;
    // Generate unique processing layer name, with base name provided
    std::string uniqueProcessingLayerName(std::string_view baseName) const;
    // Return list of processing layers
    List<ModuleLayer> &processingLayers();
    // Return data associated with main processing Modules
    GenericList &processingModuleData();
    // Create and add a named Module to the named layer (creating it if necessary), with optional Configuration target
    Module *createModuleInLayer(std::string_view moduleType, std::string_view layerName, Configuration *cfg = nullptr);

    /*
     * Simulation
     */
    private:
    // Random seed
    int seed_;
    // Frequency at which to write restart file
    int restartFileFrequency_;
    // Current simulation step
    int iteration_;
    // Number of iterations performed
    int nIterationsPerformed_;
    // Main loop timer
    Timer iterationTimer_;
    // Accumulated timing information for main loop iterations
    SampledDouble iterationTime_;

    public:
    // Set number of test points to use when calculating Box normalisation arrays
    void setNBoxNormalisationPoints(int nPoints);
    // Return number of test points to use when calculating Box normalisation arrays
    int nBoxNormalisationPoints() const;
    // Set random seed
    void setSeed(int seed);
    // Return random seed
    int seed() const;
    // Set frequency with which to write various iteration data
    void setRestartFileFrequency(int n);
    // Return frequency with which to write restart file
    int restartFileFrequency() const;
    // Prepare for main simulation
    bool prepare();
    // Iterate main simulation
    bool iterate(int nIterations = -1);
    // Reset current simulation step
    void resetIterationCounter();
    // Return current simulation step
    int iteration() const;
    // Return per-iteration time in seconds
    double iterationTime() const;
    // Print timing information
    void printTiming();

    /*
     * I/O
     */
    private:
    // Filename of current input file
    std::string inputFilename_;
    // Filename of current restart file
    std::string restartFilename_;
    // Accumulated timing information for saving restart file
    SampledDouble saveRestartTimes_;
    // Check if heartbeat file needs to be written or not
    bool writeHeartBeat_;

    private:
    // Load input file through supplied parser
    bool loadInput(LineParser &parser);

    public:
    // Load input file
    bool loadInput(std::string_view filename);
    // Load input from supplied string
    bool loadInputFromString(std::string_view inputString);
    // Save input file
    bool saveInput(std::string_view filename);
    // Load restart file
    bool loadRestart(std::string_view filename);
    // Load restart file as reference point
    bool loadRestartAsReference(std::string_view filename, std::string_view dataSuffix);
    // Save restart file
    bool saveRestart(std::string_view filename);
    // Save heartbeat file
    bool saveHeartBeat(std::string_view filename, double estimatedNSecs);
    // Set bool for heartbeat file to be written
    void setWriteHeartBeat(bool b);
    // write heartbeat file
    bool writeHeartBeat() const;
    // Return whether an input filename has been set
    bool hasInputFilename() const;
    // Set current input filenamea
    void setInputFilename(std::string_view filename);
    // Return current input filename
    std::string_view inputFilename() const;
    // Set restart filename
    void setRestartFilename(std::string_view filename);
    // Return restart filename
    std::string_view restartFilename() const;
    // Return whether a restart filename has been set
    bool hasRestartFilename() const;

    /*
     * Object Management
     */
    public:
    // Remove all references to the specified Configuration
    void removeReferencesTo(Configuration *cfg);
    // Remove all references to the specified Module
    void removeReferencesTo(Module *module);
    // Remove all references to the specified Species
    void removeReferencesTo(Species *sp);
    // Remove all references to the specified SpeciesSite
    void removeReferencesTo(SpeciesSite *site);

    /*
     * Parallel Comms
     */
    public:
    // Return world process pool
    ProcessPool &worldPool();
    // Set up local MPI pools
    bool setUpMPIPools();
};
