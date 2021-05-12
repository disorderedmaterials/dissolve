// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/sitestack.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/speciesatom.h"
#include "classes/speciessite.h"
#include "data/atomicmasses.h"
#include <numeric>

SiteStack::SiteStack() : ListItem<SiteStack>()
{
    configuration_ = nullptr;
    configurationIndex_ = -1;
    speciesSite_ = nullptr;
    sitesInMolecules_ = false;
    sitesHaveOrientation_ = false;
}

// Calculate geometric centre of atoms in the given molecule
Vec3<double> SiteStack::centreOfGeometry(const Molecule &mol, const Box *box, const std::vector<int> &indices)
{
    const auto ref = mol.atom(indices.front())->r();
    return std::accumulate(std::next(indices.begin()), indices.end(), ref,
                           [&ref, &mol, box](const auto &acc, const auto idx) {
                               return acc + box->minimumImage(mol.atom(idx)->r(), ref);
                           }) /
           indices.size();
}

// Calculate (mass-weighted) coordinate centre of atoms in the given molecule
Vec3<double> SiteStack::centreOfMass(const Molecule &mol, const Box *box, const std::vector<int> &indices)
{
    auto mass = AtomicMass::mass(mol.atom(indices.front())->speciesAtom()->Z());
    const auto ref = mol.atom(indices.front())->r();
    auto sums = std::accumulate(std::next(indices.begin()), indices.end(), std::pair<Vec3<double>, double>(ref * mass, mass),
                                [&ref, &mol, box](const auto &acc, const auto idx) {
                                    auto mass = AtomicMass::mass(mol.atom(idx)->speciesAtom()->Z());
                                    return std::pair<Vec3<double>, double>(
                                        acc.first + box->minimumImage(mol.atom(idx)->r(), ref) * mass, acc.second + mass);
                                });
    return sums.first / sums.second;
}

/*
 * Target
 */

// Create stack for specified Configuration and site
bool SiteStack::create(Configuration *cfg, SpeciesSite *speciesSite)
{
    // Are we already up-to-date?
    if (configurationIndex_ == cfg->contentsVersion())
        return true;

    // Set the defining information for the stack
    configuration_ = cfg;
    speciesSite_ = speciesSite;
    sitesInMolecules_ = true;
    sitesHaveOrientation_ = speciesSite->hasAxes();

    // Get origin atom indices from site, and grab the Configuration's Box
    auto originAtomIndices = speciesSite->originAtomIndices();
    if (originAtomIndices.empty())
        return Messenger::error("No origin atoms defined in SpeciesSite '{}'.\n", speciesSite->name());
    const auto *box = configuration_->box();

    // If the site has axes, grab the atom indices involved
    std::vector<int> xAxisAtomIndices, yAxisAtomIndices;
    if (sitesHaveOrientation_)
    {
        xAxisAtomIndices = speciesSite->xAxisAtomIndices();
        yAxisAtomIndices = speciesSite->yAxisAtomIndices();
    }

    // Set new index and clear old arrays
    configurationIndex_ = cfg->contentsVersion();
    sites_.clear();
    orientedSites_.clear();

    // Get Molecule array from Configuration and search for the target Species
    std::deque<std::shared_ptr<Molecule>> &molecules = cfg->molecules();
    auto *targetSpecies = speciesSite->parent();
    Vec3<double> origin, x, y, z;
    Matrix3 axes;
    for (const auto &molecule : molecules)
    {
        if (molecule->species() != targetSpecies)
            continue;

        // Calculate origin
        if (speciesSite->originMassWeighted())
            origin = centreOfMass(*molecule, box, originAtomIndices);
        else
            origin = centreOfGeometry(*molecule, box, originAtomIndices);

        // Calculate axes and store data
        if (sitesHaveOrientation_)
        {
            // Get vector from site origin to x-axis reference point and normalise it
            x = box->minimumVector(origin, centreOfGeometry(*molecule, box, xAxisAtomIndices));
            x.normalise();

            // Get vector from site origin to y-axis reference point, normalise it, and orthogonalise
            y = box->minimumVector(origin, centreOfGeometry(*molecule, box, yAxisAtomIndices));
            y.orthogonalise(x);
            y.normalise();

            // Calculate z vector from cross product of x and y
            z = x * y;

            // Store data
            orientedSites_.add(OrientedSite(molecule, origin, x, y, z));
        }
        else
            sites_.add(Site(molecule, origin));
    }

    return true;
}

// Return target Configuration
Configuration *SiteStack::configuration() const { return configuration_; }

// Return target SpeciesSite
SpeciesSite *SiteStack::speciesSite() const { return speciesSite_; }

/*
 * Stack
 */

// Return number of sites in the stack
int SiteStack::nSites() const { return (sitesHaveOrientation_ ? orientedSites_.nItems() : sites_.nItems()); }

// Return whether the stack contains associate Molecule information
bool SiteStack::sitesInMolecules() const { return sitesInMolecules_; }

// Return whether the current stack contains local axes information
bool SiteStack::sitesHaveOrientation() const { return sitesHaveOrientation_; }

// Return site with index specified
const Site &SiteStack::site(int index) const { return (sitesHaveOrientation_ ? orientedSites_.at(index) : sites_.at(index)); }
