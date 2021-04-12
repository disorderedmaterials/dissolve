// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/version.h"
#include "classes/masterintra.h"
#include "data/elements.h"
#include "templates/list.h"
#include "templates/optionalref.h"
#include "templates/reflist.h"
#include <list>
#include <memory>
#include <optional>
#include <vector>

// Forward Declarations
class AtomType;
class Configuration;
class Species;
class Module;

// Core Data Container
class CoreData
{
    public:
    CoreData();
    ~CoreData();
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
    void removeAtomType(std::shared_ptr<AtomType> at);
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

    /*
     * Master Intramolecular Terms
     */
    private:
    // List of master Bond parameters for Species
    std::list<MasterIntra> masterBonds_;
    // List of master Angles parameters for Species
    std::list<MasterIntra> masterAngles_;
    // List of master Torsions parameters for Species
    std::list<MasterIntra> masterTorsions_;
    // List of master Improper parameters for Species
    std::list<MasterIntra> masterImpropers_;

    public:
    // Add new master Bond parameters
    MasterIntra &addMasterBond(std::string_view name);
    // Return number of master Bond parameters in list
    int nMasterBonds() const;
    // Return list of master Bond parameters
    std::list<MasterIntra> &masterBonds();
    const std::list<MasterIntra> &masterBonds() const;
    // Return whether named master Bond parameters exist
    OptionalReferenceWrapper<MasterIntra> getMasterBond(std::string_view name);
    OptionalReferenceWrapper<const MasterIntra> getMasterBond(std::string_view name) const;
    // Add new master Angle parameters
    MasterIntra &addMasterAngle(std::string_view name);
    // Return number of master Angles parameters in list
    int nMasterAngles() const;
    // Return list of master Angle parameters
    std::list<MasterIntra> &masterAngles();
    const std::list<MasterIntra> &masterAngles() const;
    // Return whether named master Angle parameters exist
    OptionalReferenceWrapper<MasterIntra> getMasterAngle(std::string_view name);
    OptionalReferenceWrapper<const MasterIntra> getMasterAngle(std::string_view name) const;
    // Add new master Torsion parameters
    MasterIntra &addMasterTorsion(std::string_view name);
    // Return number of master Torsions parameters in list
    int nMasterTorsions() const;
    // Return list of master Torsion parameters
    std::list<MasterIntra> &masterTorsions();
    const std::list<MasterIntra> &masterTorsions() const;
    // Return whether named master Torsion parameters exist
    OptionalReferenceWrapper<MasterIntra> getMasterTorsion(std::string_view name);
    OptionalReferenceWrapper<const MasterIntra> getMasterTorsion(std::string_view name) const;
    // Add new master Improper parameters
    MasterIntra &addMasterImproper(std::string_view name);
    // Return number of master Impropers parameters in list
    int nMasterImpropers() const;
    // Return list of master Improper parameters
    std::list<MasterIntra> &masterImpropers();
    const std::list<MasterIntra> &masterImpropers() const;
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
    List<Configuration> configurations_;

    public:
    // Add new Configuration
    Configuration *addConfiguration();
    // Remove specified Configuration
    void removeConfiguration(Configuration *cfg);
    // Return number of Configuration in list
    int nConfigurations() const;
    // Return core Configuration list
    List<Configuration> &configurations();
    const List<Configuration> &configurations() const;
    // Return nth Configuration in list
    Configuration *configuration(int n);
    // Generate unique Configuration name with base name provided
    std::string uniqueConfigurationName(std::string_view base) const;
    // Search for Configuration by name
    Configuration *findConfiguration(std::string_view name) const;

    /*
     * Module Instances
     */
    private:
    // Pointer to Module instances list
    const RefList<Module> *moduleInstances_;

    public:
    // Set target Module instances list
    void setModuleInstances(RefList<Module> *moduleInstances);
    // Search for any instance of any module with the specified unique name
    Module *findModule(std::string_view uniqueName) const;
    // Search for and return any instance(s) of the specified Module type
    RefList<Module> findModules(std::string_view moduleType) const;
    // Search for and return any instance(s) of the specified List of Module types
    RefList<Module> findModules(const std::vector<std::string> moduleTypes) const;
    // Search for and return any instance(s) of the templated module class
    template <class M> RefList<M> findModulesByClass() const
    {
        RefList<M> modules;

        for (auto module : *moduleInstances_)
        {
            M *castModule = dynamic_cast<M *>(module);
            if (castModule)
                modules.append(castModule);
        }

        return modules;
    }

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
