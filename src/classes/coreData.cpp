// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/coreData.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/speciesAngle.h"
#include "classes/speciesBond.h"
#include "classes/speciesTorsion.h"
#include "main/dissolve.h"
#include "module/layer.h"
#include "module/module.h"

// Clear all data
void CoreData::clear()
{
    configurations_.clear();
    species_.clear();
    atomTypes_.clear();
    processingLayers_.clear();
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
    newAtomType->setName(DissolveSys::uniqueName(Elements::symbol(Z), atomTypes_,
                                                 [&](const auto &at) { return newAtomType == at ? "" : at->name(); }));

    // Set data
    newAtomType->setZ(Z);
    newAtomType->setIndex(nAtomTypes() - 1);

    return newAtomType;
}

// Remove specified AtomType
void CoreData::removeAtomType(std::shared_ptr<AtomType> &at)
{
    removeReferencesTo(at);

    atomTypes_.erase(std::remove(atomTypes_.begin(), atomTypes_.end(), at), atomTypes_.end());

    // Reassign AtomType indices
    auto count = 0;
    for (const auto &at : atomTypes_)
        at->setIndex(count++);
}

// Return number of AtomTypes in list
int CoreData::nAtomTypes() const { return atomTypes_.size(); }

// Return core AtomTypes list
std::vector<std::shared_ptr<AtomType>> &CoreData::atomTypes() { return atomTypes_; }

const std::vector<std::shared_ptr<AtomType>> &CoreData::atomTypes() const { return atomTypes_; }

// Return nth AtomType in list
std::shared_ptr<AtomType> CoreData::atomType(int n) { return atomTypes_[n]; }

// Search for AtomType by name
std::shared_ptr<AtomType> CoreData::findAtomType(std::string_view name) const
{
    auto it = std::find_if(atomTypes_.begin(), atomTypes_.end(),
                           [&name](const auto &at) { return DissolveSys::sameString(at->name(), name); });
    if (it == atomTypes_.end())
        return nullptr;
    return *it;
}

// Clear all atom types
void CoreData::clearAtomTypes() { atomTypes_.clear(); }

/*
 * Species
 */

// Add new Species
Species *CoreData::addSpecies()
{
    auto &newSpecies = species_.emplace_back(std::make_unique<Species>());

    // Create a suitable unique name
    newSpecies->setName(
        DissolveSys::uniqueName("NewSpecies", species_, [&](const auto &sp) { return newSpecies == sp ? "" : sp->name(); }));

    return newSpecies.get();
}

// Remove specified Species
void CoreData::removeSpecies(Species *sp)
{
    // Remove references to the Species itself
    removeReferencesTo(sp);

    species_.erase(std::remove_if(species_.begin(), species_.end(), [&](const auto &p) { return sp == p.get(); }),
                   species_.end());
}

// Return number of Species in list
int CoreData::nSpecies() const { return species_.size(); }

// Return core Species list
std::vector<std::unique_ptr<Species>> &CoreData::species() { return species_; }

const std::vector<std::unique_ptr<Species>> &CoreData::species() const { return species_; }

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

