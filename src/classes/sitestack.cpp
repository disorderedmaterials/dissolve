/*
	*** Site Stack
	*** src/classes/sitestack.cpp
	Copyright T. Youngs 2012-2018

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

#include "classes/sitestack.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/speciessite.h"
#include "data/atomicmass.h"
#include "base/processpool.h"

// Constructor
SiteStack::SiteStack() : ListItem<SiteStack>()
{
	configuration_ = NULL;
	configurationIndex_ = -1;
	speciesSite_ = NULL;
	sitesInMolecules_ = false;
	sitesHaveOrientation_ = false;
}

// Destructor
SiteStack::~SiteStack()
{
}

/*
 * Target
 */

// Create stack for specified Configuration and site
bool SiteStack::create(Configuration* cfg, SpeciesSite* speciesSite)
{
	// Are we already up-to-date?
	if (configurationIndex_ == cfg->coordinateIndex()) return true;

	// Set the defining information for the stack
	configuration_ = cfg;
	speciesSite_ = speciesSite;
	sitesInMolecules_ = true;
	sitesHaveOrientation_ = speciesSite->hasAxes();

	// Get origin atom indices from site, and grab the Configuration's Box
	Array<int> originAtomIndices = speciesSite->originAtomIndices();
	if (originAtomIndices.nItems() == 0) return Messenger::error("No origin atoms defined in SpeciesSite '%s'.\n", speciesSite->name());
	const Box* box = configuration_->box();

	// If the site has axes, grab the atom indices involved
	Array<int> xAxisAtomIndices, yAxisAtomIndices;
	if (sitesHaveOrientation_)
	{
		xAxisAtomIndices = speciesSite->xAxisAtomIndices();
		yAxisAtomIndices = speciesSite->yAxisAtomIndices();
	}

	// Set new index and clear old arrays
	configurationIndex_ = cfg->coordinateIndex();
	sites_.clear();
	orientedSites_.clear();

	// Get Molecule array from Configuration and search for the target Species
	DynamicArray<Molecule>& molecules = cfg->molecules();
	Species* targetSpecies = speciesSite->parent();
	Vec3<double> origin, v, x, y, z;
	Matrix3 axes;
	for (int n=0; n<molecules.nItems(); ++n)
	{
		Molecule* molecule = molecules[n];
		if (molecule->species() != targetSpecies) continue;

		// Calculate origin
#ifdef CHECKS
		for (int i = 0; i < originAtomIndices.nItems(); ++i) if ((originAtomIndices[i] < 0) || (originAtomIndices[i] >= molecule->nAtoms())) return Messenger::error("Origin atom index %i is out of range for Molecule %i which contains %i atoms.\n", originAtomIndices[i], n, molecule->nAtoms());
#endif
		if (speciesSite->originMassWeighted())
		{
			double mass = AtomicMass::mass(molecule->atom(originAtomIndices.firstValue())->element());
			origin = molecule->atom(originAtomIndices.firstValue())->r() * mass;
			double massNorm = mass;
			for (int m=1; m<originAtomIndices.nItems(); ++m)
			{
				mass = AtomicMass::mass(molecule->atom(originAtomIndices[m])->element());
				origin += box->minimumImage(molecule->atom(originAtomIndices[m])->r(), molecule->atom(originAtomIndices.firstValue())->r()) * mass;
				massNorm += mass;
			}
			origin /= massNorm;
		}
		else
		{
			origin = molecule->atom(originAtomIndices.firstValue())->r();
			for (int m=1; m<originAtomIndices.nItems(); ++m) origin += box->minimumImage(molecule->atom(originAtomIndices[m])->r(), molecule->atom(originAtomIndices.firstValue())->r());
			origin /= originAtomIndices.nItems();
		}

		// Calculate axes and store data
		if (sitesHaveOrientation_)
		{
#ifdef CHECKS
			for (int i = 0; i < xAxisAtomIndices.nItems(); ++i) if ((xAxisAtomIndices[i] < 0) || (xAxisAtomIndices[i] >= molecule->nAtoms())) return Messenger::error("X-axis atom index %i is out of range for Molecule %i which contains %i atoms.\n", xAxisAtomIndices[i], n, molecule->nAtoms());
			for (int i = 0; i < yAxisAtomIndices.nItems(); ++i) if ((yAxisAtomIndices[i] < 0) || (yAxisAtomIndices[i] >= molecule->nAtoms())) return Messenger::error("Y-axis atom index %i is out of range for Molecule %i which contains %i atoms.\n", yAxisAtomIndices[i], n, molecule->nAtoms());
#endif
			// Get average position of supplied x-axis atoms
			v = molecule->atom(xAxisAtomIndices.firstValue())->r();
			for (int m=1; m<xAxisAtomIndices.nItems(); ++m) v += box->minimumImage(molecule->atom(xAxisAtomIndices[m])->r(), molecule->atom(xAxisAtomIndices.firstValue())->r());
			v /= xAxisAtomIndices.nItems();

			// Get vector from site origin and normalise it
			x = box->minimumVector(origin, v);
			x.normalise();

			// Get average position of supplied y-axis atoms
			v = molecule->atom(yAxisAtomIndices.firstValue())->r();
			for (int m=1; m<yAxisAtomIndices.nItems(); ++m) v += box->minimumImage(molecule->atom(yAxisAtomIndices[m])->r(), molecule->atom(yAxisAtomIndices.firstValue())->r());
			v /= yAxisAtomIndices.nItems();

			// Get vector from site origin, normalise it, and orthogonalise
			y = box->minimumVector(origin, v);
			y.normalise();
			y.orthogonalise(x);

			// Calculate z vector from cross product of x and y
			z = x * y;

			// Store data
			orientedSites_.add(OrientedSite(molecule, origin, x, y, z));
		}
		else sites_.add(Site(molecule, origin));
	}

	return true;
}

// Return target Configuration
Configuration* SiteStack::configuration() const
{
	return configuration_;
}

// Return target SpeciesSite
SpeciesSite* SiteStack::speciesSite() const
{
	return speciesSite_;
}

/*
 * Stack
 */

// Return number of sites in the stack
int SiteStack::nSites() const
{
	return (sitesHaveOrientation_ ? orientedSites_.nItems() : sites_.nItems());
}

// Return whether the stack contains associate Molecule information
bool SiteStack::sitesInMolecules() const
{
	return sitesInMolecules_;
}

// Return whether the current stack contains local axes information
bool SiteStack::sitesHaveOrientation() const
{
	return sitesHaveOrientation_;
}

// Return site with index specified
const Site& SiteStack::site(int index) const
{
	return (sitesHaveOrientation_ ? orientedSites_.constAt(index) : sites_.constAt(index));
}
