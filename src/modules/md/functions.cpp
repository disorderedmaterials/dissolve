/*
	*** MD Module - Functions
	*** src/modules/md/functions.cpp
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

#include "modules/md/md.h"
#include "classes/configuration.h"

// Cap forces in Configuration
int MDModule::capForces(Configuration* cfg, double maxForce, Array<double>& fx, Array<double>& fy, Array<double>& fz)
{
	double fMag;
	const double maxForceSq = maxForce * maxForce;
	int nCapped = 0;
	for (int n=0; n<cfg->nAtoms(); ++n)
	{
		fMag = fx[n]*fx[n] + fy[n]*fy[n] + fz[n]*fz[n];
		if (fMag < maxForceSq) continue;

		fMag = maxForce / sqrt(fMag);
		fx[n] *= fMag;
		fy[n] *= fMag;
		fz[n] *= fMag;

		++nCapped;
	}

	return nCapped;
}

// Determine timestep based on maximal force component
double MDModule::determineTimeStep(const Array<double>& fx, const Array<double>& fy, const Array<double>& fz)
{
	double fMax = fx.maxAbs();
	double fTemp = fy.maxAbs();
	if (fTemp > fMax) fMax = fTemp;
	fTemp = fz.maxAbs();
	if (fTemp > fMax) fMax = fTemp;

	return 1.0 / fMax;
}
