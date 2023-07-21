// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/coreData.h"
#include "base/sysFunc.h"
#include "classes/atomType.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/speciesAngle.h"
#include "classes/speciesBond.h"
#include "classes/speciesTorsion.h"
#include "module/module.h"

// Clear all data
void CoreData::clear()
{
    configurations_.clear();
    species_.clear();
    masters_.bonds.clear();
    masters_.angles.clear();
    masters_.torsions.clear();
    masters_.impropers.clear();
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
    newAtomType->setName(DissolveSys::uniqueName(
        Elements::symbol(Z), atomTypes_, [&](const auto &at) { return newAtomType == at ? std::string() : at->name(); }));

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
MasterBond &CoreData::addMasterBond(std::string_view name)
{
    // Check for existence of master Bond already
    if (getMasterBond(name))
        throw(std::runtime_error(
            fmt::format("Refused to add a new master Bond named '{}' since one with the same name already exists.\n", name)));

    return *masters_.bonds.emplace_back(std::make_shared<MasterBond>(name));
}

// Return number of master Bond parameters in list
int CoreData::nMasterBonds() const { return masters_.bonds.size(); }

// Return list of master Bond parameters
std::vector<std::shared_ptr<MasterBond>> &CoreData::masterBonds() { return masters_.bonds; }
const std::vector<std::shared_ptr<MasterBond>> &CoreData::masterBonds() const { return masters_.bonds; }

// Return whether named master Bond parameters exist
OptionalReferenceWrapper<const MasterBond> CoreData::getMasterBond(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &b : masters_.bonds)
        if (DissolveSys::sameString(trimmedName, b->name()))
            return *b;
    return {};
}

// Return whether named master Bond parameters exist
OptionalReferenceWrapper<MasterBond> CoreData::getMasterBond(std::string_view name)
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &b : masters_.bonds)
        if (DissolveSys::sameString(trimmedName, b->name()))
            return *b;
    return {};
}

// Add new master Angle parameters
MasterAngle &CoreData::addMasterAngle(std::string_view name)
{
    // Check for existence of master Angle already
    if (getMasterAngle(name))
        throw(std::runtime_error(
            fmt::format("Refused to add a new master Angle named '{}' since one with the same name already exists.\n", name)));

    return *masters_.angles.emplace_back(std::make_shared<MasterAngle>(name));
}

// Return number of master Angle parameters in list
int CoreData::nMasterAngles() const { return masters_.angles.size(); }

// Return list of master Angle parameters
std::vector<std::shared_ptr<MasterAngle>> &CoreData::masterAngles() { return masters_.angles; }
const std::vector<std::shared_ptr<MasterAngle>> &CoreData::masterAngles() const { return masters_.angles; }

// Return whether named master Angle parameters exist
OptionalReferenceWrapper<MasterAngle> CoreData::getMasterAngle(std::string_view name)
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &a : masters_.angles)
        if (DissolveSys::sameString(trimmedName, a->name()))
            return *a;
    return {};
}

// Return whether named master Angle parameters exist
OptionalReferenceWrapper<const MasterAngle> CoreData::getMasterAngle(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &a : masters_.angles)
        if (DissolveSys::sameString(trimmedName, a->name()))
            return *a;
    return {};
}

// Add new master Torsion parameters
MasterTorsion &CoreData::addMasterTorsion(std::string_view name)
{
    // Check for existence of master Torsion already
    if (getMasterTorsion(name))
        throw(std::runtime_error(fmt::format(
            "Refused to add a new master Torsion named '{}' since one with the same name already exists.\n", name)));

    return *masters_.torsions.emplace_back(std::make_shared<MasterTorsion>(name));
}

// Return number of master Torsion parameters in list
int CoreData::nMasterTorsions() const { return masters_.torsions.size(); }

// Return list of master Torsion parameters
std::vector<std::shared_ptr<MasterTorsion>> &CoreData::masterTorsions() { return masters_.torsions; }
const std::vector<std::shared_ptr<MasterTorsion>> &CoreData::masterTorsions() const { return masters_.torsions; }

// Return whether named master Torsion parameters exist
OptionalReferenceWrapper<const MasterTorsion> CoreData::getMasterTorsion(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &t : masters_.torsions)
        if (DissolveSys::sameString(trimmedName, t->name()))
            return *t;
    return {};
}

// Return whether named master Torsion parameters exist
OptionalReferenceWrapper<MasterTorsion> CoreData::getMasterTorsion(std::string_view name)
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &t : masters_.torsions)
        if (DissolveSys::sameString(trimmedName, t->name()))
            return *t;
    return {};
}

