// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2026 Team Dissolve and contributors

#include "classes/species.h"
#include "classes/speciesAngle.h"
#include "classes/speciesBond.h"
#include "classes/speciesTorsion.h"

// Add new common bond parameters
CommonBond &Species::addCommonBond(std::string_view name, std::optional<int> insertAtIndex)
{
    // Check for existence of common bond already
    if (getCommonBond(name))
        Messenger::exception("Refused to add a new common bond named '{}' since one with the same name already exists.\n",
                             name);

    auto newBond = std::make_shared<CommonBond>(name);
    if (insertAtIndex)
        commonBonds_.insert(commonBonds_.begin() + *insertAtIndex, newBond);
    else
        commonBonds_.emplace_back(newBond);

    return *newBond;
}

// Remove specified common bond
void Species::removeCommonBond(const std::shared_ptr<CommonBond> &bond)
{
    for (auto &b : bonds_)
        if (b.commonTerm() == bond.get())
            b.detachFromCommonTerm();

    commonBonds_.erase(std::remove(commonBonds_.begin(), commonBonds_.end(), bond), commonBonds_.end());
}

// Return list of common bond parameters
std::vector<std::shared_ptr<CommonBond>> &Species::commonBonds() { return commonBonds_; }
const std::vector<std::shared_ptr<CommonBond>> &Species::commonBonds() const { return commonBonds_; }

// Return whether named common bond parameters exist
OptionalReferenceWrapper<const CommonBond> Species::getCommonBond(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &b : commonBonds_)
        if (DissolveSys::sameString(trimmedName, b->name()))
            return *b;
    return {};
}

// Return whether named common bond parameters exist
OptionalReferenceWrapper<CommonBond> Species::getCommonBond(std::string_view name)
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &b : commonBonds_)
        if (DissolveSys::sameString(trimmedName, b->name()))
            return *b;
    return {};
}

// Add new common angle parameters
CommonAngle &Species::addCommonAngle(std::string_view name)
{
    // Check for existence of common angle already
    if (getCommonAngle(name))
        Messenger::exception("Refused to add a new common angle named '{}' since one with the same name already exists.\n",
                             name);

    return *commonAngles_.emplace_back(std::make_shared<CommonAngle>(name));
}

// Remove specified common angle
void Species::removeCommonAngle(const std::shared_ptr<CommonAngle> &angle)
{
    for (auto &a : angles_)
        if (a.commonTerm() == angle.get())
            a.detachFromCommonTerm();

    commonAngles_.erase(std::remove(commonAngles_.begin(), commonAngles_.end(), angle), commonAngles_.end());
}

// Return list of common angle parameters
std::vector<std::shared_ptr<CommonAngle>> &Species::commonAngles() { return commonAngles_; }
const std::vector<std::shared_ptr<CommonAngle>> &Species::commonAngles() const { return commonAngles_; }

// Return whether named common angle parameters exist
OptionalReferenceWrapper<CommonAngle> Species::getCommonAngle(std::string_view name)
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &a : commonAngles_)
        if (DissolveSys::sameString(trimmedName, a->name()))
            return *a;
    return {};
}

// Return whether named common angle parameters exist
OptionalReferenceWrapper<const CommonAngle> Species::getCommonAngle(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &a : commonAngles_)
        if (DissolveSys::sameString(trimmedName, a->name()))
            return *a;
    return {};
}

// Add new common torsion parameters
CommonTorsion &Species::addCommonTorsion(std::string_view name)
{
    // Check for existence of common torsion already
    if (getCommonTorsion(name))
        Messenger::exception("Refused to add a new common torsion named '{}' since one with the same name already exists.\n",
                             name);

    return *commonTorsions_.emplace_back(std::make_shared<CommonTorsion>(name));
}

// Remove specified common torsion
void Species::removeCommonTorsion(const std::shared_ptr<CommonTorsion> &torsion)
{
    for (auto &t : torsions_)
        if (t.commonTerm() == torsion.get())
            t.detachFromCommonTerm();

    commonTorsions_.erase(std::remove(commonTorsions_.begin(), commonTorsions_.end(), torsion), commonTorsions_.end());
}

// Return list of common torsion parameters
std::vector<std::shared_ptr<CommonTorsion>> &Species::commonTorsions() { return commonTorsions_; }
const std::vector<std::shared_ptr<CommonTorsion>> &Species::commonTorsions() const { return commonTorsions_; }

// Return whether named common torsion parameters exist
OptionalReferenceWrapper<const CommonTorsion> Species::getCommonTorsion(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &t : commonTorsions_)
        if (DissolveSys::sameString(trimmedName, t->name()))
            return *t;
    return {};
}

// Return whether named common torsion parameters exist
OptionalReferenceWrapper<CommonTorsion> Species::getCommonTorsion(std::string_view name)
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &t : commonTorsions_)
        if (DissolveSys::sameString(trimmedName, t->name()))
            return *t;
    return {};
}

// Add new common improper parameters
CommonImproper &Species::addCommonImproper(std::string_view name)
{
    // Check for existence of common improper already
    if (getCommonImproper(name))
        Messenger::exception("Refused to add a new common improper named '{}' since one with the same name already exists.\n",
                             name);

    return *commonImpropers_.emplace_back(std::make_shared<CommonImproper>(name));
}

// Remove specified common Improper
void Species::removeCommonImproper(const std::shared_ptr<CommonImproper> &improper)
{
    for (auto &i : impropers_)
        if (i.commonTerm() == improper.get())
            i.detachFromCommonTerm();

    commonImpropers_.erase(std::remove(commonImpropers_.begin(), commonImpropers_.end(), improper), commonImpropers_.end());
}

// Return list of common improper parameters
std::vector<std::shared_ptr<CommonImproper>> &Species::commonImpropers() { return commonImpropers_; }
const std::vector<std::shared_ptr<CommonImproper>> &Species::commonImpropers() const { return commonImpropers_; }

// Return whether named common improper parameters exist
OptionalReferenceWrapper<const CommonImproper> Species::getCommonImproper(std::string_view name) const
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &i : commonImpropers_)
        if (DissolveSys::sameString(trimmedName, i->name()))
            return *i;
    return {};
}

// Return whether named common improper parameters exist
OptionalReferenceWrapper<CommonImproper> Species::getCommonImproper(std::string_view name)
{
    // Remove leading '@' if necessary
    std::string_view trimmedName = name[0] == '@' ? &name[1] : name;

    for (auto &i : commonImpropers_)
        if (DissolveSys::sameString(trimmedName, i->name()))
            return *i;
    return {};
}

// Clear all common terms
void Species::clearCommonTerms()
{
    commonBonds_.clear();
    commonAngles_.clear();
    commonTorsions_.clear();
    commonImpropers_.clear();
}
