/*
	*** Refine Module - Functions
	*** src/modules/refine/functions.cpp
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

#include "modules/refine/refine.h"

// Calculate c(r) from supplied sq
XYData RefineModule::calculateCR(const XYData& sq, double normFactor, double rMin, double rStep, double rMax, const BroadeningFunction& broadening, bool unbroaden, XYData::WindowFunction wf)
{
	// Create working array
	XYData cr;

	// Assume deltQ is the difference between the first two points
	double deltaQ = sq.x(1) - sq.x(0);
	int nQ = sq.nPoints();

	// Perform Fourier sine transform, apply general and omega-dependent broadening, as well as window function
	double ft, windowPos, broaden;
	double omega = rMin;
	while (omega <= rMax)
	{
		ft = 0.0;
		for (int m=0; m<nQ; ++m)
		{
			// Get window value at this position in the function
			windowPos = double(m) / double(nQ-1);

			// Calculate broadening
			broaden = (unbroaden ? 1.0 / broadening.yFT(sq.x(m), omega) : broadening.yFT(sq.x(m), omega));

			ft += broaden * XYData::window(wf, windowPos) * sq.y(m) * deltaQ * ((sin(sq.x(m)*omega) * sq.x(m)) / (sq.y(m)+1.0));
		}

		// Normalise
		if (omega > 0.0) ft /= omega;
		cr.addPoint(omega, ft);
		
		omega += rStep;
	}

	// Apply normalisation factor
	cr.arrayY() *= normFactor;

	return cr;
}
