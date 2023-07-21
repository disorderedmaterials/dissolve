// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/serialiser.h"
#include "classes/configuration.h"
#include "classes/coreData.h"
#include "classes/pairPotential.h"
#include "classes/potentialMap.h"
#include "classes/serializablePairPotential.h"
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
class Dissolve : public Serialisable<>
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
    SerializablePairPotential serializablePairPotential_;

    public:
    // Return reference to CoreData
    CoreData &coreData();
    const CoreData &coreData() const;
    // Clear all data
    void clear();
    static constexpr bool toml_testing_flag = false;

    /*
     * Pair Potentials
     */
    private:
    // Maximum distance for tabulated PairPotentials
    double pairPotentialRange_;
    // Delta to use in tabulation
    double pairPotentialDelta_;
    // Whether to automatically determine charge source
    bool automaticChargeSource_;
    // Whether to force the use of the specified charge source (if not automatic choice)
    bool forceChargeSource_;
    // Whether charges from atom types are to be used (and included in PairPotentials)
    bool atomTypeChargeSource_;
    // Simulation PairPotentials
    std::vector<std::unique_ptr<PairPotential>> pairPotentials_;
    // Map for PairPotentials
    PotentialMap potentialMap_;

    public:
    // Set maximum distance for tabulated PairPotentials
    void setPairPotentialRange(double range);
    // Return maximum distance for tabulated PairPotentials
    double pairPotentialRange() const;
    // Set delta to use in tabulations
    void setPairPotentialDelta(double delta);
    // Return delta to use in tabulations
    double pairPotentialDelta() const;
    // Set whether to automatically determine charge source
    void setAutomaticChargeSource(bool b);
    // Return whether to automatically determine charge source
    bool automaticChargeSource() const;
    // Set whether to force the use of the specified charge source (if not automatic choice)
    void setForceChargeSource(bool b);
    // Return whether to force the use of the specified charge source (if not automatic choice)
    bool forceChargeSource() const;
    // Set whether charges from atom types are to be used (and included in PairPotentials)
    void setAtomTypeChargeSource(bool b);
    // Return whether charges from atom types are to be used (and included in PairPotentials)
    bool atomTypeChargeSource() const;
    // Return index of specified PairPotential
    int indexOf(PairPotential *pp);
    // Return number of defined PairPotentials
    int nPairPotentials() const;
    // Add new pair potential to list
    PairPotential *addPairPotential(std::shared_ptr<AtomType> at1, std::shared_ptr<AtomType> at2);
    // Return PairPotentials list
    const std::vector<std::unique_ptr<PairPotential>> &pairPotentials() const;
    // Return nth PairPotential in list
    PairPotential *pairPotential(int n);
    // Return specified PairPotential (if defined)
    PairPotential *pairPotential(const std::shared_ptr<AtomType> &at1, const std::shared_ptr<AtomType> &at2) const;
    PairPotential *pairPotential(std::string_view at1, std::string_view at2) const;
    // Return map for PairPotentials
    const PotentialMap &potentialMap() const;
    // Clear and regenerate all PairPotentials, replacing those currently defined
    bool regeneratePairPotentials();
    // Revert potentials to reference state, clearing additional potentials
    void revertPairPotentials();

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
    // Return Configuration vector
    std::vector<std::unique_ptr<Configuration>> &configurations();
    const std::vector<std::unique_ptr<Configuration>> &configurations() const;
    // Return raw Configuration vector
    std::vector<Configuration *> rawConfigurations() const;
    // Find configuration by name
    Configuration *findConfiguration(std::string_view name) const;
    // Find configuration by 'nice' name
    Configuration *findConfigurationByNiceName(std::string_view name) const;

    /*
     * Layers
     */
    private:
    // List of defined processing layers
    std::vector<std::unique_ptr<ModuleLayer>> processingLayers_;
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
    // Return list of processing layers
    std::vector<std::unique_ptr<ModuleLayer>> &processingLayers();
    // Run the set-up stages of all modules in all layers
    bool setUpProcessingLayerModules();
    // Return data associated with main processing Modules
    GenericList &processingModuleData();

    /*
     * Simulation
     */
    private:
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
    // Estimate time in seconds required to perform next n steps (if possible to determine)
    std::optional<double> estimateRequiredTime(int nIterations);
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

    private:
    // Load input file through supplied parser
    bool loadInput(LineParser &parser);

    public:
    // Load input file
    bool loadInput(std::string_view filename);
    // Read values from a serialisable value
    void deserialise(const SerialisedValue &node) override;
    // Load input from supplied string
    bool loadInputFromString(std::string_view inputString);
    // Save input file
    bool saveInput(std::string_view filename);
    // Express as a serialisable value
    SerialisedValue serialise() const override;
    // Load restart file
    bool loadRestart(std::string_view filename);
    // Load restart file as reference point
    bool loadRestartAsReference(std::string_view filename, std::string_view dataSuffix);
    // Save restart file
    bool saveRestart(std::string_view filename);
    // Return whether an input filename has been set
    bool hasInputFilename() const;
    // Set current input filename
    void setInputFilename(std::string_view filename);
    // Return current input filename
    std::string_view inputFilename() const;
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
    private:
    // World process pool
    ProcessPool worldPool_;

    public:
    // Set up the world pool
    void setUpWorldPool();
    // Return the world process pool
    const ProcessPool &worldPool() const;
};
