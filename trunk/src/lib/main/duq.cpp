/*
	*** dUQ Main Structure
	*** src/lib/main/duq.cpp
	Copyright T. Youngs 2012-2014

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
#include "classes/species.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/molecule.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "base/comms.h"
#include "math/constants.h"
#include "version.h"
#include <string.h>

/*!
 * \brief Constructor
 * \details Constructor for dUQ. 
 */
DUQ::DUQ()
{
	// System Composition
	multiplier_ = 1;
	density_ = 0.1;
	densityIsAtomic_ = true;
	boxAngles_.set(90.0, 90.0, 90.0);
	relativeBoxLengths_.set(1.0, 1.0, 1.0);
	nonPeriodic_ = false;
	randomConfiguration_ = true;

	// PairPotentials
	pairPotentialDelta_ = 0.01;
	pairPotentialRange_ = 15.0;
	pairPotentialTruncationWidth_ = 2.0;

	// Partials
	partialsChangeCount_ = -1;
	rdfMethod_ = DUQ::SimpleMethod;

	// Setup
	boxNormalisationPoints_ = 50000000;
	qDependentFWHM_ = 0.0;
	qIndependentFWHM_ = 0.02;
	rdfBinWidth_ = 0.025;
	rdfRange_ = -1.0;
	rdfSmoothing_ = 0;
	requestedRDFRange_ = -1.0;
	qDelta_ = 0.05;
	qMax_ = 50.0;
	rmseDeltaQ_ = 0.05;
	seed_ = -1;
	temperature_ = 300.0;
	windowFunction_ = Data2D::GaussianWindow;
	braggCalculationOn_ = false;
	braggMaximumQ_ = 1.0;
	braggBroadening_ = 0.0063;

	// Perturbation
	simplexNMoves_ = 25;
	simplexNCycles_ = 100;
	simplexTemperature_ = 0.01;
	simplexTolerance_ = 1.0e-3;
	simplexParameterOffset_ = 100.0;
	
	// Simulation
	energyChange_ = 0.0;
	energyChanged_ = false;
}

/*!
 * \brief Destructor
 * \details Destructor for dUQ. 
 */
DUQ::~DUQ()
{
	clear();
}

/*
 * Data
 */

/*!
 * \brief Clear all data
 */
void DUQ::clear()
{
	species_.clear();
	clearModel();
	fileName_.clear();
}
