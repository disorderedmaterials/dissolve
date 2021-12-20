// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#include "classes/coredata.h"
#include "base/sysfunc.h"
#include "classes/atomtype.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/speciesangle.h"
#include "classes/speciesbond.h"
#include "classes/speciestorsion.h"
#include "module/module.h"

// Clear all data
void CoreData::clear()
{
    configurations_.clear();
    species_.clear();
    masterBonds_.clear();
    masterAngles_.clear();
    masterTorsions_.clear();
    masterImpropers_.clear();
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
void CoreData::removeAtomType(const std::shared_ptr<AtomType> &at)
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

// Remove any atom types that are unused across all species
int CoreData::removeUnusedAtomTypes()
{
    // Create an AtomTypeMix over all species
    AtomTypeMix mix;
    for (auto &sp : species_)
        mix.add(sp->atomTypes());

    auto nRemoved = atomTypes_.size();
    auto it =
        std::remove_if(atomTypes_.begin(), atomTypes_.end(), [&](const auto &atomType) { return !mix.contains(atomType); });
    if (it != atomTypes_.end())
        atomTypes_.erase(it, atomTypes_.end());

    return nRemoved - atomTypes_.size();
}

// Clear all atom types
void CoreData::clearAtomTypes() { atomTypes_.clear(); }

/*
 * Master Intramolecular Terms
 */

// Add new master Bond parameters
SpeciesBond &CoreData::addMasterBond(std::string_view name)
{
    // Check for existence of master Bond already
    if (getMasterBond(name))
        throw(std::runtime_error(
            fmt::format("Refused to add a new master Bond named '{}' since one with the same name already exists.\n", name)));

    // OK to add new master Bond
    auto masterBond = std::make_shared<SpeciesBond>();
    masterBond->setName(name);
    masterBonds_.push_back(masterBond);
    return *masterBond;
}

// Return number of master Bond parameters in list
int CoreData::nMasterBonds() const { return masterBonds_.size(); }

// Return list of master Bond parameters
std::vector<std::shared_ptr<SpeciesBond>> &CoreData::masterBonds() { return masterBonds_; }
const std::vector<std::shared_ptr<SpeciesBond>> &CoreData::masterBonds() const { return masterBonds_; }

// Return whether named master Bond parameters exist
OptionalReferenceWrapper<const SpeciesBond> CoreData::getMasterBond(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &b : masterBonds_)
        if (DissolveSys::sameString(trimmedName, b->name()))
            return *b;
    return {};
}

// Return whether named master Bond parameters exist
OptionalReferenceWrapper<SpeciesBond> CoreData::getMasterBond(std::string_view name)
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &b : masterBonds_)
        if (DissolveSys::sameString(trimmedName, b->name()))
            return *b;
    return {};
}

// Add new master Angle parameters
SpeciesAngle &CoreData::addMasterAngle(std::string_view name)
{
    // Check for existence of master Angle already
    if (getMasterAngle(name))
        throw(std::runtime_error(
            fmt::format("Refused to add a new master Angle named '{}' since one with the same name already exists.\n", name)));

    // OK to add new master Angle
    auto masterAngle = std::make_shared<SpeciesAngle>();
    masterAngle->setName(name);
    masterAngles_.push_back(masterAngle);
    return *masterAngle;
}

// Return number of master Angle parameters in list
int CoreData::nMasterAngles() const { return masterAngles_.size(); }

// Return list of master Angle parameters
std::vector<std::shared_ptr<SpeciesAngle>> &CoreData::masterAngles() { return masterAngles_; }
const std::vector<std::shared_ptr<SpeciesAngle>> &CoreData::masterAngles() const { return masterAngles_; }

// Return whether named master Angle parameters exist
OptionalReferenceWrapper<SpeciesAngle> CoreData::getMasterAngle(std::string_view name)
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &a : masterAngles_)
        if (DissolveSys::sameString(trimmedName, a->name()))
            return *a;
    return {};
}

// Return whether named master Angle parameters exist
OptionalReferenceWrapper<const SpeciesAngle> CoreData::getMasterAngle(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &a : masterAngles_)
        if (DissolveSys::sameString(trimmedName, a->name()))
            return *a;
    return {};
}

