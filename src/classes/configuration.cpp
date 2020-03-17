/*
	*** Configuration
	*** src/classes/configuration.cpp
	Copyright T. Youngs 2012-2020

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
#include "classes/masterintra.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "base/processpool.h"
#include "base/sysfunc.h"
#include "modules/energy/energy.h"

// Static Members (ObjectStore)
template<class Configuration> RefDataList<Configuration,int> ObjectStore<Configuration>::objects_;
template<class Configuration> int ObjectStore<Configuration>::objectCount_ = 0;
template<class Configuration> int ObjectStore<Configuration>::objectType_ = ObjectInfo::ConfigurationObject;
template<class Configuration> const char* ObjectStore<Configuration>::objectTypeName_ = "Configuration";

// Constructor
Configuration::Configuration() : ListItem<Configuration>(), ObjectStore<Configuration>(this), generator_(ProcedureNode::GenerationContext, "EndGenerator")
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

	// Reset composition
	requestedSizeFactor_ = 1.0;
	appliedSizeFactor_ = 1.0;

	// Reset box / Cells
	requestedCellDivisionLength_ = 7.0;
	contentsVersion_.zero();

	// Reset definition
	temperature_ = 300.0;
	generator_.clear();
}

/*
 * Definition
 */

// Set name of the Configuration
void Configuration::setName(const char* name)
{
	name_ = name;

	// Generate a nice name (i.e. no spaces, slashes etc.)
	niceName_ = DissolveSys::niceName(name_);
}

// Return name of the Configuration
const char* Configuration::name() const
{
	return name_.get();
}

// Return nice name of the Configuration
const char* Configuration::niceName() const
{
	return niceName_.get();
}

// Return the current generator
Procedure& Configuration::generator()
{
	return generator_;
}

// Create the Configuration according to its generator Procedure
bool Configuration::generate(ProcessPool& procPool, double pairPotentialRange)
{
	// Empty the current contents
	empty();

	// Generate the contents
	Messenger::print("\nExecuting generator procedure for Configuration '%s'...\n\n", niceName());
	bool result = generator_.execute(procPool, this, "Generator", moduleData_);
	if (!result) return Messenger::error("Failed to generate Configuration '%s'.\n", niceName());
	Messenger::print("\n");

	// Set-up Cells for the Box
	if (cells_.nCells() == 0) cells_.generate(box_, requestedCellDivisionLength_, pairPotentialRange);

	// Make sure Cell contents / Atom locations are up-to-date
	updateCellContents();

	// Finalise used AtomType list
	usedAtomTypes_.finalise();

	++contentsVersion_;

	// Sanity check the contents - if we have zero atoms then there's a problem!
	if (nAtoms() == 0) return Messenger::error("Generated contents for Configuration '%s' contains no atoms!\n", name());

	return true;
}

// Return import coordinates file / format
CoordinateImportFileFormat& Configuration::inputCoordinates()
{
	return inputCoordinates_;
}

// Load coordinates from file
bool Configuration::loadCoordinates(LineParser& parser, CoordinateImportFileFormat::CoordinateImportFormat format)
{
	// Load coordinates into temporary array
	Array< Vec3<double> > r;
	CoordinateImportFileFormat coordinateFormat(format);
	if (!coordinateFormat.importData(parser, r)) return false;

	// Temporary array now contains some number of atoms - does it match the number in the configuration's molecules?
	if (atoms_.nItems() != r.nItems())
	{
		Messenger::error("Number of atoms read from initial coordinates file (%i) does not match that in Configuration (%i).\n", r.nItems(), atoms_.nItems());
		return false;
	}

	// All good, so copy atom coordinates over into our array
	for (int n=0; n<atoms_.nItems(); ++n) atoms_[n]->setCoordinates(r[n]);

	return true;
}

// Initialise (generate or load) the basic contents of the Configuration
bool Configuration::initialiseContent(ProcessPool& procPool, double pairPotentialRange, bool emptyCurrentContent)
{
	// Clear existing content?
	if (emptyCurrentContent) empty();

	/*
	 * Content Initialisation
	 */

	// If the Configuation is currently empty, run the generator Procedure and potentially load coordinates from file
	if (nAtoms() == 0)
	{
		// Run the generator procedure (we will need species / atom info to load any coordinates in to anyway)
		if (!generate(procPool, pairPotentialRange)) return false;

		// If there are still no atoms, complain.
		if (nAtoms() == 0) return false;

		// If an input file was specified, try to load it
		if (inputCoordinates_.hasValidFileAndFormat())
		{
			if (DissolveSys::fileExists(inputCoordinates_))
			{
				Messenger::print("Loading initial coordinates from file '%s'...\n", inputCoordinates_.filename());
				LineParser inputFileParser(&procPool);
				if (!inputFileParser.openInput(inputCoordinates_)) return false;
				if (!loadCoordinates(inputFileParser, inputCoordinates_.coordinateFormat())) return false;
				inputFileParser.closeFiles();

				// Need to update cell locations now, as we have new atom positions
				updateCellContents();
			}
			else return Messenger::error("Input coordinates file '%s' specified for Configuration '%s', but the file doesn't exist.\n", name(), inputCoordinates_.filename());
		}
	}

	return true;
}

// Set configuration temperature
void Configuration::setTemperature(double t)
{
	temperature_ = t;
}

// Return configuration temperature
double Configuration::temperature() const
{
	return temperature_;
}

/*
 * Modules
 */

// Associate Module to the Configuration
bool Configuration::ownModule(Module* module)
{
	return moduleLayer_.own(module);
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

	// Broadcast contents version
	if (!contentsVersion_.broadcast(procPool, rootRank)) return false;
#endif
	return true;
}
