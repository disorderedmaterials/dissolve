/*
	*** Export Module - Initialisation
	*** src/modules/export/init.cpp
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

#include "modules/export/export.h"
#include "keywords/types.h"

// Perform any necessary initialisation for the Module
void ExportModule::initialise()
{
	// Export
	keywords_.add("Export", new FileAndFormatKeyword(coordinatesFormat_, "EndWriteCoordinates"), "WriteCoordinates", "Write coordinates for the Configuration targets");
	keywords_.add("Export", new FileAndFormatKeyword(pairPotentialFormat_, "EndWritePairPotentials"), "WritePairPotentials", "Write all pair potentials to supplied basename");
	keywords_.add("Export", new FileAndFormatKeyword(trajectoryFormat_, "EndWriteTrajectory"), "WriteTrajectory", "Write coordinate trajectories for the Configuration targets");
}

