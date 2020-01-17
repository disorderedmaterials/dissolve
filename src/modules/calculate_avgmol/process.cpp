/*
	*** Calculate Average Molecule Module - Processing
	*** src/modules/calculate_avgmol/process.cpp
	Copyright T. Youngs 2012-2020

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

#include "modules/calculate_avgmol/avgmol.h"
#include "main/dissolve.h"
#include "classes/box.h"
#include "genericitems/listhelper.h"
#include "base/sysfunc.h"

// Run set-up stage
bool CalculateAvgMolModule::setUp(Dissolve& dissolve, ProcessPool& procPool)
{
	SpeciesSite* site = keywords_.retrieve<SpeciesSite*>("Site");

	// Clear species
	averageSpecies_.clear();

	// If the targetSpecies_ is different from the current target site, or the site is NULL, clear the arrays
	if (!site) targetSpecies_ = NULL;
	else
	{
		if (site->parent() == NULL)
		{
			targetSpecies_ = NULL;

			return Messenger::error("Target site has no parent species.\n");
		}
		else if (site->parent() != targetSpecies_)
		{
			targetSpecies_ = site->parent();

			// Copy basic atom and bond information from species
			ListIterator<SpeciesAtom> atomIterator(targetSpecies_->atoms());
			while (SpeciesAtom* i = atomIterator.iterate()) averageSpecies_.addAtom(i->element(), i->r());
			DynamicArrayIterator<SpeciesBond> bondIterator(targetSpecies_->bonds());
			while (SpeciesBond* b = bondIterator.iterate()) averageSpecies_.addBond(b->indexI(), b->indexJ());
		}
	}

	// Set name and object tag for average species
	averageSpecies_.setName(CharString("%s@%s", site ? site->name() : "???", targetSpecies_ ? targetSpecies_->name() : "???"));
	averageSpecies_.setObjectTag(CharString("CalculateAvgMol_%s", averageSpecies_.name()));

	// Realise arrays
	updateArrays(dissolve);

	// Retrieve data arrays
	Array<SampledDouble>& x = GenericListHelper< Array<SampledDouble> >::retrieve(dissolve.processingModuleData(), "X", uniqueName());
	Array<SampledDouble>& y = GenericListHelper< Array<SampledDouble> >::retrieve(dissolve.processingModuleData(), "Y", uniqueName());
	Array<SampledDouble>& z = GenericListHelper< Array<SampledDouble> >::retrieve(dissolve.processingModuleData(), "Z", uniqueName());

	// Update our Species
	updateSpecies(x, y, z);

	return true;
}

// Run main processing
bool CalculateAvgMolModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	// Check for zero Configuration targets
	if (targetConfigurations_.nItems() == 0) return Messenger::error("No configuration targets set for module '%s'.\n", uniqueName());

	// Grab Configuration and Box pointers
	Configuration* cfg = targetConfigurations_.firstItem();
	const Box* box = cfg->box();

	// Set up process pool - must do this to ensure we are using all available processes
	procPool.assignProcessesToGroups(cfg->processPool());

	// Get the target site
	SpeciesSite* site = keywords_.retrieve<SpeciesSite*>("Site");
	if (!site) return Messenger::error("No target site defined.\n");

	// Get site parent species
	Species* sp = site->parent();
	if (sp != targetSpecies_) return Messenger::error("Internal error - target site parent is not the same as the target species.\n");

	// Update arrays
	updateArrays(dissolve);

	// Get the site stack
	const SiteStack* stack = cfg->siteStack(site);

	// Retrieve data arrays
	Array<SampledDouble>& x = GenericListHelper< Array<SampledDouble> >::retrieve(dissolve.processingModuleData(), "X", uniqueName());
	Array<SampledDouble>& y = GenericListHelper< Array<SampledDouble> >::retrieve(dissolve.processingModuleData(), "Y", uniqueName());
	Array<SampledDouble>& z = GenericListHelper< Array<SampledDouble> >::retrieve(dissolve.processingModuleData(), "Z", uniqueName());

	// Loop over sites
	Vec3<double> r;
	for (int n=0; n<stack->nSites(); ++n)
	{
		const Site& s = stack->site(n);
#ifdef CHECKS
		if (s.molecule()->species() != targetSpecies_)
		{
			Messenger::error("Site species doesn't match target species.\n");
			continue;
		}
#endif

		// Get axes and take inverse
		Matrix3 inverseAxes = s.axes();
		inverseAxes.invert();

		// Loop over atoms, taking delta position with origin, and rotating into local axes
		for (int i=0; i<s.molecule()->nAtoms(); ++i)
		{
			r = inverseAxes * box->minimumVector(s.origin(), s.molecule()->atom(i)->r());

			x.at(i) += r.x;
			y.at(i) += r.y;
			z.at(i) += r.z;
		}
	}

	updateSpecies(x, y, z);

	return true;
}
