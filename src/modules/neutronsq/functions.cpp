/*
	*** Neutron SQ Module - Functions
	*** src/modules/neutronsq/functions.cpp
	Copyright T. Youngs 2012-2018

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "modules/neutronsq/neutronsq.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/speciesinfo.h"

// Calculate weighted S(Q) from supplied unweighted S(Q)
bool NeutronSQModule::calculateWeightedSQ(PartialSet& unweightedsq, PartialSet& weightedsq, Weights& weights, NeutronSQModule::NormalisationType normalisation)
{
	int typeI, typeJ;
	for (typeI=0; typeI<unweightedsq.nAtomTypes(); ++typeI)
	{
		for (typeJ=typeI; typeJ<unweightedsq.nAtomTypes(); ++typeJ)
		{
			// Weight S(Q), Bragg S(Q) and full partial S(Q)
			double factor = weights.fullWeight(typeI, typeJ);

			weightedsq.partial(typeI, typeJ).copyData(unweightedsq.partial(typeI, typeJ));
			weightedsq.partial(typeI, typeJ).arrayY() *= factor;
			weightedsq.boundPartial(typeI, typeJ).copyData(unweightedsq.boundPartial(typeI, typeJ));
			weightedsq.boundPartial(typeI, typeJ).arrayY() *= factor;
			weightedsq.unboundPartial(typeI, typeJ).copyData(unweightedsq.unboundPartial(typeI, typeJ));
			weightedsq.unboundPartial(typeI, typeJ).arrayY() *= factor;
			weightedsq.braggPartial(typeI, typeJ).copyData(unweightedsq.braggPartial(typeI, typeJ));
			weightedsq.braggPartial(typeI, typeJ).arrayY() *= factor;
		}
	}

	// Calculate and normalise total to form factor if requested
	weightedsq.formTotal(false);
	if (normalisation == NeutronSQModule::AverageOfSquaresNormalisation) weightedsq.total().arrayY() /= weights.boundCoherentAverageOfSquares();
	else if (normalisation == NeutronSQModule::SquareOfAverageNormalisation) weightedsq.total().arrayY() /= weights.boundCoherentSquareOfAverage();

	return true;
}

// Calculate Weights matrix summed over target Configurations
bool NeutronSQModule::calculateSummedWeights(Weights& summedWeights) const
{
	summedWeights.clear();

	// Loop over Configurations
	RefListIterator<Configuration,bool> configIterator(targetConfigurations_);
	while (Configuration* cfg = configIterator.iterate())
	{
		// Loop over Species used in this Configuration and find its entry in the defined Isotopologues for the Module
		ListIterator<SpeciesInfo> speciesInfoIterator(cfg->usedSpecies());
		while (SpeciesInfo* spInfo = speciesInfoIterator.iterate())
		{
			// Find any relevant entries in the Isotopologues list
			int nAdded = 0;
			ListIterator<IsotopologueReference> refIterator(isotopologues_);
			while (IsotopologueReference* ref = refIterator.iterate())
			{
				// If this reference does not target the Configuration / Species we are considering, continue the loop
				if ((ref->configuration() != cfg) || (ref->species() != spInfo->species())) continue;

				// Add the isotopologue, in the isotopic proportions defined in the Isotopologue, to the weights.
				int speciesPopulation = spInfo->population() * cfg->multiplier();
				summedWeights.addIsotopologue(ref->species(), speciesPopulation, ref->isotopologue(), ref->weight());

				++nAdded;
			}

			// We will complain strongly if a species in the Configuration is not covered by at least one Isotopologue definition
			if (nAdded == 0)
			{
				Messenger::error("Isotopologue specification for Species '%s' in Configuration '%s' is missing, so can't generate summed Weights.\n", spInfo->species()->name(), cfg->name());
				summedWeights.clear();
				return false;
			}
		}
	}

	// Finalise the Weights
	summedWeights.createFromIsotopologues();

	return true;
}
