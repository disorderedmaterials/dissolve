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
// #include "classes/configuration.h"
// #include "classes/atom.h"
// #include "classes/box.h"
// #include "classes/braggpeak.h"
// #include "classes/cell.h"
// #include "classes/kvector.h"
// #include "classes/species.h"
// #include "classes/weights.h"
// #include "math/broadeningfunction.h"
// #include "templates/array3d.h"
// #include "templates/genericlisthelper.h"

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
