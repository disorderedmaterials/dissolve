/*
	*** dUQ Main Structure
	*** src/main/duq.cpp
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

#include "main/duq.h"
#include "classes/species.h"
#include "classes/atomtype.h"

// List<T> Master Instance
template<class T> List<T>* List<T>::masterInstance_ = NULL;

// Constructor
DUQ::DUQ()
{
	// PairPotentials
	pairPotentialDelta_ = 0.005;
	pairPotentialRange_ = 15.0;
	pairPotentialTruncationWidth_ = 2.0;
	pairPotentialsIncludeCoulomb_ = true;

	// Set up
	nBoxNormalisationPoints_ = 500000000;
	seed_ = -1;
	maxIterations_ = -1;
	windowFunction_ = XYData::NoWindow;
	allIntra_ = false;

	// Simulation
	iteration_ = 0;
	writeIterationData_ = true;

	// Parallel Comms
	parallelStrategy_ = DUQ::SequentialConfigStrategy;

	// I/O
	autoAddDependentModules_ = false;

	// Master Instances
	List<AtomType>::setMasterInstance(&atomTypes_);
	List<Species>::setMasterInstance(&species_);
}

// Destructor
DUQ::~DUQ()
{
	clear();
}

// Clear all data
void DUQ::clear()
{
	Messenger::printVerbose("Clearing Configurations...\n");
	configurations_.clear();

	Messenger::printVerbose("Clearing Species...\n");
	species_.clear();

	Messenger::printVerbose("Clearing PotentialMap...\n");
	potentialMap_.clear();

	Messenger::printVerbose("Clearing AtomTypes...\n");
	atomTypes_.clear();

	Messenger::printVerbose("Clearing misc...\n");
	filename_.clear();
}

