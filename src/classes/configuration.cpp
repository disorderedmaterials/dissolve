/*
	*** Configuration
	*** src/classes/configuration.cpp
	Copyright T. Youngs 2012-2016

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
#include "classes/box.h"
#include "classes/grain.h"
#include "classes/species.h"
#include "base/processpool.h"
#include "base/sysfunc.h"

// Constructor
Configuration::Configuration() : ListItem<Configuration>()
{
	// Contents
	atoms_ = NULL;
	grains_ = NULL;
	nAtoms_ = 0;
	nGrains_ = 0;

	// Composition
	multiplier_ = 1;
	density_ = 0.1;
	densityIsAtomic_ = true;
	boxAngles_.set(90.0, 90.0, 90.0);
	relativeBoxLengths_.set(1.0, 1.0, 1.0);
	nonPeriodic_ = false;
	randomConfiguration_ = true;

	// Box / Cells
	box_ = NULL;
	cells_ = NULL;
	cellFlag_ = NULL;
	nCells_ = 0;
	coordinateIndex_ = 0;

	// Partials
	partialsIndex_ = -1;

	// Setup
	rdfBinWidth_ = 0.025;
	rdfRange_ = -1.0;
	rdfSmoothing_ = 0;
	requestedRDFRange_ = -1.0;
	temperature_ = 300.0;
	braggMaximumQ_ = 1.0;
}

// Destructor
Configuration::~Configuration()
{
	clear();
}

// Assignment operator
void Configuration::operator=(Configuration &source)
{
	Messenger::error("XXX CONFIGURATION COPY (via ASSIGNMENT OPERATOR) IS NOT YET IMPLEMENTED.\n");
// 	clear();
// 
// 	// Create arrays
// 	nAtoms_ = source.nAtoms_;
// 	nGrains_ = source.nGrains_;
// 	grains_ = new Grain[nGrains_];
// 
// 	// Set Grain indices
// 	int n;
// 	for (n=0; n<nGrains_; ++n) grains_[n].setIndex(n);
// 
// // // 	// Copy basic Molecule information, and assign Atom/Grains	
// // // 	int atomCount = 0, grainCount = 0, startAtom = 0;
// // // 	Molecule* newMol;
// // // 	for (Molecule* mol = source.molecules_.first(); mol != NULL; mol = mol->next)
// // // 	{
// // // 		// Create Molecule copy
// // // 		newMol = molecules_.add();
// // // 		newMol->initialise(mol->species(), molecules_.nItems()-1);
// // // 
// // // 		// Assign Atoms
// // // 		for (n = 0; n<newMol->nAtoms(); ++n)
// // // 		{
// // // 			if (atomCount >= nAtoms_)
// // // 			{
// // // 				Messenger::error("Mismatch between size of Atom array in Configuration, and number of Atoms needed by Molecules (in assignment operator).\n");
// // // 				return;
// // // 			}
// // // 			newMol->setAtom(n, &atoms_[atomCount++]);
// // // 		}
// // // 		
// // // 		// Assign Grains
// // // 		for (n = 0; n<newMol->nGrains(); ++n)
// // // 		{
// // // 			if (grainCount >= nGrains_)
// // // 			{
// // // 				Messenger::error("Mismatch between size of Grain array in Configuration, and number of Grains needed by Molecules (in assignment operator).\n");
// // // 				return;
// // // 			}
// // // 			newMol->setGrain(n, &grains_[grainCount]);
// // // 			grains_[grainCount].setParent(newMol);
// // // 			++grainCount;
// // // 		}
// // // 		
// // // 		// Instantiate (without setting Atom properties)
// // // 		newMol->instantiate(false);
// // // 
// // // 		// Increase startAtom
// // // 		startAtom += newMol->nAtoms();
// // // 	}
// // // 
// 
// 	// Copy Box
// 	if (source.box_ != NULL)
// 	{
// 		// Create a new Box of the correct class, sending dummy parameters to the constructor
// 		if (source.box_->type() == Box::NonPeriodicBox) box_ = new NonPeriodicBox(1.0);
// 		else if (source.box_->type() == Box::CubicBox) box_ = new CubicBox(1.0);
// 		else if (source.box_->type() == Box::OrthorhombicBox) box_ = new OrthorhombicBox(1.0, Vec3<double>(1.0,1.0,1.0));
// 		else if (source.box_->type() == Box::MonoclinicBox) box_ = new MonoclinicBox(1.0, Vec3<double>(1.0,1.0,1.0), 90.0);
// 		else if (source.box_->type() == Box::TriclinicBox) box_ = new TriclinicBox(1.0, Vec3<double>(1.0,1.0,1.0), Vec3<double>(90,90,90));
// 		
// 		// Now copy proper Box information
// 		(*box_) = (*source.box_);
// 	}
// 
// 	// Cell data
// 	divisions_ = source.divisions_;
// 	cellSize_ = source.cellSize_;
// 	realCellSize_ = source.realCellSize_;
// 	cellExtents_ = source.cellExtents_;
// 	clearCells();
// 	nCells_ = source.nCells_;
// 	cells_ = new Cell[nCells_];
// 	Cell* oldCell;
// 	int m, x, y, z, count = 0;
// 	for (x = 0; x<divisions_.x; ++x)
// 	{
// 		for (y = 0; y<divisions_.y; ++y)
// 		{
// 			for (z = 0; z < divisions_.z; ++z)
// 			{
// 				cells_[count].setIndex(count);
// 				cells_[count].setGridReference(x,y,z);
// 				++count;
// 			}
// 		}
// 	}
// 
// 	// Copy cellFlag_
// 	cellFlag_ = new bool[nCells_];
// 	for (n=0; n<nCells_; ++n) cellFlag_[n] = source.cellFlag_[n];
// 
// 	// Copy neighbour information from source...
// 	RefListItem<Cell,bool>* ri;
// 	for (n=0; n<nCells_; ++n)
// 	{
// 		for (ri = source.cells_[n].neighbours().first(); ri != NULL; ri = ri->next) cells_[n].addNeighbour(&cells_[ri->item->index()], ri->data);
// 	}
// 
// 	nCellsDistributed_ = source.nCellsDistributed_;
// 	lastCellDistributed_ = source.lastCellDistributed_;
// 
// 	// Reconstruct Atom reference array
// 	// TODO
// 
// 
// 	coordinateIndex_ = source.coordinateIndex_;
// 
// 	return;
}

// Clear data
void Configuration::clear()
{
	molecules_.clear();
	if (grains_ != NULL) delete[] grains_;
	grains_ = NULL;
	nGrains_ = 0;
	nAtoms_ = 0;
	if (box_ == NULL) delete box_;
	box_ = NULL;
	clearCells();
}

// Setup configuration
bool Configuration::setup(ProcessPool& procPool, const List<AtomType>& atomTypes, double pairPotentialRange, int boxNormalisationNPoints)
{
	/*
	 * Order of business:
	 * 1) Check for a sensible multiplier for the system
	 * 2) Create all molecules in the system, based on the defined multiplier and Species
	 * 3) Add a periodic box (since we now know the total number of atoms in the system, and hence can work out the number density)
 	 * 4) Check RDF range
	 * 5) Create atom/grain arrays
	 * 6) Calculate/load Box normalisation if necessary
	 */

	// Check Configuration multiplier
	if (multiplier_ < 1)
	{
		Messenger::error("Configuration multiplier is zero or negative (%i).\n", multiplier_);
		return false;
	}

	// Create 'empty' molecules in the main Configuration
	Messenger::print("--> Creating space for molecules...\n");
	for (RefListItem<Species,double>* refSp = usedSpecies_.first(); refSp != NULL; refSp = refSp->next)
	{
		// Determine the number of molecules of this component
		int count = refSp->data * multiplier_;

		// Check for zero count
		if (count == 0)
		{
			Messenger::error("Relative population for Species '%s' is too low (%e) to provide any Molecules in this Configuration.\n", refSp->item->name(), refSp->data);
			return false;
		}

		// Add Molecule instances for this Species
		for (int n = 0; n < count; ++n) addMolecule(refSp->item);
	}

	// Create a Box
	Messenger::print("--> Creating periodic box...\n");
	if (!setupBox(pairPotentialRange))
	{
		Messenger::error("Failed to set-up Box/Cells for Configuration.\n");
		return false;
	}

	// Determine maximal extent of RDF (from origin to centre of box)
	Vec3<double> half = box()->axes() * Vec3<double>(0.5,0.5,0.5);
	double maxR = half.magnitude(), inscribedSphereRadius = box()->inscribedSphereRadius();
	Messenger::print("\n");
	Messenger::print("--> Maximal extent for RDFs is %f Angstrom (half cell diagonal distance).\n", maxR);
	Messenger::print("--> Inscribed sphere radius (maximum RDF range avoiding periodic images) is %f Angstroms.\n", inscribedSphereRadius);
	if (requestedRDFRange_ < -1.5)
	{
		Messenger::print("--> Using maximal non-minimum image range for RDFs.\n");
		rdfRange_ = inscribedSphereRadius;
	}
	else if (requestedRDFRange_ < -0.5)
	{
		Messenger::print("--> Using 90%% of maximal extent for RDFs.\n");
		rdfRange_ = 0.90*maxR;
	}
	else
	{
		Messenger::print("--> Specific RDF range supplied (%f Angstroms).\n", requestedRDFRange_);
		rdfRange_ = requestedRDFRange_;
		if (rdfRange_ < 0.0)
		{
			Messenger::error("Negative RDF range requested.\n");
			return false;
		}
		else if (rdfRange_ > maxR)
		{
			Messenger::error("Requested RDF range is greater then the maximum possible extent for the Box.\n");
			return false;
		}
		else if (rdfRange_ > (0.90*maxR)) Messenger::warn("Requested RDF range is greater than 90%% of the maximum possible extent for the Box. FT may be suspect!\n");
	}
	// 'Snap' rdfRange_ to nearest bin width...
	rdfRange_ = int(rdfRange_/rdfBinWidth_) * rdfBinWidth_;
	Messenger::print("--> RDF range (snapped to bin width) is %f Angstroms.\n", rdfRange_);

	// Setup arrays for atoms and grains, based on the local molecules list
	Messenger::print("--> Creating Atom/Grain arrays...\n");
	if (!createArrays())
	{
		Messenger::error("Failed to create arrays for Configuration.\n");
		return false;
	}

	// Now, we need some coordinates - either we are creating a random configuration of molecules, or we are loading in a set of coordinates from a file
	if (randomConfiguration())
	{
		Messenger::print("--> Generating random initial configuration...\n");
		if (!createRandom()) return false;
	}
	else
	{
		Messenger::print("--> Loading initial coordinates from file '%s'...\n", initialCoordinatesFile_.get());
		if (!loadCoordinates(initialCoordinatesFile_)) return false;
	}

	// Populate Atom and Grain arrays from Molecule copies
	Messenger::print("--> Setting up Molecules, Atoms, and Grains...\n");
	if (!setupMolecules())
	{
		Messenger::error("Failed to setup molecules.\n");
		return false;
	}

	// Initialise neighbour lists and update grains
	Messenger::print("--> Initialising Cell neighbour lists and Grains\n");
	if (!updateAtomsInCells()) return false;
	updateGrains();
	
	// Initialise cell atom neighbour lists
	recreateCellAtomNeighbourLists(pairPotentialRange);

	// Create/load Box normalisation array
	if (!boxNormalisationFileName_.isEmpty())
	{
		// Master will open file and attempt to read it...
		bool loadResult;
		if (procPool.isMaster()) loadResult = boxNormalisation_.load(boxNormalisationFileName_);
		if (!procPool.broadcast(&loadResult, 1)) return false;

		// Did we successfully load the file?
		if (loadResult)
		{
			Messenger::print("--> Successfully loaded box normalisation data from file '%s'.\n", boxNormalisationFileName_.get());
			if (!boxNormalisation_.broadcast(procPool)) return false;
		}
		else Messenger::print("--> Couldn't load Box normalisation data - it will be calculated.\n");
	}
	if (boxNormalisation_.nPoints() <= 1)
	{
		// Only calculate if RDF range is greater than the inscribed sphere radius
		if (rdfRange_ <= inscribedSphereRadius)
		{
			Messenger::print("--> No need to calculate Box normalisation array since rdfRange is within periodic range.\n");
			boxNormalisation_.clear();
			double x = rdfBinWidth_*0.5;
			while (x < rdfRange_)
			{
				boxNormalisation_.addPoint(x, 1.0);
				x += rdfBinWidth_;
			}
		}
		else
		{
			Messenger::print("--> Calculating box normalisation array for RDFs...\n");
			if (!box()->calculateRDFNormalisation(procPool, boxNormalisation_, rdfRange_, rdfBinWidth_, boxNormalisationNPoints)) return false;
		}
	}

	// Interpolate the Box normalisation array
	boxNormalisation_.interpolate();
	
	return true;
}

