/*
    *** Dissolve Main Structure
    *** src/main/dissolve.cpp
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

#include "main/dissolve.h"
#include "classes/atomtype.h"
#include "classes/braggreflection.h"
#include "classes/kvector.h"
#include "classes/neutronweights.h"
#include "classes/partialset.h"
#include "classes/species.h"
#include "genericitems/item.h"
#include "genericitems/items.h"
#include "math/histogram1d.h"
#include "math/histogram2d.h"
#include "math/histogram3d.h"
#include "math/pairbroadeningfunction.h"

// Static Members (ObjectInfo)
bool ObjectInfo::autoSuffixing_ = false;
std::string ObjectInfo::autoSuffix_;

Dissolve::Dissolve(CoreData &coreData) : coreData_(coreData)
{
    // Set Module instances list in our core data
    coreData_.setModuleInstances(&moduleInstances_);

    // Set core simulation variables
    seed_ = -1;
    restartFileFrequency_ = 10;

    // Clear everything
    clear();

    // Parallel Comms
    parallelStrategy_ = Dissolve::SequentialConfigStrategy;
    parallelGroupPopulation_ = ProcessPool::MinimumGroupPopulation;

    // Register GenericItems
    registerGenericItems();
}

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
CoreData &Dissolve::coreData() { return coreData_; }

// Return const reference to CoreData
const CoreData &Dissolve::constCoreData() const { return coreData_; }

// Clear all data
void Dissolve::clear()
{
    // Core
    Messenger::printVerbose("Clearing Core Data...\n");
    coreData_.clear();

    // PairPotentials
    Messenger::printVerbose("Clearing Pair Potentials...\n");
    pairPotentialDelta_ = 0.005;
    pairPotentialRange_ = 15.0;
    pairPotentialRangeSquared_ = pairPotentialRange_ * pairPotentialRange_;
    pairPotentialsIncludeCoulomb_ = false;
    pairPotentials_.clear();
    potentialMap_.clear();
    pairPotentialAtomTypeVersion_ = -1;

    // Modules
    Messenger::printVerbose("Clearing Modules...\n");
    moduleInstances_.clear();

    // Simulation
    Messenger::printVerbose("Clearing Simulation...\n");
    processingLayers_.clear();
    processingModuleData_.clearAll();
    iteration_ = 0;
    nIterationsPerformed_ = 0;

    // I/O
    setInputFilename("");
    restartFilename_.clear();
    saveRestartTimes_ = 0.0;
}

// Register GenericItems
void Dissolve::registerGenericItems()
{
    GenericItem::addItemClass(new GenericItemContainer<bool>("bool"));
    GenericItem::addItemClass(new GenericItemContainer<int>("int"));
    GenericItem::addItemClass(new GenericItemContainer<double>("double"));
    GenericItem::addItemClass(new GenericItemContainer<std::streampos>("streampos"));
    GenericItem::addItemClass(new GenericItemContainer<std::string>("string"));

    GenericItem::addItemClass(new GenericItemContainer<Vec3<int>>("Vec3<int>"));
    GenericItem::addItemClass(new GenericItemContainer<Vec3<double>>("Vec3<double>"));

    GenericItem::addItemClass(new GenericItemContainer<Array2D<double>>("Array2D<double>"));
    GenericItem::addItemClass(new GenericItemContainer<Array2D<Array<double>>>("Array2D<Array<double>>"));
    GenericItem::addItemClass(new GenericItemContainer<Array2D<DummyClass>>("Array2D<DummyClass>"));

    GenericItem::addItemClass(new GenericItemContainer<Array<int>>("Array<int>"));
    GenericItem::addItemClass(new GenericItemContainer<Array<double>>("Array<double>"));
    GenericItem::addItemClass(new GenericItemContainer<Array<DummyClass>>("Array<DummyClass>"));
    GenericItem::addItemClass(new GenericItemContainer<Array<BraggReflection>>("Array<BraggReflection>"));
    GenericItem::addItemClass(new GenericItemContainer<Array<Vec3<int>>>("Array<Vec3<int>>"));
    GenericItem::addItemClass(new GenericItemContainer<Array<Vec3<double>>>("Array<Vec3<double>>"));

    GenericItem::addItemClass(new GenericItemContainer<BraggReflection>(BraggReflection::itemClassName()));
    GenericItem::addItemClass(new GenericItemContainer<Data1D>(Data1D::itemClassName()));
    GenericItem::addItemClass(new GenericItemContainer<Data2D>(Data2D::itemClassName()));
    GenericItem::addItemClass(new GenericItemContainer<Data3D>(Data3D::itemClassName()));
    GenericItem::addItemClass(new GenericItemContainer<Histogram1D>(Histogram1D::itemClassName()));
    GenericItem::addItemClass(new GenericItemContainer<Histogram2D>(Histogram2D::itemClassName()));
    GenericItem::addItemClass(new GenericItemContainer<Histogram3D>(Histogram3D::itemClassName()));
    GenericItem::addItemClass(new GenericItemContainer<Isotopologues>(Isotopologues::itemClassName()));
    GenericItem::addItemClass(new GenericItemContainer<KVector>(KVector::itemClassName()));
    GenericItem::addItemClass(new GenericItemContainer<NeutronWeights>(NeutronWeights::itemClassName()));
    GenericItem::addItemClass(new GenericItemContainer<PairBroadeningFunction>(PairBroadeningFunction::itemClassName()));
    GenericItem::addItemClass(new GenericItemContainer<PartialSet>(PartialSet::itemClassName()));
}
