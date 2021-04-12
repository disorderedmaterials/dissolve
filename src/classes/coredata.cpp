// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/coredata.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/speciesangle.h"
#include "classes/speciesbond.h"
#include "classes/speciestorsion.h"
#include "module/list.h"
#include "module/module.h"

CoreData::CoreData() { moduleInstances_ = nullptr; }

CoreData::~CoreData() {}

// Clear all data
void CoreData::clear()
{
    configurations_.clear();
    species_.clear();
    masterBonds_.clear();
    masterAngles_.clear();
    masterTorsions_.clear();
    atomTypes_.clear();
}

/*
 * Atom Types
 */

// Add new AtomType
std::shared_ptr<AtomType> CoreData::addAtomType(Elements::Element Z)
{
    auto newAtomType = std::make_shared<AtomType>();
    atomTypes_.push_back(newAtomType);

    // Create a suitable unique name
    newAtomType->setName(uniqueAtomTypeName(Elements::symbol(Z)));

    // Set data
    newAtomType->setZ(Z);
    newAtomType->setIndex(nAtomTypes() - 1);

    // Bump version
    ++atomTypesVersion_;

    return newAtomType;
}

// Remove specified AtomType
void CoreData::removeAtomType(std::shared_ptr<AtomType> at)
{
    atomTypes_.erase(std::remove(atomTypes_.begin(), atomTypes_.end(), at));
}

// Return number of AtomTypes in list
int CoreData::nAtomTypes() const { return atomTypes_.size(); }

// Return core AtomTypes list
std::vector<std::shared_ptr<AtomType>> &CoreData::atomTypes() { return atomTypes_; }

const std::vector<std::shared_ptr<AtomType>> &CoreData::atomTypes() const { return atomTypes_; }

// Return nth AtomType in list
std::shared_ptr<AtomType> CoreData::atomType(int n) { return atomTypes_[n]; }

// Generate unique AtomType name with base name provided
std::string CoreData::uniqueAtomTypeName(std::string_view base) const
{
    std::string uniqueName{base};

    // Find an unused name starting with the baseName provided
    auto suffix = 0;
    while (findAtomType(uniqueName))
        uniqueName = fmt::format("{}{}", base, ++suffix);

    return uniqueName;
}

// Search for AtomType by name
std::shared_ptr<AtomType> CoreData::findAtomType(std::string_view name) const
{
    auto it = std::find_if(atomTypes_.begin(), atomTypes_.end(),
                           [&name](const auto &at) { return DissolveSys::sameString(at->name(), name); });
    if (it == atomTypes_.end())
        return nullptr;
    return *it;
}

// Bump AtomTypes version
void CoreData::bumpAtomTypesVersion() { ++atomTypesVersion_; }

// Return AtomTypes version
int CoreData::atomTypesVersion() const { return atomTypesVersion_; }

/*
 * Master Intramolecular Terms
 */

// Add new master Bond parameters
MasterIntra &CoreData::addMasterBond(std::string_view name)
{
    // Check for existence of master Bond already
    if (getMasterBond(name))
        throw(std::runtime_error(
            fmt::format("Refused to add a new master Bond named '{}' since one with the same name already exists.\n", name)));

    // OK to add new master Bond
    masterBonds_.emplace_back(name, SpeciesIntra::InteractionType::Bond);
    return masterBonds_.back();
}

// Return number of master Bond parameters in list
int CoreData::nMasterBonds() const { return masterBonds_.size(); }

// Return list of master Bond parameters
std::list<MasterIntra> &CoreData::masterBonds() { return masterBonds_; }
const std::list<MasterIntra> &CoreData::masterBonds() const { return masterBonds_; }

// Return whether named master Bond parameters exist
OptionalReferenceWrapper<const MasterIntra> CoreData::getMasterBond(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &b : masterBonds_)
        if (DissolveSys::sameString(trimmedName, b.name()))
            return b;
    return {};
}

// Return whether named master Bond parameters exist
OptionalReferenceWrapper<MasterIntra> CoreData::getMasterBond(std::string_view name)
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &b : masterBonds_)
        if (DissolveSys::sameString(trimmedName, b.name()))
            return b;
    return {};
}

// Add new master Angle parameters
MasterIntra &CoreData::addMasterAngle(std::string_view name)
{
    // Check for existence of master Angle already
    if (getMasterAngle(name))
        throw(std::runtime_error(
            fmt::format("Refused to add a new master Angle named '{}' since one with the same name already exists.\n", name)));

    // OK to add new master Angle
    masterAngles_.emplace_back(name, SpeciesIntra::InteractionType::Angle);
    return masterAngles_.back();
}

