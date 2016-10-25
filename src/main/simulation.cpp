/*
	*** dUQ - Simulation
	*** src/main/simulation.cpp
	Copyright T. Youngs 2012-2016

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

#include "main/duq.h"
#include "base/comms.h"

// Run Simulation
bool DUQ::go()
{
	/*
	 * This is the main program loop. We perform the following steps:
	 *
	 *  1)	For all configurations, run all Evolution modules
	 *  2)	For all configurations, run all Analysis modules
	 *  3)	RDF calculation?
	 */

	// TEST - Calculate energy of current system, partial data, write, and quit
	for (Configuration* cfg = configurations_.first(); cfg != NULL; cfg = cfg->next)
	{
		totalEnergyTest(*cfg);
		totalEnergy(*cfg);
		intramolecularEnergy(*cfg);
	}
	return true;
}