// Add new master Improper parameters
MasterImproper &CoreData::addMasterImproper(std::string_view name)
{
    // Check for existence of master Improper already
    if (getMasterImproper(name))
        throw(std::runtime_error(fmt::format(
            "Refused to add a new master Improper named '{}' since one with the same name already exists.\n", name)));

    return *masters_.impropers.emplace_back(std::make_shared<MasterImproper>(name));
}

// Return number of master Improper parameters in list
int CoreData::nMasterImpropers() const { return masters_.impropers.size(); }

// Return list of master Improper parameters
std::vector<std::shared_ptr<MasterImproper>> &CoreData::masterImpropers() { return masters_.impropers; }
const std::vector<std::shared_ptr<MasterImproper>> &CoreData::masterImpropers() const { return masters_.impropers; }

// Return whether named master Improper parameters exist
OptionalReferenceWrapper<const MasterImproper> CoreData::getMasterImproper(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &i : masters_.impropers)
        if (DissolveSys::sameString(trimmedName, i->name()))
            return *i;
    return {};
}

// Return whether named master Improper parameters exist
OptionalReferenceWrapper<MasterImproper> CoreData::getMasterImproper(std::string_view name)
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &i : masters_.impropers)
        if (DissolveSys::sameString(trimmedName, i->name()))
            return *i;
    return {};
}

// Clear all master terms
void CoreData::clearMasterTerms()
{
    masters_.bonds.clear();
    masters_.angles.clear();
    masters_.torsions.clear();
    masters_.impropers.clear();
}

/*
 * Species
 */

