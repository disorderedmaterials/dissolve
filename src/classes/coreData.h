// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#pragma once

#include "base/version.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "data/elements.h"
#include "task/task.h"
#include "templates/optionalRef.h"
#include <list>
#include <memory>
#include <optional>
#include <vector>

// Forward Declarations
class Dissolve;
class Module;
class ModuleLayer;
class MasterBond;
class MasterAngle;
class MasterTorsion;
class MasterImproper;

// Core Data Container
class CoreData
{
    public:
    CoreData() = default;
    ~CoreData() = default;
    // Clear all data
    void clear();

    /*
     * Atom Types
     */
    private:
    // Core AtomTypes list
    std::vector<std::shared_ptr<AtomType>> atomTypes_;
    // AtomTypes version
    VersionCounter atomTypesVersion_;

    public:
    // Add new AtomType
    std::shared_ptr<AtomType> addAtomType(Elements::Element Z);
    // Remove specified AtomType
    void removeAtomType(const std::shared_ptr<AtomType> &at);
    // Return number of AtomTypes in list
    int nAtomTypes() const;
    // Return core AtomTypes list
    std::vector<std::shared_ptr<AtomType>> &atomTypes();
    const std::vector<std::shared_ptr<AtomType>> &atomTypes() const;
    // Return nth AtomType in list
    std::shared_ptr<AtomType> atomType(int n);
    // Search for AtomType by name
    std::shared_ptr<AtomType> findAtomType(std::string_view name) const;
    // Bump AtomTypes version
    void bumpAtomTypesVersion();
    // Return AtomTypes version
    int atomTypesVersion() const;
    // Remove any atom types that are unused across all species
    int removeUnusedAtomTypes();
    // Clear all atom types
    void clearAtomTypes();

    /*
     * Master Intramolecular Terms
     */
    private:
    class Masters : public Serialisable<>
    {
        public:
        // Master Bond parameters for Species
        std::vector<std::shared_ptr<MasterBond>> bonds;
        // Master Angles parameters for Species
        std::vector<std::shared_ptr<MasterAngle>> angles;
        // Master Torsions parameters for Species
        std::vector<std::shared_ptr<MasterTorsion>> torsions;
        // Master Improper parameters for Species
        std::vector<std::shared_ptr<MasterImproper>> impropers;

        // Express as a serialisable value
        SerialisedValue serialise() const override;
        // Read values from a serialisable value
        void deserialise(const SerialisedValue &node) override;
    };
    // Master terms
    Masters masters_;

    public:
    // Express Master terms as serialisable value
    SerialisedValue serialiseMaster() const;
    // Read Master values from serialisable value
    void deserialiseMaster(const SerialisedValue &node);
    // Add new master Bond parameters
    MasterBond &addMasterBond(std::string_view name);
    // Return number of master Bond parameters in list
    int nMasterBonds() const;
    // Return list of master Bond parameters
    std::vector<std::shared_ptr<MasterBond>> &masterBonds();
    const std::vector<std::shared_ptr<MasterBond>> &masterBonds() const;
    // Return whether named master Bond parameters exist
    OptionalReferenceWrapper<MasterBond> getMasterBond(std::string_view name);
    OptionalReferenceWrapper<const MasterBond> getMasterBond(std::string_view name) const;
    // Add new master Angle parameters
    MasterAngle &addMasterAngle(std::string_view name);
    // Return number of master Angles parameters in list
    int nMasterAngles() const;
    // Return list of master Angle parameters
    std::vector<std::shared_ptr<MasterAngle>> &masterAngles();
    const std::vector<std::shared_ptr<MasterAngle>> &masterAngles() const;
    // Return whether named master Angle parameters exist
    OptionalReferenceWrapper<MasterAngle> getMasterAngle(std::string_view name);
    OptionalReferenceWrapper<const MasterAngle> getMasterAngle(std::string_view name) const;
    // Add new master Torsion parameters
    MasterTorsion &addMasterTorsion(std::string_view name);
    // Return number of master Torsions parameters in list
    int nMasterTorsions() const;
    // Return list of master Torsion parameters
    std::vector<std::shared_ptr<MasterTorsion>> &masterTorsions();
    const std::vector<std::shared_ptr<MasterTorsion>> &masterTorsions() const;
    // Return whether named master Torsion parameters exist
    OptionalReferenceWrapper<MasterTorsion> getMasterTorsion(std::string_view name);
    OptionalReferenceWrapper<const MasterTorsion> getMasterTorsion(std::string_view name) const;
    // Add new master Improper parameters
    MasterImproper &addMasterImproper(std::string_view name);
    // Return number of master Impropers parameters in list
    int nMasterImpropers() const;
    // Return list of master Improper parameters
    std::vector<std::shared_ptr<MasterImproper>> &masterImpropers();
    const std::vector<std::shared_ptr<MasterImproper>> &masterImpropers() const;
    // Return whether named master Improper parameters exist
    OptionalReferenceWrapper<MasterImproper> getMasterImproper(std::string_view name);
    OptionalReferenceWrapper<const MasterImproper> getMasterImproper(std::string_view name) const;
    // Clear all master terms
    void clearMasterTerms();

