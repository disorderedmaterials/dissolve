/*
	*** Dissolve Main Structure
	*** src/main/dissolve.cpp
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

#include "main/dissolve.h"
#include "classes/species.h"
#include "classes/atomtype.h"
#include "classes/braggpeak.h"
#include "classes/kvector.h"
#include "classes/partialset.h"
#include "classes/weights.h"
#include "math/histogram1d.h"
#include "math/pairbroadeningfunction.h"
#include "base/genericitem.h"
#include "genericitems/genericitems.h"

// Constructor
Dissolve::Dissolve(CoreData& coreData) : coreData_(coreData)
{
	// Set Module instances list in our core data
	coreData_.setModuleInstances(&moduleInstances_);

	// Clear everything
	clear();

	// Parallel Comms
	parallelStrategy_ = Dissolve::SequentialConfigStrategy;
	parallelGroupPopulation_ = ProcessPool::MinimumGroupPopulation;

	// Register GenericItems
	registerGenericItems();
}

// Destructor
Dissolve::~Dissolve()
{
	// Clear main data
	clear();

	// Delete all master Modules
	masterModules_.clear();
}

/*
 * Core
 */

// Return reference to CoreData
const CoreData& Dissolve::coreData() const
{
	return coreData_;
}

// Clear all data
void Dissolve::clear()
{
	// Core
	setUp_ = false;

	// Atom Types
	Messenger::printVerbose("Clearing Atom Types...\n");
	atomTypes().clear();

	// Master Terms
	Messenger::printVerbose("Clearing Master Terms...\n");
	masterBonds_.clear();
	masterAngles_.clear();
	masterTorsions_.clear();

	// Species
	Messenger::printVerbose("Clearing Species...\n");
	species().clear();

	// PairPotentials
	Messenger::printVerbose("Clearing Pair Potentials...\n");
	pairPotentialDelta_ = 0.005;
	pairPotentialRange_ = 15.0;
	pairPotentialRangeSquared_ = pairPotentialRange_*pairPotentialRange_;
	pairPotentialsIncludeCoulomb_ = true;
	pairPotentials_.clear();
	potentialMap_.clear();

	// Configurations
	Messenger::printVerbose("Clearing Configurations...\n");
	configurations().clear();

	// Modules
	Messenger::printVerbose("Clearing Modules...\n");
	moduleInstances_.clear();

	// Simulation
	Messenger::printVerbose("Clearing Simulation...\n");
	nBoxNormalisationPoints_ = 500000000;
	seed_ = -1;
	restartFileFrequency_ = 10;
	processingLayers_.clear();
	processingModuleData_.clear();
	iteration_ = 0;
	nIterationsPerformed_ = 0;

	// I/O
	inputFilename_.clear();
	restartFilename_.clear();
	saveRestartTimes_ = 0.0;
}

// Register GenericItems
void Dissolve::registerGenericItems()
{
	GenericItem::addItemClass(new GenericItemContainer<bool>("bool"));
	GenericItem::addItemClass(new GenericItemContainer<int>("int"));
	GenericItem::addItemClass(new GenericItemContainer<double>("double"));
	GenericItem::addItemClass(new GenericItemContainer<CharString>("CharString"));
	GenericItem::addItemClass(new GenericItemContainer<streampos>("StreamPos"));

	GenericItem::addItemClass(new GenericItemContainer< Vec3<int> >("Vec3<int>"));
	GenericItem::addItemClass(new GenericItemContainer< Vec3<double> >("Vec3<double>"));

	GenericItem::addItemClass(new GenericItemContainer< Array2D<double> >("Array2D<double>"));
	GenericItem::addItemClass(new GenericItemContainer< Array2D< Array<double> > >("Array2D<Array<double>>"));
	GenericItem::addItemClass(new GenericItemContainer< Array2D<DummyClass> >("Array2D<DummyClass>"));

	GenericItem::addItemClass(new GenericItemContainer< Array<int> >("Array<int>"));
	GenericItem::addItemClass(new GenericItemContainer< Array<double> >("Array<double>"));
	GenericItem::addItemClass(new GenericItemContainer< Array<DummyClass> >("Array<DummyClass>"));
	GenericItem::addItemClass(new GenericItemContainer< Array< Vec3<int> > >("Array<Vec3<int>>"));
	GenericItem::addItemClass(new GenericItemContainer< Array< Vec3<double> > >("Array<Vec3<double>>"));

	GenericItem::addItemClass(new GenericItemContainer<BraggPeak>(BraggPeak::itemClassName()));
	GenericItem::addItemClass(new GenericItemContainer<Data1D>(Data1D::itemClassName()));
	GenericItem::addItemClass(new GenericItemContainer<Histogram1D>(Histogram1D::itemClassName()));
	GenericItem::addItemClass(new GenericItemContainer<IsotopologueMix>(IsotopologueMix::itemClassName()));
	GenericItem::addItemClass(new GenericItemContainer<KVector>(KVector::itemClassName()));
	GenericItem::addItemClass(new GenericItemContainer<PairBroadeningFunction>(PairBroadeningFunction::itemClassName()));
	GenericItem::addItemClass(new GenericItemContainer<PartialSet>(PartialSet::itemClassName()));
	GenericItem::addItemClass(new GenericItemContainer<Weights>(Weights::itemClassName()));
}

// Set up everything needed to run the simulation
bool Dissolve::setUp()
{
	setUp_ = false;

	// Initialise random seed
	if (seed_ == -1) srand( (unsigned)time( NULL ) );
	else srand(seed_);

	// Perform simulation set up (all processes)
	Messenger::banner("Setting Up Simulation");
	if (!setUpSimulation())
	{
		Messenger::print("Failed to set up simulation.\n");
		return false;
	}

	// Set up parallel comms / limits etc.
	Messenger::banner("Setting Up Parallelism");
	if (!setUpMPIPools())
	{
		Messenger::print("Failed to set up parallel communications.\n");
		return false;
	}

	setUp_ = true;

	return true;
}

// Flag that the set up is no longer valid and should be done again
void Dissolve::invalidateSetUp()
{
	setUp_ = false;
}

// Return whether the simulation has been set up
bool Dissolve::isSetUp() const
{
	return setUp_;
}