// Return number of master Angle parameters in list
int CoreData::nMasterAngles() const { return masterAngles_.size(); }

// Return list of master Angle parameters
std::list<MasterIntra> &CoreData::masterAngles() { return masterAngles_; }
const std::list<MasterIntra> &CoreData::masterAngles() const { return masterAngles_; }

// Return whether named master Angle parameters exist
OptionalReferenceWrapper<MasterIntra> CoreData::getMasterAngle(std::string_view name)
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &a : masterAngles_)
        if (DissolveSys::sameString(trimmedName, a.name()))
            return a;
    return {};
}

// Return whether named master Angle parameters exist
OptionalReferenceWrapper<const MasterIntra> CoreData::getMasterAngle(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &a : masterAngles_)
        if (DissolveSys::sameString(trimmedName, a.name()))
            return a;
    return {};
}

// Add new master Torsion parameters
MasterIntra &CoreData::addMasterTorsion(std::string_view name)
{
    // Check for existence of master Torsion already
    if (getMasterTorsion(name))
        throw(std::runtime_error(fmt::format(
            "Refused to add a new master Torsion named '{}' since one with the same name already exists.\n", name)));

    // OK to add new master Torsion
    masterTorsions_.emplace_back(name, SpeciesIntra::InteractionType::Torsion);
    return masterTorsions_.back();
}

// Return number of master Torsion parameters in list
int CoreData::nMasterTorsions() const { return masterTorsions_.size(); }

// Return list of master Torsion parameters
std::list<MasterIntra> &CoreData::masterTorsions() { return masterTorsions_; }
const std::list<MasterIntra> &CoreData::masterTorsions() const { return masterTorsions_; }

// Return whether named master Torsion parameters exist
OptionalReferenceWrapper<const MasterIntra> CoreData::getMasterTorsion(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &t : masterTorsions_)
        if (DissolveSys::sameString(trimmedName, t.name()))
            return t;
    return {};
}

// Return whether named master Torsion parameters exist
OptionalReferenceWrapper<MasterIntra> CoreData::getMasterTorsion(std::string_view name)
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &t : masterTorsions_)
        if (DissolveSys::sameString(trimmedName, t.name()))
            return t;
    return {};
}

// Add new master Improper parameters
MasterIntra &CoreData::addMasterImproper(std::string_view name)
{
    // Check for existence of master Improper already
    if (getMasterImproper(name))
        throw(std::runtime_error(fmt::format(
            "Refused to add a new master Improper named '{}' since one with the same name already exists.\n", name)));

    // OK to add new master Improper
    masterImpropers_.emplace_back(name, SpeciesIntra::InteractionType::Improper);
    return masterImpropers_.back();
}

// Return number of master Improper parameters in list
int CoreData::nMasterImpropers() const { return masterImpropers_.size(); }

// Return list of master Improper parameters
std::list<MasterIntra> &CoreData::masterImpropers() { return masterImpropers_; }
const std::list<MasterIntra> &CoreData::masterImpropers() const { return masterImpropers_; }

// Return whether named master Improper parameters exist
OptionalReferenceWrapper<const MasterIntra> CoreData::getMasterImproper(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &i : masterImpropers_)
        if (DissolveSys::sameString(trimmedName, i.name()))
            return i;
    return {};
}

// Return whether named master Improper parameters exist
OptionalReferenceWrapper<MasterIntra> CoreData::getMasterImproper(std::string_view name)
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &i : masterImpropers_)
        if (DissolveSys::sameString(trimmedName, i.name()))
            return i;
    return {};
}

// Return the named master term (of any form) if it exists
OptionalReferenceWrapper<const MasterIntra> CoreData::findMasterTerm(std::string_view name) const
{
    OptionalReferenceWrapper<const MasterIntra> result;
    result = getMasterBond(name);
    if (result)
        return result;
    result = getMasterAngle(name);
    if (result)
        return result;
    result = getMasterTorsion(name);
    if (result)
        return result;
    return getMasterImproper(name);
}

// Clear all master terms
void CoreData::clearMasterTerms()
{
    masterBonds_.clear();
    masterAngles_.clear();
    masterTorsions_.clear();
}

/*
 * Species
 */

// Add new Species
Species *CoreData::addSpecies()
{
    auto &newSpecies = species_.emplace_back(std::make_unique<Species>());

    // Create a suitable unique name
    newSpecies->setName(uniqueSpeciesName("NewSpecies"));

    return newSpecies.get();
}