// Copy Species from supplied instance
Species *CoreData::copySpecies(const Species *species)
{
    // Create our new Species
    Species *newSpecies = addSpecies();
    newSpecies->setName(DissolveSys::uniqueName(species->name(), species_,
                                                [&](const auto &sp) { return newSpecies == sp.get() ? "" : sp->name(); }));

    // Copy Box definition if one exists
    if (species->box()->type() != Box::BoxType::NonPeriodic)
        newSpecies->createBox(species->box()->axisLengths(), species->box()->axisAngles());

    // Duplicate atom types
    for (auto &at : species->atomTypes())
    {
        // Create a new atom type and copy the data of the other
        auto newAt = newSpecies->addAtomType(at->Z());
        *newAt = *at;
    }

    // Duplicate atoms
    for (auto &i : species->atoms())
    {
        // Create the Atom in our new Species
        auto id = newSpecies->addAtom(i.Z(), i.r(), i.q());

        // Find and assign the atom type
        newSpecies->atom(id).setAtomType(newSpecies->findAtomType(i.atomType()->name()));
    }

    // Duplicate bonds
    for (const auto &bond : species->bonds())
    {
        // Create the bond in the new Species
        auto &newBond = newSpecies->addBond(bond.indexI(), bond.indexJ());
        // copySpeciesBond(bond, newBond);
        // TODO DISSOLVE2
    }

    // Duplicate angles
    for (const auto &angle : species->angles())
    {
        // Create the angle in the new Species
        auto &newAngle = newSpecies->addAngle(angle.indexI(), angle.indexJ(), angle.indexK());
        // copySpeciesAngle(angle, newAngle);
        // TODO DISSOLVE2
    }

    // Duplicate torsions
    for (const auto &torsion : species->torsions())
    {
        // Create the torsion in the new Species
        auto &newTorsion = newSpecies->addTorsion(torsion.indexI(), torsion.indexJ(), torsion.indexK(), torsion.indexL());
        // copySpeciesTorsion(torsion, newTorsion);
        // TODO DISSOLVE2
    }

    // Duplicate impropers
    for (const auto &improper : species->impropers())
    {
        // Create the improper in the new Species
        auto &newImproper = newSpecies->addImproper(improper.indexI(), improper.indexJ(), improper.indexK(), improper.indexL());
        // copySpeciesImproper(improper, newImproper);
        // TODO DISSOLVE2
    }

    return newSpecies;
}

/*
 * Configuration
 */

// Add new Configuration
Configuration *CoreData::addConfiguration()
{
    auto &newConfiguration = configurations_.emplace_back(std::make_unique<Configuration>());

    // Create a suitable unique name
    newConfiguration->setName(DissolveSys::uniqueName("NewConfiguration", configurations_, [&](const auto &cfg)
                                                      { return newConfiguration == cfg ? "" : cfg->name(); }));

    return newConfiguration.get();
}

