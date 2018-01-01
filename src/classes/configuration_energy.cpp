/*
	*** Configuration - Energy Functions
	*** src/classes/configuration_energy.cpp
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

#include "classes/configuration.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/grain.h"
#include "classes/species.h"
#include "base/processpool.h"

// Register a change in the total energy of the system
void Configuration::registerEnergyChange(double deltaE)
{
	energyChange_ += deltaE;
	energyChanged_ = true;
}

// Accumulate current energy change into energyData_
void Configuration::accumulateEnergyChange()
{
	// If energy hasn't changed, don't do anything
	if (!energyChanged_) return;

	// Get index of new point to add
	double x = energyData_.nPoints()+1.0;

	// Determine y value of new point...
	double y = energyData_.nPoints() == 0 ? energyChange_ : energyData_.y(energyData_.nPoints()-1) + energyChange_;

	// Add point and signal change
	energyData_.addPoint(x, y);
	energyChange_ = 0.0;
	energyChanged_ = false;
}

// Set absolute energy of system, after total energy calculation
void Configuration::setAbsoluteEnergy(double energy)
{
	// Shift existing energy data so that the last point equals the supplied energy
	if (energyData_.nPoints() == 0) return;
	
	double y = energyData_.arrayY().last();
	if (energyChanged_)
	{
		y += energyChange_;
		energyChange_ = 0.0;
	}
	
	double delta = energy - y;
	energyData_.arrayY() += delta;
}
