// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/sitestack.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/speciessite.h"
#include "data/atomicmasses.h"
#include <numeric>

SiteStack::SiteStack()
{
    configuration_ = nullptr;
    configurationIndex_ = -1;
    speciesSite_ = nullptr;
    sitesInMolecules_ = false;
    sitesHaveOrientation_ = false;
}

/*
 * Target
 */

// Return target Configuration
Configuration *SiteStack::configuration() const { return configuration_; }

// Return target SpeciesSite
const SpeciesSite *SiteStack::speciesSite() const { return speciesSite_; }

/*
 * Generation
 */

// Calculate geometric centre of atoms in the given molecule
Vec3<double> SiteStack::centreOfGeometry(const Molecule &mol, const Box *box, const std::vector<int> &indices)
{
    const auto ref = mol.atom(indices.front())->r();
    return std::accumulate(std::next(indices.begin()), indices.end(), ref,
                           [&ref, &mol, box](const auto &acc, const auto idx)
                           { return acc + box->minimumImage(mol.atom(idx)->r(), ref); }) /
           indices.size();
}

// Calculate (mass-weighted) coordinate centre of atoms in the given molecule
Vec3<double> SiteStack::centreOfMass(const Molecule &mol, const Box *box, const std::vector<int> &indices)
{
    auto mass = AtomicMass::mass(mol.atom(indices.front())->speciesAtom()->Z());
    const auto ref = mol.atom(indices.front())->r();
    auto sums = std::accumulate(std::next(indices.begin()), indices.end(), std::pair<Vec3<double>, double>(ref * mass, mass),
                                [&ref, &mol, box](const auto &acc, const auto idx)
                                {
                                    auto mass = AtomicMass::mass(mol.atom(idx)->speciesAtom()->Z());
                                    return std::pair<Vec3<double>, double>(
                                        acc.first + box->minimumImage(mol.atom(idx)->r(), ref) * mass, acc.second + mass);
                                });
    return sums.first / sums.second;
}

// Create stack of static, unoriented sites
bool SiteStack::createStatic()
{
    // Get origin atom indices from site
    auto originAtomIndices = speciesSite_->originAtomIndices();
    if (originAtomIndices.empty())
        return Messenger::error("No origin atoms defined in species site '{}'.\n", speciesSite_->name());

    auto *targetSpecies = speciesSite_->parent();

    // Resize the array
    auto spPop = configuration_->speciesPopulation(targetSpecies);
    if (spPop == 0)
        return true;
    sites_.reserve(spPop);

    // Get Molecule array from Configuration and search for the target Species
    const auto *box = configuration_->box();
    for (const auto &molecule : configuration_->molecules())
    {
        if (molecule->species() != targetSpecies)
            continue;

        sites_.emplace_back(molecule, speciesSite_->originMassWeighted() ? centreOfMass(*molecule, box, originAtomIndices)
                                                                         : centreOfGeometry(*molecule, box, originAtomIndices));
    }

    return true;
}

// Create stack of static, oriented sites
bool SiteStack::createStaticOriented()
{
    // Get origin atom indices from site
    auto originAtomIndices = speciesSite_->originAtomIndices();
    if (originAtomIndices.empty())
        return Messenger::error("No origin atoms defined in species site '{}'.\n", speciesSite_->name());

    // Get axis atom indices
    auto xAxisAtomIndices = speciesSite_->xAxisAtomIndices();
    if (xAxisAtomIndices.empty())
        return Messenger::error("No x-axis atoms defined in species site '{}'.\n", speciesSite_->name());
    auto yAxisAtomIndices = speciesSite_->yAxisAtomIndices();
    if (yAxisAtomIndices.empty())
        return Messenger::error("No y-axis atoms defined in species site '{}'.\n", speciesSite_->name());

    auto *targetSpecies = speciesSite_->parent();

    // Resize the array
    auto spPop = configuration_->speciesPopulation(targetSpecies);
    if (spPop == 0)
        return true;
    sites_.reserve(spPop);

    // Get Molecule array from Configuration and search for the target Species
    Vec3<double> origin, x, y, z;
    Matrix3 axes;
    const auto *box = configuration_->box();
    for (const auto &molecule : configuration_->molecules())
    {
        if (molecule->species() != targetSpecies)
            continue;

        origin = speciesSite_->originMassWeighted() ? centreOfMass(*molecule, box, originAtomIndices)
                                                    : centreOfGeometry(*molecule, box, originAtomIndices);

        // Get vector from site origin to x-axis reference point and normalise it
        x = box->minimumVector(origin, centreOfGeometry(*molecule, box, xAxisAtomIndices));
        x.normalise();

        // Get vector from site origin to y-axis reference point, normalise it, and orthogonalise
        y = box->minimumVector(origin, centreOfGeometry(*molecule, box, yAxisAtomIndices));
        y.orthogonalise(x);
        y.normalise();

        // Calculate z vector from cross product of x and y
        z = x * y;

        orientedSites_.emplace_back(molecule, origin, x, y, z);
    }

    return true;
}