/*
 * Parallel Comms
 */

// Setup process pool for this Configuration
bool Configuration::setupProcessPool(Array<int> worldRanks)
{
	// Create pool
	processPool_.setup(name_, worldRanks);

	// Assign Atom/Grain limits to processes
	if (!processPool_.calculateLimits(nAtoms(), nGrains())) return false;
	
	// Send Cell info to Comm so suitable parallel strategy over cells can be deduced
	if (!processPool_.setupCellStrategy(divisions_, cellExtents_, cellNeighbourIndices_)) return false;
}

// Return process pool for this Configuration
ProcessPool& Configuration::processPool()
{
	return processPool_;
}

// Broadcast data
bool Configuration::broadcast(ProcessPool& procPool, const List<Species>& species, double pairPotentialRange, const RefList<Module,bool>& allModules)
{
#ifdef PARALLEL
	// Composition
	if (!procPool.broadcast(name_)) return false;
	bool result;
	BroadcastRefList<Species,double>(procPool, usedSpecies_, species, result);
	if (!result) return false;
	if (!procPool.broadcast(&multiplier_, 1)) return false;
	if (!procPool.broadcast(&density_, 1)) return false;
	if (!procPool.broadcast(&densityIsAtomic_, 1)) return false;
	if (!procPool.broadcast(&nonPeriodic_, 1)) return false;
	if (!procPool.broadcast(&randomConfiguration_, 1)) return false;
	if (!procPool.broadcast(initialCoordinatesFile_)) return false;
	if (!procPool.broadcast(&temperature_, 1)) return false;

	// Content
	// -- Broadcast size of molecule list
	int count = molecules_.nItems(), spIndex;
	if (!procPool.broadcast(&count, 1)) return false;
	// Create molecules, which will also create the atom and grain space for us
	for (int n=0; n<count; ++n)
	{
		// Broadcast index of Species
		if (procPool.isMaster()) spIndex = species.indexOf(molecules_[n]->species());
		if (!procPool.broadcast(&spIndex, 1)) return false;

		// Add Molecule to Configuration
		if (procPool.isSlave()) addMolecule(species.item(spIndex));
	}
	// Create Atom / Grain arrays
	if (procPool.isSlave() && (!createArrays())) return false;

	// Box
	if (!procPool.broadcast(relativeBoxLengths_)) return false;
	if (!procPool.broadcast(boxAngles_)) return false;
	if (!procPool.broadcast(&nonPeriodic_, 1)) return false;
	if (procPool.isSlave() && (!setupBox(pairPotentialRange))) return false;
	if (!procPool.broadcast(boxNormalisationFileName_)) return false;
	if (!boxNormalisation_.broadcast(procPool)) return false;

	// Coordinates
	double* x, *y, *z;
	x = new double[nAtoms_];
	y = new double[nAtoms_];
	z = new double[nAtoms_];
	
	// Master assembles Atom coordinate arrays...
	if (procPool.isMaster())
	{
		Messenger::print("--> Sending Configuration to slaves...\n");
		for (int n=0; n<nAtoms_; ++n)
		{
			x[n] = atoms_[n].r().x;
			y[n] = atoms_[n].r().y;
			z[n] = atoms_[n].r().z;
		}
	}
	else Messenger::print("--> Waiting for Master to send Configuration.\n");

	if (!procPool.broadcast(x, nAtoms_)) return false;
	if (!procPool.broadcast(y, nAtoms_)) return false;
	if (!procPool.broadcast(z, nAtoms_)) return false;
	
	// Slaves then store values into Atoms...
	if (procPool.isSlave()) for (int n=0; n<nAtoms_; ++n) atoms_[n].setCoordinatesNasty(x[n], y[n], z[n]);

	delete[] x;
	delete[] y;
	delete[] z;

	if (!procPool.broadcast(&coordinateIndex_, 1)) return false;

	// Molecule Setup
	if (procPool.isSlave() && !(setupMolecules())) return false;

	// RDF Range
	if (!procPool.broadcast(&requestedRDFRange_, 1)) return false;
	if (!procPool.broadcast(&rdfRange_, 1)) return false;
	if (!procPool.broadcast(&rdfBinWidth_, 1)) return false;

	// Modules
	// Master sends number of modules of to expect
	int moduleCount;
	if (procPool.isMaster()) moduleCount = modules_.nItems();
	if (!procPool.broadcast(&moduleCount, 1)) return false;

	// Loop over associated modules of this type
	for (int m = 0; m<moduleCount; ++m)
	{
		// Grab existing module pointer
		Module* moduleOnMaster;
		if (procPool.isMaster()) moduleOnMaster = modules_.item(m)->item;

		// Master will broadcast module name to slaves
		Dnchar moduleName;
		if (procPool.isMaster()) moduleName = moduleOnMaster->name();
		if (!procPool.broadcast(moduleName)) return false;

		// Find pointer to master module instance
		Module* masterInstance = NULL;
		for (RefListItem<Module,bool>* ri = allModules.first(); ri != NULL; ri = ri->next)
		{
			if (DUQSys::sameString(ri->item->name(), moduleName.get()))
			{
				masterInstance = ri->item;
				break;
			}
		}

		// Check pointer
		if (!masterInstance)
		{
			Messenger::print("Failed to find master instance of module '%s'.\n", moduleName.get());
			return false;
		}

		// Add this module to the current Configuration
		if (procPool.isSlave())
		{
			// Add module
			Module* newModule = addModule(masterInstance);

			// Receive VariableList associated to module
			newModule->broadcastVariables(procPool);
		}
		else
		{
			// Just broadcast existing VariableList
			moduleOnMaster->broadcastVariables(procPool);
		}
	}
#endif
	return true;
}

// Broadcast coordinate from specified root process
bool Configuration::broadcastCoordinates(ProcessPool& procPool, int rootRank)
{
#ifdef PARALLEL
	double* x, *y, *z;
	x = new double[nAtoms_];
	y = new double[nAtoms_];
	z = new double[nAtoms_];
	
	// Master assembles Atom coordinate arrays...
	if (procPool.poolRank() == rootRank)
	{
		Messenger::printVerbose("--> Process rank %i is assembling coordinate data...\n", procPool.poolRank());
		for (int n=0; n<nAtoms_; ++n)
		{
			x[n] = atoms_[n].r().x;
			y[n] = atoms_[n].r().y;
			z[n] = atoms_[n].r().z;
		}
	}

	if (!procPool.broadcast(x, nAtoms_, rootRank)) return false;
	if (!procPool.broadcast(y, nAtoms_, rootRank)) return false;
	if (!procPool.broadcast(z, nAtoms_, rootRank)) return false;
	
	// Slaves then store values into Atoms, updating related info as we go
	if (procPool.isSlave()) for (int n=0; n<nAtoms_; ++n) atoms_[n].setCoordinates(x[n], y[n], z[n]);

	delete[] x;
	delete[] y;
	delete[] z;
#endif
	return true;
}