    /*
     * Species
     */
    private:
    // Core Species list
    std::vector<std::unique_ptr<Species>> species_;

    public:
    // Add new Species
    Species *addSpecies();
    // Remove specified Species
    void removeSpecies(Species *sp);
    // Return number of Species in list
    int nSpecies() const;
    // Return core Species list
    std::vector<std::unique_ptr<Species>> &species();
    const std::vector<std::unique_ptr<Species>> &species() const;
    // Search for Species by name
    Species *findSpecies(std::string_view name) const;
    // Copy AtomType between SpeciesAtoms, creating a new one if necessary
    void copyAtomType(const SpeciesAtom &sourceAtom, SpeciesAtom &destAtom);
    // Copy intramolecular interaction parameters, adding master term if necessary
    void copySpeciesBond(const SpeciesBond &source, SpeciesBond &dest);
    void copySpeciesAngle(const SpeciesAngle &source, SpeciesAngle &dest);
    void copySpeciesTorsion(const SpeciesTorsion &source, SpeciesTorsion &dest);
    void copySpeciesImproper(const SpeciesImproper &source, SpeciesImproper &dest);
    // Copy Species
    Species *copySpecies(const Species *species);

    /*
     * Configurations
     */
    private:
    // Core Configurations list
    std::vector<std::unique_ptr<Configuration>> configurations_;

    public:
    // Add new Configuration
    Configuration *addConfiguration();
    // Remove specified Configuration
    void removeConfiguration(Configuration *cfg);
    // Return number of Configuration in list
    int nConfigurations() const;
    // Return core Configuration list
    std::vector<std::unique_ptr<Configuration>> &configurations();
    const std::vector<std::unique_ptr<Configuration>> &configurations() const;
    // Return nth Configuration in list
    Configuration *configuration(int n);
    // Search for Configuration by name
    Configuration *findConfiguration(std::string_view name) const;
    // Find configuration by 'nice' name
    Configuration *findConfigurationByNiceName(std::string_view name) const;

    /*
     * Layers
     */
    private:
    // List of defined processing layers
    std::vector<std::unique_ptr<ModuleLayer>> processingLayers_;

    public:
    // Add new processing layer
    ModuleLayer *addProcessingLayer();
    // Remove processing layer
    void removeProcessingLayer(ModuleLayer *layer);
    // Find named processing layer
    ModuleLayer *findProcessingLayer(std::string_view name) const;
    // Return number of processing layers
    int nProcessingLayers() const;
    // Return current processing layers
    std::vector<std::unique_ptr<ModuleLayer>> &processingLayers();
    const std::vector<std::unique_ptr<ModuleLayer>> &processingLayers() const;
    // Run the set-up stages of all modules in all layers
    bool setUpProcessingLayerModules(Dissolve &dissolve);

    /*
     * Tasks
     */
    private:
    // Master task
    Task masterTask_;

    public:
    // Return master task
    const Task &masterTask() const;
    Task &masterTask();

    /*
     * Input Filename
     */
    private:
    // Current input filename (mirroring that in Dissolve)
    std::string inputFilename_;

    public:
    // Set current input filename
    void setInputFilename(std::string_view filename);
    // Return the current input filename
    std::string_view inputFilename() const;

    /*
     * Object Management
     */
    public:
    // Remove all references to the specified data
    void removeReferencesTo(Module *data);
    void removeReferencesTo(Configuration *data);
    void removeReferencesTo(Species *data);
    void removeReferencesTo(SpeciesSite *data);
};