// Create stack from dynamic site definition
bool SiteStack::createDynamic()
{
    // Get dynamic site data
    const auto &elements = speciesSite_->elements();
    const auto &atomTypes = speciesSite_->atomTypes();
    if (elements.empty() && atomTypes.empty())
        return Messenger::error("No elements or atom types defined for dynamic species site '{}'.\n", speciesSite_->name());

    auto *targetSpecies = speciesSite_->parent();

    // Get species population for use later once we have established number of sites per molecule
    auto spPop = configuration_->speciesPopulation(targetSpecies);
    if (spPop == 0)
        return true;

    // Determine matching atom indices for the species
    std::vector<int> siteIndices;
    for (auto &i : targetSpecies->atoms())
    {
        // Valid element or atom type?
        if ((std::find(elements.begin(), elements.end(), i.Z()) != elements.end()) ||
            std::find(atomTypes.begin(), atomTypes.end(), i.atomType()) != atomTypes.end())
            siteIndices.push_back(i.index());
    }
    if (siteIndices.empty())
        return true;

    // Resize our array
    sites_.reserve(siteIndices.size() * spPop);

    // Get Molecule array from Configuration and search for the target Species
    for (const auto &molecule : configuration_->molecules())
    {
        if (molecule->species() != targetSpecies)
            continue;

        auto &atoms = molecule->atoms();

        // Loop over site indices
        for (auto id : siteIndices)
            sites_.emplace_back(molecule, atoms[id]->r());
    }

    return true;
}

bool SiteStack::createFragment()
{
    auto *targetSpecies = speciesSite_->parent();
    const auto &fragment = speciesSite_->fragment();
    const auto *box = configuration_->box();

    auto spPop = configuration_->speciesPopulation(targetSpecies);
    if (spPop == 0)
        return true;

    // Determine matching atom indices for the species
    std::vector<int> siteIndices;
    std::vector<std::vector<int>> matchedIndices;
    for (auto &i : targetSpecies->atoms())
    {
        if (fragment.matches(&i))
        {
            // Determine the path of matched atoms - i.e. the atoms in the fragment.
            auto matchedAtoms = fragment.matchedPath(&i).set();

            // Create vector of indices of the matched atoms.
            std::vector<int> matchedAtomIndices(matchedAtoms.size());
            std::transform(matchedAtoms.begin(), matchedAtoms.end(), matchedAtomIndices.begin(),
                           [](const auto &atom) { return atom->index(); });

            // Check if the fragment we have found is unique.
            std::sort(matchedAtomIndices.begin(), matchedAtomIndices.end());
            if (std::find(matchedIndices.begin(), matchedIndices.end(), matchedAtomIndices) != matchedIndices.end())
                continue;

            // If it's unique, remember it and proceed.
            matchedIndices.push_back(std::move(matchedAtomIndices));
            siteIndices.push_back(i.index());
        }
    }

    if (siteIndices.empty())
        return true;

    // Resize our array
    sites_.reserve(siteIndices.size() * spPop);

    // Get Molecule array from Configuration and search for the target Species
    for (const auto &molecule : configuration_->molecules())
    {
        if (molecule->species() != targetSpecies)
            continue;

        auto &atoms = molecule->atoms();

        // Loop over site indices
        for (auto id : siteIndices)
        {
            // Determine orgin atoms.
            auto identifiers = fragment.matchedPath(&targetSpecies->atoms()[id]).identifiers();
            std::vector<int> originAtomIndices(identifiers["origin"].size());
            std::transform(identifiers["origin"].begin(), identifiers["origin"].end(), originAtomIndices.begin(),
                           [](const auto &at) { return at->index(); });

            sites_.emplace_back(molecule, speciesSite_->originMassWeighted()
                                              ? centreOfMass(*molecule, box, originAtomIndices)
                                              : centreOfGeometry(*molecule, box, originAtomIndices));
        }
    }

    return true;
}