// Remove specified Configuration
void CoreData::removeConfiguration(Configuration *cfg)
{
    // Remove references to the Configuration itself
    removeReferencesTo(cfg);

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

// Search for Configuration by name
Configuration *CoreData::findConfiguration(std::string_view name) const
{
    auto it = std::find_if(configurations_.begin(), configurations_.end(),
                           [&name](const auto &cfg) { return DissolveSys::sameString(cfg->name(), name); });
    if (it == configurations_.end())
        return nullptr;
    return it->get();
}

// Find configuration by 'nice' name
Configuration *CoreData::findConfigurationByNiceName(std::string_view name) const
{
    auto it = std::find_if(configurations().begin(), configurations().end(),
                           [&name](const auto &cfg) { return DissolveSys::sameString(name, cfg->name()); });
    if (it == configurations().end())
        return nullptr;
    return it->get();
}

/*
 * Layers
 */

// Add new processing layer
ModuleLayer *CoreData::addProcessingLayer() { return processingLayers_.emplace_back(std::make_unique<ModuleLayer>()).get(); }

// Remove processing layer
void CoreData::removeProcessingLayer(ModuleLayer *layer)
{
    if (!layer)
        return;

    // Remove any references to the Modules in the layer before we delete it
    for (auto &module : layer->modules())
        removeReferencesTo(module.get());

    // Now safe to remove the layer
    processingLayers_.erase(
        std::find_if(processingLayers_.begin(), processingLayers_.end(), [layer](const auto &l) { return l.get() == layer; }));
}

// Find named processing layer
ModuleLayer *CoreData::findProcessingLayer(std::string_view name) const
{
    auto it = std::find_if(processingLayers_.begin(), processingLayers_.end(),
                           [name](auto &layer) { return DissolveSys::sameString(layer->name(), name); });
    if (it == processingLayers_.end())
        return nullptr;
    return it->get();
}

// Return number of processing layers
int CoreData::nProcessingLayers() const { return processingLayers_.size(); }

// Return current processing layers
std::vector<std::unique_ptr<ModuleLayer>> &CoreData::processingLayers() { return processingLayers_; }
const std::vector<std::unique_ptr<ModuleLayer>> &CoreData::processingLayers() const { return processingLayers_; }

// Run the set-up stages of all modules in all layers
bool CoreData::setUpProcessingLayerModules(Dissolve &dissolve)
{
    auto setUpResult = true;
    for (auto &layer : processingLayers())
        if (!layer->setUpAll(dissolve))
            setUpResult = false;
    return setUpResult;
}

/*
 * Input Filename
 */

// Set pointer to the current input filename
void CoreData::setInputFilename(std::string_view filename) { inputFilename_ = filename; }

// Return the current input filename
std::string_view CoreData::inputFilename() const { return inputFilename_; }

/*
 * Object Management
 */

// Templated remove all references to the specified data in a CoreData
template <class O> void objectNoLongerValid(CoreData *coreData, O *object)
{
    // Loop over all keyword objects and call their local functions
    for (auto &layer : coreData->processingLayers())
        for (auto &mod : layer->modules())
            mod->keywords().objectNoLongerValid(object);
}

template <class P> void objectNoLongerValid(CoreData *coreData, std::shared_ptr<P> object)
{
    // Loop over all keyword objects and call their local functions
    for (auto &layer : coreData->processingLayers())
        for (auto &mod : layer->modules())
            mod->keywords().objectNoLongerValid(object);
}

// Remove all references to the specified data
void CoreData::removeReferencesTo(Module *data) { objectNoLongerValid(this, data); }
void CoreData::removeReferencesTo(Isotopologue *data) { objectNoLongerValid(this, data); }
void CoreData::removeReferencesTo(Configuration *data) { objectNoLongerValid(this, data); }
void CoreData::removeReferencesTo(Species *data)
{
    objectNoLongerValid(this, data);

    // Check Configurations - if the Species was used, we must clear the configuration contents
    for (auto &cfg : configurations_)
        if (cfg->speciesPopulations().contains(data))
            cfg->empty();
}
void CoreData::removeReferencesTo(SpeciesSite *data) { objectNoLongerValid(this, data); }
void CoreData::removeReferencesTo(std::shared_ptr<AtomType> data)
{
    for (auto &species : species_)
    {
        for (auto &atom : species->atoms())
        {
            if (atom.atomType() == data.get())
            {
                atom.setAtomType(nullptr);
            }
        }
    }

    objectNoLongerValid(this, data);
}

/*
 * Modules
 */

// Return vector of all existing Modules
const std::vector<Module *> CoreData::moduleInstances() const
{
    std::vector<Module *> result{};
    for (auto &layer : processingLayers())
        std::transform(layer->modules().begin(), layer->modules().end(), std::back_inserter(result),
                       [](auto &source) { return source.get(); });

    return result;
}

// Search for any instance of any module with the specified unique name
Module *CoreData::findModule(std::string_view uniqueName) const
{
    auto instances = moduleInstances();
    auto it = std::find_if(instances.begin(), instances.end(),
                           [uniqueName](const auto *m) { return DissolveSys::sameString(m->name(), uniqueName); });
    if (it != instances.end())
        return *it;

    return nullptr;
}

// Search for and return any instance(s) of the specified Module type
std::vector<Module *> CoreData::allOfType(ModuleTypes::ModuleType type) const
{
    return allOfType(std::vector<ModuleTypes::ModuleType>{type});
}

// Search for and return any instance(s) of the specified Module type
std::vector<Module *> CoreData::allOfType(std::vector<ModuleTypes::ModuleType> types) const
{
    std::vector<Module *> modules;
    auto instances = moduleInstances();
    std::copy_if(instances.begin(), instances.end(), std::back_inserter(modules),
                 [&types](const auto *m) { return std::find(types.begin(), types.end(), m->type()) != types.end(); });
    return modules;
}
