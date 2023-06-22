// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2023 Team Dissolve and contributors

#include "classes/sitestack.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/speciessite.h"
#include "data/atomicmasses.h"
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

    auto originAtoms = site->sitesOriginAtoms();
    auto xAxisAtoms = site->sitesXAxisAtoms();
    auto yAxisAtoms = site->sitesYAxisAtoms();
    
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

        for (int i = 0; i < site->nSites(); ++i)
        {
            std::vector<int> originAtomIndices(originAtoms.at(i).size());
            std::transform(originAtoms.at(i).begin(), originAtoms.at(i).end(), originAtomIndices.begin(),
                           [](const auto &atom) { return atom->index(); });

            origin = speciesSite_->originMassWeighted() ? centreOfMass(*molecule, box, originAtomIndices)
                                                        : centreOfGeometry(*molecule, box, originAtomIndices);

            if (sitesHaveOrientation_)
            {
                std::vector<int> staticXAxisAtomIndices(xAxisAtoms.at(i).size());
                std::transform(xAxisAtoms.at(i).begin(), xAxisAtoms.at(i).end(), staticXAxisAtomIndices.begin(),
                               [](const auto &atom) { return atom->index(); });

                // Get vector from site origin to x-axis reference point and normalise it
                x = box->minimumVector(origin, centreOfGeometry(*molecule, box, staticXAxisAtomIndices));
                x.normalise();

                std::vector<int> staticYAxisAtomIndices(yAxisAtoms.at(i).size());
                std::transform(yAxisAtoms.at(i).begin(), yAxisAtoms.at(i).end(), staticYAxisAtomIndices.begin(),
                               [](const auto &atom) { return atom->index(); });

                // Get vector from site origin to y-axis reference point, normalise it, and orthogonalise
                y = box->minimumVector(origin, centreOfGeometry(*molecule, box, staticYAxisAtomIndices));
                y.orthogonalise(x);
                y.normalise();


                orientedSites_.emplace_back(molecule, origin, x, y, x * y);
            }
            else
                sites_.emplace_back(molecule, origin);
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
