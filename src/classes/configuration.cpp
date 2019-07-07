/*
	*** Configuration
	*** src/classes/configuration.cpp
	Copyright T. Youngs 2012-2019

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
#include "modules/energy/energy.h"
#include "modules/export/export.h"

// Static Members (ObjectStore)
template<class Configuration> RefList<Configuration,int> ObjectStore<Configuration>::objects_;
template<class Configuration> int ObjectStore<Configuration>::objectCount_ = 0;
template<class Configuration> int ObjectStore<Configuration>::objectType_ = ObjectInfo::ConfigurationObject;
template<class Configuration> const char* ObjectStore<Configuration>::objectTypeName_ = "Configuration";

// Constructor
Configuration::Configuration() : ListItem<Configuration>(), ObjectStore<Configuration>(this), boxNormalisationInterpolation_(boxNormalisation_)
{
	box_ = NULL;
	clear();
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

// Clear all data
void Configuration::clear()
{
	// Clear generated content
	empty();

	// Clear used species
	usedSpecies_.clear();

	// Reset composition
	multiplier_ = 1;
	density_ = -1.0;
	densityIsAtomic_ = true;
	boxAngles_.set(90.0, 90.0, 90.0);
	relativeBoxLengths_.set(1.0, 1.0, 1.0);
	requestedSizeFactor_ = 1.0;
	appliedSizeFactor_ = 1.0;
	nonPeriodic_ = false;

	// Reset box / Cells
	requestedCellDivisionLength_ = 10.0;
	contentsVersion_.zero();

	// Reset set-up
	rdfBinWidth_ = 0.025;
	rdfRange_ = -1.0;
	requestedRDFRange_ = -2.0;
	temperature_ = 300.0;
}

/*
 * Basic Information
 */

// Set name of the Configuration
void Configuration::setName(const char* name)
{
	name_ = name;

	// Generate a nice name (i.e. no spaces, slashes etc.)
	niceName_ = DissolveSys::niceName(name_);

	// Set box normalisation filename based on Configuration name
	boxNormalisationFileName_ = niceName_;
	boxNormalisationFileName_.strcat(".boxnorm");
}

// Return name of the Configuration
const char* Configuration::name()
{
	return name_.get();
}

// Return nice name of the Configuration
const char* Configuration::niceName()
{
	return niceName_.get();
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

/*
 * Modules
 */

// Add Module to the Configuration
bool Configuration::addModule(Module* module)
{
	return moduleLayer_.add(module);
}

// Return number of Modules associated to this Configuration
int Configuration::nModules() const
{
	return moduleLayer_.nModules();
}

// Return Module layer for this Configuration
ModuleLayer& Configuration::moduleLayer()
{
	return moduleLayer_;
}

// Return list of Modules associated to this Configuration
ModuleList& Configuration::modules()
{
	return moduleLayer_;
}

// Return list of data variables set by Modules
GenericList& Configuration::moduleData()
{
	return moduleData_;
}

/*
 * Preparation
 */

// Perform any pre-processing tasks for the Configuration
bool Configuration::prepare(const PotentialMap& potentialMap)
{
	/*
	 * Size Factor Scaling
	 * 
	 * Scale Box, Cells, and Molecule geometric centres according to current sizeFactor_
	 */

	const double reductionFactor = 0.95;

	while (true)
	{
		// Calculate ratio between current and applied size factors for use later on
		const double sizeFactorRatio = requestedSizeFactor_ / appliedSizeFactor_;

		// Check current vs applied size factors (via the ratio) - if unequal, perform scaling and set the new applied size factor
		if (fabs(sizeFactorRatio - 1.0) > 1.0e-5)
		{
			Messenger::print("Requested SizeFactor for Configuration is %f, current SizeFactor is %f, so scaling Box contents.\n", requestedSizeFactor_, appliedSizeFactor_);

			/*
			 * Recalculate all Atom positions, molecule-by-molecule
			 * 
			 * First, work out the centre of geometry of the Molecule, and fold it into the Box.
			 * Calculate the scaled centre of geometry coordinate by dividing by the old scale factor, and multiplying by the new one.
			 * Calculate the minimum image delta between each Atom and the original center of geometry.
			 * Add this delta on to the new centre of geometry to get the new Atom coordinate.
			 */

			Vec3<double> oldCog, newCog, newPos;
			for (int n=0; n<molecules_.nItems(); ++n)
			{
				// Get Molecule pointer
				Molecule* mol = molecules_[n];

				// Calculate current and new centre of geometry
				oldCog = box()->fold(mol->centreOfGeometry(box()));
				newCog = oldCog * sizeFactorRatio;

				// Loop over Atoms in Molecule, setting new coordinates as we go. Remove Atom from its current Cell at the same time
				for (int m=0; m<mol->nAtoms(); ++m)
				{
					// Get Atom pointer
					Atom* i = mol->atom(m);

					// Remove from its current Cell
					if (i->cell()) i->cell()->removeAtom(i);

					// Calculate and set new position
					newPos = newCog + box()->minimumVector(i->r(), oldCog);
					i->setCoordinates(newPos);
				}
			}

			// Now scale the Box and its Cells
			scaleBox(sizeFactorRatio);

			// Re-assign all Atoms to Cells
			updateCellContents();

			// Store new size factors
			appliedSizeFactor_ = requestedSizeFactor_;

			// Can now break out of the loop
			break;
		}

		// Now check the current sizeFactor or energy
		//  -- If the current sizeFactor is 1.0, break
		//  -- Otherwise, check energy - if it is negative, reduce requested size factor and loop
		//  -- If energy is positive, break
		if (fabs(requestedSizeFactor_ - 1.0) < 1.0e-5) break;
		else if (EnergyModule::interMolecularEnergy(processPool_, this, potentialMap) <= 0.0)
		{
			requestedSizeFactor_ *= reductionFactor;
			if (requestedSizeFactor_ < 1.0) requestedSizeFactor_ = 1.0;
			Messenger::print("Intermolecular energy is zero or negative, so reducing SizeFactor to %f\n", requestedSizeFactor_);
		}
		else
		{
			Messenger::print("Intermolecular energy is positive, so SizeFactor remains at %f\n", requestedSizeFactor_);
			break;
		}
	}

	return true;
}

/*
 * Parallel Comms
 */

// Set up process pool for this Configuration
bool Configuration::setUpProcessPool(Array<int> worldRanks, int groupPopulation)
{
	// Create pool
	processPool_.setUp(name_, worldRanks, groupPopulation);

	// Assign processes, and 
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
