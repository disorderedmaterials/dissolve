/*
	*** Calculate Distance-Angle Module - Functions
	*** src/modules/calculate/dangle/functions.cpp
	Copyright T. Youngs 2012-2019

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

#include "modules/calculate/dangle/dangle.h"
#include "procedure/nodes/process1d.h"
#include "procedure/nodes/process2d.h"

// Update names of processed quantities
void CalculateDAngleModule::updateNodeNames()
{
	if (processDistance_) processDistance_->setName(CharString("%s_RDF(BC)", uniqueName()));
	if (processAngle_) processAngle_->setName(CharString("%s_ANGLE(ABC)", uniqueName()));
	if (processDAngle_) processDAngle_->setName(uniqueName());
}
