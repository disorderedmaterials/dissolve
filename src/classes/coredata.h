// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/version.h"
#include "classes/configuration.h"
#include "classes/masterintra.h"
#include "classes/species.h"
#include "data/elements.h"
#include "templates/optionalref.h"
#include <list>
#include <memory>
#include <optional>
#include <vector>

// Forward Declarations
class Module;

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
    // Generate unique AtomType name with base name provided
    std::string uniqueAtomTypeName(std::string_view base) const;
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
    // List of master Bond parameters for Species
    std::vector<std::shared_ptr<MasterIntra>> masterBonds_;
    // List of master Angles parameters for Species
    std::vector<std::shared_ptr<MasterIntra>> masterAngles_;
    // List of master Torsions parameters for Species
    std::vector<std::shared_ptr<MasterIntra>> masterTorsions_;
    // List of master Improper parameters for Species
    std::vector<std::shared_ptr<MasterIntra>> masterImpropers_;

    public:
    // Add new master Bond parameters
    MasterIntra &addMasterBond(std::string_view name);
    // Return number of master Bond parameters in list
    int nMasterBonds() const;
    // Return list of master Bond parameters
    std::vector<std::shared_ptr<MasterIntra>> &masterBonds();
    const std::vector<std::shared_ptr<MasterIntra>> &masterBonds() const;
    // Return whether named master Bond parameters exist
    OptionalReferenceWrapper<MasterIntra> getMasterBond(std::string_view name);
    OptionalReferenceWrapper<const MasterIntra> getMasterBond(std::string_view name) const;
    // Add new master Angle parameters
    MasterIntra &addMasterAngle(std::string_view name);
    // Return number of master Angles parameters in list
    int nMasterAngles() const;
    // Return list of master Angle parameters
    std::vector<std::shared_ptr<MasterIntra>> &masterAngles();
    const std::vector<std::shared_ptr<MasterIntra>> &masterAngles() const;
    // Return whether named master Angle parameters exist
    OptionalReferenceWrapper<MasterIntra> getMasterAngle(std::string_view name);
    OptionalReferenceWrapper<const MasterIntra> getMasterAngle(std::string_view name) const;
    // Add new master Torsion parameters
    MasterIntra &addMasterTorsion(std::string_view name);
    // Return number of master Torsions parameters in list
    int nMasterTorsions() const;
    // Return list of master Torsion parameters
    std::vector<std::shared_ptr<MasterIntra>> &masterTorsions();
    const std::vector<std::shared_ptr<MasterIntra>> &masterTorsions() const;
    // Return whether named master Torsion parameters exist
    OptionalReferenceWrapper<MasterIntra> getMasterTorsion(std::string_view name);
    OptionalReferenceWrapper<const MasterIntra> getMasterTorsion(std::string_view name) const;
    // Add new master Improper parameters
    MasterIntra &addMasterImproper(std::string_view name);
    // Return number of master Impropers parameters in list
    int nMasterImpropers() const;
    // Return list of master Improper parameters
    std::vector<std::shared_ptr<MasterIntra>> &masterImpropers();
    const std::vector<std::shared_ptr<MasterIntra>> &masterImpropers() const;
    // Return whether named master Improper parameters exist
    OptionalReferenceWrapper<MasterIntra> getMasterImproper(std::string_view name);
    OptionalReferenceWrapper<const MasterIntra> getMasterImproper(std::string_view name) const;
    // Return the named master term (of any form) if it exists
    OptionalReferenceWrapper<const MasterIntra> findMasterTerm(std::string_view name) const;
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
    // Generate unique Species name with base name provided
    std::string uniqueSpeciesName(std::string_view baseName) const;
    // Search for Species by name
    Species *findSpecies(std::string_view name) const;

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
    // Generate unique Configuration name with base name provided
    std::string uniqueConfigurationName(std::string_view base) const;
    // Search for Configuration by name
    Configuration *findConfiguration(std::string_view name) const;

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
};