// Add new Species
Species *CoreData::addSpecies()
{
    auto &newSpecies = species_.emplace_back(std::make_unique<Species>());

    // Create a suitable unique name
    newSpecies->setName(DissolveSys::uniqueName("NewSpecies", species_,
                                                [&](const auto &sp) { return newSpecies == sp ? std::string() : sp->name(); }));

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

// Copy AtomType, creating a new one if necessary
void CoreData::copyAtomType(const SpeciesAtom *sourceAtom, SpeciesAtom *destAtom)
{
    // Check for no AtomType being set
    if (!sourceAtom->atomType())
    {
        destAtom->setAtomType(nullptr);
        return;
    }

    // Search for the existing atom's AtomType by name, and create it if it doesn't exist
    auto at = findAtomType(sourceAtom->atomType()->name());
    if (!at)
    {
        at = addAtomType(sourceAtom->Z());
        at->setName(sourceAtom->atomType()->name());
        at->interactionPotential() = sourceAtom->atomType()->interactionPotential();
    }

    destAtom->setAtomType(at);
}

// Copy intramolecular interaction parameters, adding master term if necessary
void CoreData::copySpeciesBond(const SpeciesBond &source, SpeciesBond &dest)
{
    if (source.masterTerm())
    {
        auto master = getMasterBond(source.masterTerm()->name());
        if (!master)
            master = addMasterBond(source.masterTerm()->name());

        master->get().setInteractionFormAndParameters(source.interactionForm(), source.interactionParameters());
        dest.setMasterTerm(&master->get());
    }
    else
        dest.setInteractionFormAndParameters(source.interactionForm(), source.interactionParameters());
}
void CoreData::copySpeciesAngle(const SpeciesAngle &source, SpeciesAngle &dest)
{
    if (source.masterTerm())
    {
        auto master = getMasterAngle(source.masterTerm()->name());
        if (!master)
            master = addMasterAngle(source.masterTerm()->name());

        master->get().setInteractionFormAndParameters(source.interactionForm(), source.interactionParameters());
        dest.setMasterTerm(&master->get());
    }
    else
        dest.setInteractionFormAndParameters(source.interactionForm(), source.interactionParameters());
}
void CoreData::copySpeciesTorsion(const SpeciesTorsion &source, SpeciesTorsion &dest)
{
    if (source.masterTerm())
    {
        auto master = getMasterTorsion(source.masterTerm()->name());
        if (!master)
            master = addMasterTorsion(source.masterTerm()->name());

        master->get().setInteractionFormAndParameters(source.interactionForm(), source.interactionParameters());
        dest.setMasterTerm(&master->get());
    }
    else
        dest.setInteractionFormAndParameters(source.interactionForm(), source.interactionParameters());
}
void CoreData::copySpeciesImproper(const SpeciesImproper &source, SpeciesImproper &dest)
{
    if (source.masterTerm())
    {
        auto master = getMasterImproper(source.masterTerm()->name());
        if (!master)
            master = addMasterImproper(source.masterTerm()->name());

        master->get().setInteractionFormAndParameters(source.interactionForm(), source.interactionParameters());
        dest.setMasterTerm(&master->get());
    }
    else
        dest.setInteractionFormAndParameters(source.interactionForm(), source.interactionParameters());
}

// Copy Species from supplied instance
Species *CoreData::copySpecies(const Species *species)
{
    // Create our new Species
    Species *newSpecies = addSpecies();
    newSpecies->setName(DissolveSys::uniqueName(
        species->name(), species_, [&](const auto &sp) { return newSpecies == sp.get() ? std::string() : sp->name(); }));

    // Copy Box definition if one exists
    if (species->box()->type() != Box::BoxType::NonPeriodic)
        newSpecies->createBox(species->box()->axisLengths(), species->box()->axisAngles());

    // Duplicate atoms
    for (auto &i : species->atoms())
    {
        // Create the Atom in our new Species
        auto id = newSpecies->addAtom(i.Z(), i.r(), i.charge());
        if (i.isSelected())
            newSpecies->selectAtom(id);

        // Search for the existing atom's AtomType by name, and create it if it doesn't exist
        copyAtomType(&i, &newSpecies->atom(id));
    }

    // Duplicate bonds
    for (const auto &bond : species->bonds())
    {
        // Create the bond in the new Species
        auto &newBond = newSpecies->addBond(bond.indexI(), bond.indexJ());
        copySpeciesBond(bond, newBond);
    }

    // Duplicate angles
    for (const auto &angle : species->angles())
    {
        // Create the angle in the new Species
        auto &newAngle = newSpecies->addAngle(angle.indexI(), angle.indexJ(), angle.indexK());
        copySpeciesAngle(angle, newAngle);
    }

    // Duplicate torsions
    for (const auto &torsion : species->torsions())
    {
        // Create the torsion in the new Species
        auto &newTorsion = newSpecies->addTorsion(torsion.indexI(), torsion.indexJ(), torsion.indexK(), torsion.indexL());
        copySpeciesTorsion(torsion, newTorsion);
    }

    // Duplicate impropers
    for (const auto &improper : species->impropers())
    {
        // Create the improper in the new Species
        auto &newImproper = newSpecies->addImproper(improper.indexI(), improper.indexJ(), improper.indexK(), improper.indexL());
        copySpeciesImproper(improper, newImproper);
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
    newConfiguration->setName(DissolveSys::uniqueName("NewConfiguration", configurations_,
                                                      [&](const auto &cfg)
                                                      { return newConfiguration == cfg ? std::string() : cfg->name(); }));

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

// Express as a serialisable value
SerialisedValue CoreData::Masters::serialise() const
{
    SerialisedValue node;
    Serialisable::fromVectorToTable<>(bonds, "bonds", node);
    Serialisable::fromVectorToTable<>(angles, "angles", node);
    Serialisable::fromVectorToTable<>(torsions, "torsions", node);
    Serialisable::fromVectorToTable<>(impropers, "impropers", node);
    return node;
}

// Read values from a serialisable value
void CoreData::Masters::deserialise(const SerialisedValue &node)
{
    Serialisable::toMap(node, "bonds",
                        [this](const std::string &name, const SerialisedValue &bond)
                        { bonds.emplace_back(std::make_unique<MasterBond>(name))->deserialise(bond); });
    Serialisable::toMap(node, "angles",
                        [this](const std::string &name, const SerialisedValue &angle)
                        { angles.emplace_back(std::make_unique<MasterAngle>(name))->deserialise(angle); });
    Serialisable::toMap(node, "torsions",
                        [this](const std::string &name, const SerialisedValue &torsion)
                        { torsions.emplace_back(std::make_unique<MasterTorsion>(name))->deserialise(torsion); });
    Serialisable::toMap(node, "impropers",
                        [this](const std::string &name, const SerialisedValue &improper)
                        { impropers.emplace_back(std::make_unique<MasterImproper>(name))->deserialise(improper); });
    return;
}

// Express Master terms as serialisable value
SerialisedValue CoreData::serialiseMaster() const { return masters_.serialise(); }

// Read Master values from serialisable value
void CoreData::deserialiseMaster(const SerialisedValue &node) { masters_.deserialise(node); }
