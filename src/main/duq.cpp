/*
	*** dUQ Main Structure
	*** src/main/duq.cpp
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
#include "classes/species.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/molecule.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include "base/processpool.h"
#include "math/constants.h"
#include "version.h"
#include <string.h>

// Constructor
DUQ::DUQ()
{
	// PairPotentials
	pairPotentialDelta_ = 0.01;
	pairPotentialRange_ = 15.0;
	pairPotentialTruncationWidth_ = 2.0;

	// Setup
	boxNormalisationPoints_ = 50000000;
	rmseDeltaQ_ = 0.05;
	seed_ = -1;
	maxIterations_ = -1;
	windowFunction_ = Data2D::NoWindow;

	// Parallel
	parallelStrategy_ = DUQ::SequentialConfigStrategy;
}

// Destructor
DUQ::~DUQ()
{
	clear();
}

/*
 * Data
 */

// Clear all data
void DUQ::clear()
{
	configurations_.clear();
	samples_.clear();
	species_.clear();
	potentialMap_.clear();
	fileName_.clear();
}
