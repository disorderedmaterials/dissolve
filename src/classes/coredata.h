/*
    *** Core Data
    *** src/classes/coredata.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "base/version.h"
#include "classes/masterintra.h"
#include "templates/list.h"
#include "templates/reflist.h"
#include <memory>
#include <optional>
#include <vector>

// Forward Declarations
class AtomType;
class Configuration;
class Element;
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
    std::shared_ptr<AtomType> addAtomType(Element *el);
    // Remove specified AtomType
    void removeAtomType(std::shared_ptr<AtomType> at);
    // Return number of AtomTypes in list
    int nAtomTypes() const;
    // Return core AtomTypes list
    std::vector<std::shared_ptr<AtomType>> &atomTypes();
    // Return core AtomTypes list (const)
    const std::vector<std::shared_ptr<AtomType>> &constAtomTypes() const;
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
    List<MasterIntra> masterBonds_;
    // List of master Angles parameters for Species
    List<MasterIntra> masterAngles_;
    // List of master Torsions parameters for Species
    List<MasterIntra> masterTorsions_;
    // List of master Improper parameters for Species
    List<MasterIntra> masterImpropers_;

    public:
    // Add new master Bond parameters
    MasterIntra *addMasterBond(std::string_view name);
    // Return number of master Bond parameters in list
    int nMasterBonds() const;
    // Return list of master Bond parameters
    const List<MasterIntra> &masterBonds() const;
    // Return nth master Bond
    MasterIntra *masterBond(int n);
    // Return whether named master Bond parameters exist
    MasterIntra *hasMasterBond(std::string_view name) const;
    // Add new master Angle parameters
    MasterIntra *addMasterAngle(std::string_view name);
    // Return number of master Angles parameters in list
    int nMasterAngles() const;
    // Return list of master Angle parameters
    const List<MasterIntra> &masterAngles() const;
    // Return nth master Angle parameters
    MasterIntra *masterAngle(int n);
    // Return whether named master Angle parameters exist
    MasterIntra *hasMasterAngle(std::string_view name) const;
    // Add new master Torsion parameters
    MasterIntra *addMasterTorsion(std::string_view name);
    // Return number of master Torsions parameters in list
    int nMasterTorsions() const;
    // Return list of master Torsion parameters
    const List<MasterIntra> &masterTorsions() const;
    // Return nth master Torsion parameters
    MasterIntra *masterTorsion(int n);
    // Return whether named master Torsion parameters exist
    MasterIntra *hasMasterTorsion(std::string_view name) const;
    // Add new master Improper parameters
    MasterIntra *addMasterImproper(std::string_view name);
    // Return number of master Impropers parameters in list
    int nMasterImpropers() const;
    // Return list of master Improper parameters
    const List<MasterIntra> &masterImpropers() const;
    // Return nth master Improper parameters
    MasterIntra *masterImproper(int n);
    // Return whether named master Improper parameters exist
    MasterIntra *hasMasterImproper(std::string_view name) const;
    // Return the named master term (of any form) if it exists
    MasterIntra *findMasterTerm(std::string_view name) const;
    // Clear all master terms
    void clearMasterTerms();

    /*
     * Species
     */
    private:
    // Core Species list
    List<Species> species_;

    public:
    // Add new Species
    Species *addSpecies();
    // Remove specified Species
    void removeSpecies(Species *sp);
    // Return number of Species in list
    int nSpecies() const;
    // Return core Species list
    List<Species> &species();
    // Return core Species list (const)
    const List<Species> &constSpecies() const;
    // Return nth Species in list
    Species *species(int n);
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
    // Return core Configuration list (const)
    const List<Configuration> &constConfigurations() const;
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
