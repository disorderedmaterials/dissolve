// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "classes/sitestack.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/speciesatom.h"
#include "classes/speciessite.h"
#include "data/atomicmasses.h"

SiteStack::SiteStack() : ListItem<SiteStack>()
{
    configuration_ = nullptr;
    configurationIndex_ = -1;
    speciesSite_ = nullptr;
    sitesInMolecules_ = false;
    sitesHaveOrientation_ = false;
}

SiteStack::~SiteStack() {}

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
    if (originAtomIndices.nItems() == 0)
        return Messenger::error("No origin atoms defined in SpeciesSite '{}'.\n", speciesSite->name());
    const auto *box = configuration_->box();

    // If the site has axes, grab the atom indices involved
    Array<int> xAxisAtomIndices, yAxisAtomIndices;
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
    Vec3<double> origin, v, x, y, z;
    Matrix3 axes;
    for (const auto &molecule : molecules)
    {
        if (molecule->species() != targetSpecies)
            continue;

        // Calculate origin
        if (speciesSite->originMassWeighted())
        {
            double mass = AtomicMass::mass(molecule->atom(originAtomIndices.firstValue())->speciesAtom()->Z());
            origin = molecule->atom(originAtomIndices.firstValue())->r() * mass;
            double massNorm = mass;
            for (auto m = 1; m < originAtomIndices.nItems(); ++m)
            {
                mass = AtomicMass::mass(molecule->atom(originAtomIndices[m])->speciesAtom()->Z());
                origin += box->minimumImage(molecule->atom(originAtomIndices[m])->r(),
                                            molecule->atom(originAtomIndices.firstValue())->r()) *
                          mass;
                massNorm += mass;
            }
            origin /= massNorm;
        }
        else
        {
            origin = molecule->atom(originAtomIndices.firstValue())->r();
            for (auto m = 1; m < originAtomIndices.nItems(); ++m)
                origin += box->minimumImage(molecule->atom(originAtomIndices[m])->r(),
                                            molecule->atom(originAtomIndices.firstValue())->r());
            origin /= originAtomIndices.nItems();
        }

        // Calculate axes and store data
        if (sitesHaveOrientation_)
        {
            // Get average position of supplied x-axis atoms
            v = molecule->atom(xAxisAtomIndices.firstValue())->r();
            for (auto m = 1; m < xAxisAtomIndices.nItems(); ++m)
                v += box->minimumImage(molecule->atom(xAxisAtomIndices[m])->r(),
                                       molecule->atom(xAxisAtomIndices.firstValue())->r());
            v /= xAxisAtomIndices.nItems();

            // Get vector from site origin and normalise it
            x = box->minimumVector(origin, v);
            x.normalise();

            // Get average position of supplied y-axis atoms
            v = molecule->atom(yAxisAtomIndices.firstValue())->r();
            for (auto m = 1; m < yAxisAtomIndices.nItems(); ++m)
                v += box->minimumImage(molecule->atom(yAxisAtomIndices[m])->r(),
                                       molecule->atom(yAxisAtomIndices.firstValue())->r());
            v /= yAxisAtomIndices.nItems();

            // Get vector from site origin, normalise it, and orthogonalise
            y = box->minimumVector(origin, v);
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
