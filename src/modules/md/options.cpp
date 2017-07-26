/*
	*** MD Module - Options
	*** src/modules/md/options.cpp
	Copyright T. Youngs 2012-2017

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

#include "modules/md/md.h"

// Setup options for module
void MDModule::setupOptions()
{
	options_.add("CapForces", 1.0e7, "Set cap on allowable force (kJ/mol) per atom per axis (or -ve to inhibit)", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("CutoffDistance", -1.0, "Interatomic cutoff distance to employ", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("DeltaT", 1.0e-4, "Timestep (ps) to use in MD simulation", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("EnergyFrequency", 10, "Frequency at which to calculate total system energy (or 0 to inhibit)", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("NSteps", 100, "Number of MD steps to perform", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("OutputFrequency", 5, "Frequency at which to output step information (or 0 to inhibit)", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("RandomVelocities", bool(false), "Whether random velocities should always be assigned before beginning MD simulation", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
	options_.add("TrajectoryFrequency", 0, "Write frequency for trajectory file (or 0 to inhibit)", GenericItem::ModuleOptionFlag+GenericItem::NoOutputFlag);
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int MDModule::parseKeyword(LineParser& parser, DUQ* duq, GenericList& targetList)
{
	return -1;
}