// Remove specified Species
void CoreData::removeSpecies(Species *sp)
{
    species_.erase(std::remove_if(species_.begin(), species_.end(), [&](const auto &p) { return sp == p.get(); }),
                   species_.end());
}

// Return number of Species in list
int CoreData::nSpecies() const { return species_.size(); }

// Return core Species list
std::vector<std::unique_ptr<Species>> &CoreData::species() { return species_; }

const std::vector<std::unique_ptr<Species>> &CoreData::species() const { return species_; }

// Generate unique Species name with base name provided
std::string CoreData::uniqueSpeciesName(std::string_view base) const
{
    std::string_view baseName = base.empty() ? "Unnamed" : base;
    std::string uniqueName{baseName};

    // Find an unused name starting with the baseName provided
    auto suffix = 0;
    while (findSpecies(uniqueName))
        uniqueName = fmt::format("{}{}", baseName, ++suffix);

    return uniqueName;
}

// Search for Species by name
Species *CoreData::findSpecies(std::string_view name) const
{
    auto it = std::find_if(species_.begin(), species_.end(),
                           [name](const auto &sp) { return DissolveSys::sameString(sp->name(), name); });
    if (it != species_.end())
    {
        return (*it).get();
    }
    else
    {
        return nullptr;
    }
}

/*
 * Configuration
 */

// Add new Configuration
Configuration *CoreData::addConfiguration()
{
    Configuration *newConfiguration = configurations_.add();

    // Create a suitable unique name
    newConfiguration->setName(uniqueConfigurationName("NewConfiguration"));

    return newConfiguration;
}

// Remove specified Configuration
void CoreData::removeConfiguration(Configuration *cfg) { configurations_.remove(cfg); }

// Return number of Configurations in list
int CoreData::nConfigurations() const { return configurations_.nItems(); }

// Return core Configurations list
List<Configuration> &CoreData::configurations() { return configurations_; }

const List<Configuration> &CoreData::configurations() const { return configurations_; }

// Return nth Configuration in list
Configuration *CoreData::configuration(int n) { return configurations_[n]; }

// Generate unique Configuration name with base name provided
std::string CoreData::uniqueConfigurationName(std::string_view base) const
{
    std::string baseName = base.empty() ? "Unnamed" : std::string(base);
    std::string uniqueName = baseName;
    auto suffix = 0;

    // Find an unused name starting with the baseName provided
    while (findConfiguration(uniqueName))
    {
        // Increase suffix value and regenerate uniqueName from baseName
        ++suffix;
        uniqueName = fmt::format("{}{}", baseName, suffix);
    }

    return uniqueName;
}

// Search for Configuration by name
Configuration *CoreData::findConfiguration(std::string_view name) const
{
    for (auto *cfg = configurations_.first(); cfg != nullptr; cfg = cfg->next())
        if (DissolveSys::sameString(cfg->name(), name))
            return cfg;

    return nullptr;
}

/*
 * Module List
 */

// Set target Module instances list
void CoreData::setModuleInstances(RefList<Module> *moduleInstances) { moduleInstances_ = moduleInstances; }

// Search for any instance of any module with the specified unique name
Module *CoreData::findModule(std::string_view uniqueName) const
{
    if (!moduleInstances_)
        return nullptr;

    for (auto module : *moduleInstances_)
        if (DissolveSys::sameString(module->uniqueName(), uniqueName))
            return module;

    return nullptr;
}

// Search for and return any instance(s) of the specified Module type
RefList<Module> CoreData::findModules(std::string_view moduleType) const
{
    RefList<Module> modules;

    for (auto module : *moduleInstances_)
        if (DissolveSys::sameString(module->type(), moduleType))
            modules.append(module);

    return modules;
}

// Search for and return any instance(s) of the specified Module type
RefList<Module> CoreData::findModules(const std::vector<std::string> moduleTypes) const
{
    RefList<Module> modules;

    for (auto module : *moduleInstances_)
        if (std::find_if(moduleTypes.cbegin(), moduleTypes.cend(), [module](const auto &s) { return s == module->type(); }) !=
            moduleTypes.cend())
            modules.append(module);

    return modules;
}

/*
 * Input Filename
 */

// Set pointer to the current input filename
void CoreData::setInputFilename(std::string_view filename) { inputFilename_ = filename; }

// Return the current input filename
std::string_view CoreData::inputFilename() const { return inputFilename_; }
