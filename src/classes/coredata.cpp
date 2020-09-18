/*
    *** Core Data
    *** src/classes/coredata.cpp
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
std::shared_ptr<AtomType> CoreData::addAtomType(Element *el)
{
    auto newAtomType = std::make_shared<AtomType>();
    atomTypes_.push_back(newAtomType);

    // Create a suitable unique name
    newAtomType->setName(uniqueAtomTypeName(el->symbol()));

    // Set data
    newAtomType->setElement(el);
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

// Return core AtomTypes list (const)
const std::vector<std::shared_ptr<AtomType>> &CoreData::constAtomTypes() const { return atomTypes_; }

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
MasterIntra *CoreData::addMasterBond(std::string_view name)
{
    // Check for existence of master Bond already
    if (hasMasterBond(name))
    {
        Messenger::error("Refused to add a new master Bond named '{}' since one with the same name already exists.\n", name);
        return nullptr;
    }

    // OK to add new master Bond
    MasterIntra *b = masterBonds_.add();
    b->setName(name);
    b->setType(SpeciesIntra::BondInteraction);

    return b;
}

// Return number of master Bond parameters in list
int CoreData::nMasterBonds() const { return masterBonds_.nItems(); }

// Return list of master Bond parameters
const List<MasterIntra> &CoreData::masterBonds() const { return masterBonds_; }

// Return nth master Bond parameters
MasterIntra *CoreData::masterBond(int n) { return masterBonds_[n]; }

// Return whether named master Bond parameters exist
MasterIntra *CoreData::hasMasterBond(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto *b = masterBonds_.first(); b != nullptr; b = b->next())
        if (DissolveSys::sameString(trimmedName, b->name()))
            return b;
    return nullptr;
}

// Add new master Angle parameters
MasterIntra *CoreData::addMasterAngle(std::string_view name)
{
    // Check for existence of master Angle already
    if (hasMasterAngle(name))
    {
        Messenger::error("Refused to add a new master Angle named '{}' since one with the same name already exists.\n", name);
        return nullptr;
    }

    // OK to add new master Angle
    MasterIntra *a = masterAngles_.add();
    a->setName(name);
    a->setType(SpeciesIntra::AngleInteraction);

    return a;
}

// Return number of master Angle parameters in list
int CoreData::nMasterAngles() const { return masterAngles_.nItems(); }

// Return list of master Angle parameters
const List<MasterIntra> &CoreData::masterAngles() const { return masterAngles_; }

// Return nth master Angle parameters
MasterIntra *CoreData::masterAngle(int n) { return masterAngles_[n]; }

// Return whether named master Angle parameters exist
MasterIntra *CoreData::hasMasterAngle(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto *a = masterAngles_.first(); a != nullptr; a = a->next())
        if (DissolveSys::sameString(trimmedName, a->name()))
            return a;
    return nullptr;
}

// Add new master Torsion parameters
MasterIntra *CoreData::addMasterTorsion(std::string_view name)
{
    // Check for existence of master Torsion already
    if (hasMasterTorsion(name))
    {
        Messenger::error("Refused to add a new master Torsion named '{}' since one with the same name already exists.\n", name);
        return nullptr;
    }

    // OK to add new master Torsion
    MasterIntra *t = masterTorsions_.add();
    t->setName(name);
    t->setType(SpeciesIntra::TorsionInteraction);

    return t;
}

// Return number of master Torsion parameters in list
int CoreData::nMasterTorsions() const { return masterTorsions_.nItems(); }

// Return list of master Torsion parameters
const List<MasterIntra> &CoreData::masterTorsions() const { return masterTorsions_; }

// Return nth master Torsion parameters
MasterIntra *CoreData::masterTorsion(int n) { return masterTorsions_[n]; }

// Return whether named master Torsion parameters exist
MasterIntra *CoreData::hasMasterTorsion(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto *t = masterTorsions_.first(); t != nullptr; t = t->next())
        if (DissolveSys::sameString(trimmedName, t->name()))
            return t;
    return nullptr;
}

// Add new master Improper parameters
MasterIntra *CoreData::addMasterImproper(std::string_view name)
{
    // Check for existence of master Improper already
    if (hasMasterImproper(name))
    {
        Messenger::error("Refused to add a new master Improper named '{}' since one with the same name already exists.\n",
                         name);
        return nullptr;
    }

    // OK to add new master Improper
    MasterIntra *i = masterImpropers_.add();
    i->setName(name);
    i->setType(SpeciesIntra::ImproperInteraction);

    return i;
}

// Return number of master Improper parameters in list
int CoreData::nMasterImpropers() const { return masterImpropers_.nItems(); }

// Return list of master Improper parameters
const List<MasterIntra> &CoreData::masterImpropers() const { return masterImpropers_; }

// Return nth master Improper parameters
MasterIntra *CoreData::masterImproper(int n) { return masterImpropers_[n]; }

// Return whether named master Improper parameters exist
MasterIntra *CoreData::hasMasterImproper(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto *t = masterImpropers_.first(); t != nullptr; t = t->next())
        if (DissolveSys::sameString(trimmedName, t->name()))
            return t;
    return nullptr;
}

// Return the named master term (of any form) if it exists
MasterIntra *CoreData::findMasterTerm(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto *b = masterBonds_.first(); b != nullptr; b = b->next())
        if (DissolveSys::sameString(trimmedName, b->name()))
            return b;
    for (auto *a = masterAngles_.first(); a != nullptr; a = a->next())
        if (DissolveSys::sameString(trimmedName, a->name()))
            return a;
    for (auto *t = masterTorsions_.first(); t != nullptr; t = t->next())
        if (DissolveSys::sameString(trimmedName, t->name()))
            return t;
    for (auto *i = masterImpropers_.first(); i != nullptr; i = i->next())
        if (DissolveSys::sameString(trimmedName, i->name()))
            return i;

    return nullptr;
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
    Species *newSpecies = species_.add();

    // Create a suitable unique name
    newSpecies->setName(uniqueSpeciesName("NewSpecies"));

    return newSpecies;
}

// Remove specified Species
void CoreData::removeSpecies(Species *sp) { species_.remove(sp); }

// Return number of Species in list
int CoreData::nSpecies() const { return species_.nItems(); }

// Return core Species list
List<Species> &CoreData::species() { return species_; }

// Return core Species list (const)
const List<Species> &CoreData::constSpecies() const { return species_; }

// Return nth Species in list
Species *CoreData::species(int n) { return species_[n]; }

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
    for (auto *sp = species_.first(); sp != nullptr; sp = sp->next())
        if (DissolveSys::sameString(sp->name(), name))
            return sp;

    return nullptr;
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

// Return core Configuration list (const)
const List<Configuration> &CoreData::constConfigurations() const { return configurations_; }

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