// Add new master Torsion parameters
SpeciesTorsion &CoreData::addMasterTorsion(std::string_view name)
{
    // Check for existence of master Torsion already
    if (getMasterTorsion(name))
        throw(std::runtime_error(fmt::format(
            "Refused to add a new master Torsion named '{}' since one with the same name already exists.\n", name)));

    // OK to add new master Torsion
    auto masterTorsion = std::make_shared<SpeciesTorsion>();
    masterTorsion->setName(name);
    masterTorsions_.push_back(masterTorsion);
    return *masterTorsion;
}

// Return number of master Torsion parameters in list
int CoreData::nMasterTorsions() const { return masterTorsions_.size(); }

// Return list of master Torsion parameters
std::vector<std::shared_ptr<SpeciesTorsion>> &CoreData::masterTorsions() { return masterTorsions_; }
const std::vector<std::shared_ptr<SpeciesTorsion>> &CoreData::masterTorsions() const { return masterTorsions_; }

// Return whether named master Torsion parameters exist
OptionalReferenceWrapper<const SpeciesTorsion> CoreData::getMasterTorsion(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &t : masterTorsions_)
        if (DissolveSys::sameString(trimmedName, t->name()))
            return *t;
    return {};
}

// Return whether named master Torsion parameters exist
OptionalReferenceWrapper<SpeciesTorsion> CoreData::getMasterTorsion(std::string_view name)
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &t : masterTorsions_)
        if (DissolveSys::sameString(trimmedName, t->name()))
            return *t;
    return {};
}

// Add new master Improper parameters
SpeciesImproper &CoreData::addMasterImproper(std::string_view name)
{
    // Check for existence of master Improper already
    if (getMasterImproper(name))
        throw(std::runtime_error(fmt::format(
            "Refused to add a new master Improper named '{}' since one with the same name already exists.\n", name)));

    // OK to add new master Improper
    auto masterImproper = std::make_shared<SpeciesImproper>();
    masterImproper->setName(name);
    masterImpropers_.push_back(masterImproper);
    return *masterImproper;
}

// Return number of master Improper parameters in list
int CoreData::nMasterImpropers() const { return masterImpropers_.size(); }

// Return list of master Improper parameters
std::vector<std::shared_ptr<SpeciesImproper>> &CoreData::masterImpropers() { return masterImpropers_; }
const std::vector<std::shared_ptr<SpeciesImproper>> &CoreData::masterImpropers() const { return masterImpropers_; }

// Return whether named master Improper parameters exist
OptionalReferenceWrapper<const SpeciesImproper> CoreData::getMasterImproper(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &i : masterImpropers_)
        if (DissolveSys::sameString(trimmedName, i->name()))
            return *i;
    return {};
}

// Return whether named master Improper parameters exist
OptionalReferenceWrapper<SpeciesImproper> CoreData::getMasterImproper(std::string_view name)
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &i : masterImpropers_)
        if (DissolveSys::sameString(trimmedName, i->name()))
            return *i;
    return {};
}

// Clear all master terms
void CoreData::clearMasterTerms()
{
    masterBonds_.clear();
    masterAngles_.clear();
    masterTorsions_.clear();
    masterImpropers_.clear();
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
    auto &newConfiguration = configurations_.emplace_back(std::make_unique<Configuration>());

    // Create a suitable unique name
    newConfiguration->setName(uniqueConfigurationName("NewConfiguration"));

    return newConfiguration.get();
}

// Remove specified Configuration
void CoreData::removeConfiguration(Configuration *cfg)
{
    configurations_.erase(
        std::remove_if(configurations_.begin(), configurations_.end(), [cfg](const auto &c) { return cfg == c.get(); }),
        configurations_.end());
}

// Return number of Configurations in list
int CoreData::nConfigurations() const { return configurations_.size(); }

// Return core Configurations list
std::vector<std::unique_ptr<Configuration>> &CoreData::configurations() { return configurations_; }

const std::vector<std::unique_ptr<Configuration>> &CoreData::configurations() const { return configurations_; }

// Return nth Configuration in list
Configuration *CoreData::configuration(int n) { return configurations_[n].get(); }

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
    auto it = std::find_if(configurations_.begin(), configurations_.end(),
                           [&name](const auto &cfg) { return DissolveSys::sameString(cfg->name(), name); });
    if (it == configurations_.end())
        return nullptr;
    return it->get();
}

/*
 * Input Filename
 */

// Set pointer to the current input filename
void CoreData::setInputFilename(std::string_view filename) { inputFilename_ = filename; }

// Return the current input filename
std::string_view CoreData::inputFilename() const { return inputFilename_; }
