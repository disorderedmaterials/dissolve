/*
	*** Configuration
	*** src/classes/configuration.cpp
	Copyright T. Youngs 2012-2018

	This file is part of Configuration.

	Configuration is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Configuration is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Configuration.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "classes/configuration.h"
#include "classes/atomtype.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/grain.h"
#include "classes/masterintra.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "modules/export/export.h"

// Constructor
Configuration::Configuration() : ListItem<Configuration>()
{
	// Contents

	// Composition
	multiplier_ = 1;
	density_ = -1.0;
	densityIsAtomic_ = true;
	boxAngles_.set(90.0, 90.0, 90.0);
	relativeBoxLengths_.set(1.0, 1.0, 1.0);
	nonPeriodic_ = false;
	inputCoordinatesFormat_ = ImportModuleFormats::XYZCoordinates;

	// Box / Cells
	box_ = NULL;
	requestedCellDivisionLength_ = 10.0;
	coordinateIndex_ = 0;

	// Set up
	rdfBinWidth_ = 0.025;
	rdfRange_ = -1.0;
	requestedRDFRange_ = -2.0;
	temperature_ = 300.0;
	braggQMax_ = 0.01;
	braggQMax_ = 2.0;
	braggMultiplicity_.set(1,1,1);

	// Ensemble
	appendEnsemble_ = false;
	ensembleFrequency_ = 10;
}

// Destructor
Configuration::~Configuration()
{
	clear();
}

// Assignment operator
void Configuration::operator=(Configuration& source)
{
	Messenger::error("XXX CONFIGURATION COPY (via ASSIGNMENT OPERATOR) IS NOT YET IMPLEMENTED.\n");
}

// Clear data
void Configuration::clear()
{
	bonds_.clear();
	angles_.clear();
	torsions_.clear();
	molecules_.clear();
	grains_.clear();
	atoms_.clear();
	usedAtomTypes_.clear();
	if (box_ != NULL) delete box_;
	box_ = NULL;
	cells_.clear();
}

/*
 * Calculation Limits
 */

// Set RDF bin width
void Configuration::setRDFBinWidth(double width)
{
	rdfBinWidth_ = width;
}

// Return RDF bin width
double Configuration::rdfBinWidth()
{
	return rdfBinWidth_;
}

// Return working RDF extent
double Configuration::rdfRange()
{
	return rdfRange_;
}

// Set requested RDF extent
void Configuration::setRequestedRDFRange(double range)
{
	requestedRDFRange_ = range;
}

// Return requested RDF extent
double Configuration::requestedRDFRange()
{
	return requestedRDFRange_;
}

// Set minimum Q value for Bragg calculation
void Configuration::setBraggQMin(double qMin)
{
	braggQMin_ = qMin;
}

// Return minimum Q value for Bragg calculation
double Configuration::braggQMin()
{
	return braggQMin_;
}

// Set maximum Q value for Bragg calculation
void Configuration::setBraggQMax(double qMax)
{
	braggQMax_ = qMax;
}

// Return maximum Q value for Bragg calculation
double Configuration::braggQMax()
{
	return braggQMax_;
}

// Set multiplicities reflecting any crystal supercell
void Configuration::setBraggMultiplicity(Vec3<int> mult)
{
	braggMultiplicity_ = mult;
}

// Return multiplicities reflecting any crystal supercell
Vec3<int> Configuration::braggMultiplicity()
{
	return braggMultiplicity_;
}

/*
 * Modules
 */

// Add Module (or an instance of it) to the Configuration
Module* Configuration::addModule(Module* masterInstance)
{
	return modules_.addModule(masterInstance, moduleData_);
}

// Return number of Modules associated to this Configuration
int Configuration::nModules() const
{
	return modules_.nModules();
}

// Return list of Modules associated to this Configuration
ModuleList& Configuration::modules()
{
	return modules_;
}

// Return list of data variables set by Modules
GenericList& Configuration::moduleData()
{
	return moduleData_;
}

/*
 * Ensemble
 */

// Set whether ensemble file is to be appended
void Configuration::setAppendEnsemble(bool b)
{
	appendEnsemble_ = b;
}

// Return whether ensemble file is to be appended
bool Configuration::appendEnsemble() const
{
	return appendEnsemble_;
}

// Return frequency at which to append ensemble
int Configuration::setEnsembleFrequency(int frequency)
{
	ensembleFrequency_ = frequency;
}

// Return frequency at which to append ensemble
int Configuration::ensembleFrequency() const
{
	return ensembleFrequency_;
}

/*
 * Parallel Comms
 */

// Set up process pool for this Configuration
bool Configuration::setUpProcessPool(Array<int> worldRanks)
{
	// Create pool
	processPool_.setUp(name_, worldRanks);

	// Give Cell info to our processPool_ so a suitable parallel strategy can be created
	processPool_.determineMaxProcessGroups(cells_.divisions(), cells_.extents(), cells_.neighbourIndices());
	if (!processPool_.assignProcessesToGroups()) return false;
	processPool_.setGroupsFixed();

	return true;
}

// Return process pool for this Configuration
ProcessPool& Configuration::processPool()
{
	return processPool_;
}

// Broadcast coordinate from specified root process
bool Configuration::broadcastCoordinates(ProcessPool& procPool, int rootRank)
{
#ifdef PARALLEL
	double* x, *y, *z;
	x = new double[atoms_.nItems()];
	y = new double[atoms_.nItems()];
	z = new double[atoms_.nItems()];
	
	// Master assembles Atom coordinate arrays...
	if (procPool.poolRank() == rootRank)
	{
		Messenger::printVerbose("Process rank %i is assembling coordinate data...\n", procPool.poolRank());
		for (int n=0; n<atoms_.nItems(); ++n)
		{
			x[n] = atoms_[n]->r().x;
			y[n] = atoms_[n]->r().y;
			z[n] = atoms_[n]->r().z;
		}
	}

	if (!procPool.broadcast(x, atoms_.nItems(), rootRank)) return false;
	if (!procPool.broadcast(y, atoms_.nItems(), rootRank)) return false;
	if (!procPool.broadcast(z, atoms_.nItems(), rootRank)) return false;
	
	// Slaves then store values into Atoms, updating related info as we go
	if (procPool.isSlave()) for (int n=0; n<atoms_.nItems(); ++n) atoms_[n]->setCoordinates(x[n], y[n], z[n]);

	delete[] x;
	delete[] y;
	delete[] z;

	// Update coordinate index
	if (!procPool.broadcast(coordinateIndex_, rootRank)) return false;
#endif
	return true;
}