bool SiteStack::createFragmentOriented()
{
    auto *targetSpecies = speciesSite_->parent();
    const auto &fragment = speciesSite_->fragment();

    auto spPop = configuration_->speciesPopulation(targetSpecies);
    if (spPop == 0)
        return true;

    // Determine matching atom indices for the species
    std::vector<int> siteIndices;
    std::vector<std::vector<int>> matchedIndices;
    for (auto &i : targetSpecies->atoms())
    {
        if (fragment.matches(&i))
        {
            // Determine the path of matched atoms - i.e. the atoms in the fragment.
            auto matchedAtoms = fragment.matchedPath(&i).set();

            // Create vector of indices of the matched atoms.
            std::vector<int> matchedAtomIndices(matchedAtoms.size());
            std::transform(matchedAtoms.begin(), matchedAtoms.end(), matchedAtomIndices.begin(),
                           [](const auto &atom) { return atom->index(); });

            // Check if the fragment we have found is unique.
            std::sort(matchedAtomIndices.begin(), matchedAtomIndices.end());
            if (std::find(matchedIndices.begin(), matchedIndices.end(), matchedAtomIndices) != matchedIndices.end())
                continue;

            // If it's unique, remember it and proceed.
            matchedIndices.push_back(std::move(matchedAtomIndices));
            siteIndices.push_back(i.index());
        }
    }

    if (siteIndices.empty())
        return true;

    // Resize our array
    orientedSites_.reserve(siteIndices.size() * spPop);

    const auto *box = configuration_->box();
    Vec3<double> origin, x, y, z;

    // Get Molecule array from Configuration and search for the target Species
    for (const auto &molecule : configuration_->molecules())
    {
        if (molecule->species() != targetSpecies)
            continue;

        auto &atoms = molecule->atoms();

        // Loop over site indices
        for (auto id : siteIndices)
        {
            auto identifiers = fragment.matchedPath(&targetSpecies->atoms()[id]).identifiers();

            // Determine origin atoms
            std::vector<int> originAtomIndices(identifiers["origin"].size());
            std::transform(identifiers["origin"].begin(), identifiers["origin"].end(), originAtomIndices.begin(),
                           [](const auto &at) { return at->index(); });

            // Determine x axis atoms.
            std::vector<int> xAxisAtomIndices(identifiers["x"].size());
            std::transform(identifiers["x"].begin(), identifiers["x"].end(), xAxisAtomIndices.begin(),
                           [](const auto &at) { return at->index(); });

            // Determine y axis atoms.
            std::vector<int> yAxisAtomIndices(identifiers["y"].size());
            std::transform(identifiers["y"].begin(), identifiers["y"].end(), yAxisAtomIndices.begin(),
                           [](const auto &at) { return at->index(); });

            origin = speciesSite_->originMassWeighted() ? centreOfMass(*molecule, box, originAtomIndices)
                                                        : centreOfGeometry(*molecule, box, originAtomIndices);

            // Get vector from site origin to x-axis reference point and normalise it
            x = box->minimumVector(origin, centreOfGeometry(*molecule, box, xAxisAtomIndices));
            x.normalise();

            // Get vector from site origin to y-axis reference point, normalise it, and orthogonalise
            y = box->minimumVector(origin, centreOfGeometry(*molecule, box, yAxisAtomIndices));
            y.orthogonalise(x);
            y.normalise();

            // Calculate z vector from cross product of x and y
            z = x * y;

            orientedSites_.emplace_back(molecule, origin, x, y, z);
        }
    }

    return true;
}

// Create stack for specified Configuration and site
bool SiteStack::create(Configuration *cfg, const SpeciesSite *site)
{
    // Are we already up-to-date?
    if (configurationIndex_ == cfg->contentsVersion())
        return true;

    // Set the defining information for the stack
    configuration_ = cfg;
    speciesSite_ = site;
    sitesInMolecules_ = true;
    sitesHaveOrientation_ = speciesSite_->hasAxes();

    // Set new index and clear old arrays
    configurationIndex_ = configuration_->contentsVersion();
    sites_.clear();
    orientedSites_.clear();

    // Create based on the type of site we were given
    switch (site->type())
    {
        case (SpeciesSite::SiteType::Static):
            return sitesHaveOrientation_ ? createStaticOriented() : createStatic();
        case (SpeciesSite::SiteType::Dynamic):
            return createDynamic();
        case (SpeciesSite::SiteType::Fragment):
            return sitesHaveOrientation_ ? createFragmentOriented() : createFragment();
        default:
            return Messenger::error("Species site type not handled in stack generation.\n");
    }
}

/*
 * Stack
 */

// Return number of sites in the stack
int SiteStack::nSites() const { return (sitesHaveOrientation_ ? orientedSites_.size() : sites_.size()); }

// Return site with index specified
const Site &SiteStack::site(int index) const { return (sitesHaveOrientation_ ? orientedSites_.at(index) : sites_.at(index)); }
