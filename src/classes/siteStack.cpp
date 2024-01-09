// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#include "classes/siteStack.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/speciesSite.h"
#include "data/atomicMasses.h"
#include <algorithm>
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

    auto originAtomsIndices = site->sitesOriginAtomsIndices();
    auto xAxisAtomsIndices = site->sitesXAxisAtomsIndices();
    auto yAxisAtomsIndices = site->sitesYAxisAtomsIndices();

    auto *targetSpecies = site->parent();

    auto sPop = configuration_->speciesPopulation(targetSpecies);
    if (sPop == 0)
        return true;

    if (sitesHaveOrientation_)
        orientedSites_.reserve(site->nSites() * sPop);
    else
        sites_.reserve(site->nSites() * sPop);

    Vec3<double> origin, x, y;
    const auto *box = configuration_->box();

    for (const auto &molecule : configuration_->molecules())
    {
        if (molecule->species() != targetSpecies)
            continue;

        for (auto i = 0; i < site->nSites(); ++i)
        {
            origin = speciesSite_->originMassWeighted() ? centreOfMass(*molecule, box, originAtomsIndices.at(i))
                                                        : centreOfGeometry(*molecule, box, originAtomsIndices.at(i));
            if (sitesHaveOrientation_)
            {
                // Get vector from site origin to x-axis reference point and normalise it
                x = box->minimumVector(origin, centreOfGeometry(*molecule, box, xAxisAtomsIndices.at(i)));
                x.normalise();

                // Get vector from site origin to y-axis reference point, normalise it, and orthogonalise
                y = box->minimumVector(origin, centreOfGeometry(*molecule, box, yAxisAtomsIndices.at(i)));
                y.orthogonalise(x);
                y.normalise();

                orientedSites_.emplace_back(speciesSite_, i, molecule, origin, x, y, x * y);
            }
            else
                sites_.emplace_back(speciesSite_, i, molecule, origin);
        }
    }
    return true;
}

/*
 * Stack
 */

// Return number of sites in the stack
int SiteStack::nSites() const { return (sitesHaveOrientation_ ? orientedSites_.size() : sites_.size()); }

// Return site with index specified
const Site &SiteStack::site(int index) const { return (sitesHaveOrientation_ ? orientedSites_.at(index) : sites_.at(index)); }
