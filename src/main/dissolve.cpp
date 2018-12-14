/*
	*** Dissolve Main Structure
	*** src/main/dissolve.cpp
	Copyright T. Youngs 2012-2018

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
#include "module/registry.h"
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

// List<T> Master Instances
template <class T> List<T> List<T>::masterInstance_;

// Constructor
Dissolve::Dissolve() : atomTypes_(List<AtomType>::master()), species_(List<Species>::master()), configurations_(List<Configuration>::master())
{
	clear();

	// Parallel Comms
	parallelStrategy_ = Dissolve::SequentialConfigStrategy;

	// Register GenericItems
	registerGenericItems();
}

// Destructor
Dissolve::~Dissolve()
{
	clear();
}

/*
 * Core
 */

// Clear all data
void Dissolve::clear()
{
	// PairPotentials
	pairPotentialDelta_ = 0.005;
	pairPotentialRange_ = 15.0;
	pairPotentialsIncludeCoulomb_ = true;

	// Simulation
	iteration_ = 0;
	nIterationsPerformed_ = 0;
	setUp_ = false;

	// Simulation Setup
	nBoxNormalisationPoints_ = 500000000;
	seed_ = -1;
	restartFileFrequency_ = 10;

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

// Return whether the simulation has been set up
bool Dissolve::isSetUp() const
{
	return setUp_;
}
