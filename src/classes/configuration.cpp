/*
	*** Configuration
	*** src/classes/configuration.cpp
	Copyright T. Youngs 2012-2017

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
	randomConfiguration_ = true;
	useOutputCoordinatesAsInput_ = false;
	coordinatesOutputFrequency_ = 1;
	inputCoordinatesFormat_ = "xyz";

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
	grains_.clear();
	atoms_.clear();
	if (box_ != NULL) delete box_;
	box_ = NULL;
	cells_.clear();
}

// Set up configuration
bool Configuration::setUp(ProcessPool& procPool, double pairPotentialRange, int boxNormalisationNPoints)
{
	Messenger::print("--> Setting up Configuration from Species / multiplier definition...\n");

	/*
	 * Check Species populations, and calculate total number of expected Atoms
	 */
	if (multiplier_ < 1)
	{
		Messenger::error("Configuration multiplier is zero or negative (%i).\n", multiplier_);
		return false;
	}
	int nExpectedAtoms = 0;
	for (SpeciesInfo* spInfo = usedSpecies_.first(); spInfo != NULL; spInfo = spInfo->next)
	{
		// Get Species pointer
		Species* sp = spInfo->species();

		// Determine the number of molecules of this component
		int count =  spInfo->population() * multiplier_;

		// Check for zero count
		if (count == 0)
		{
			Messenger::error("Relative population for Species '%s' is too low (%e) to provide any Molecules in this Configuration.\n",  sp->name(),  spInfo->population());
			return false;
		}

		nExpectedAtoms += count * sp->nAtoms();
	}

	
	/*
	 * Create a Box to contain the system
	 */
	Messenger::print("--> Creating periodic Box and Cell partitioning...\n");
	if (!setUpBox(pairPotentialRange, nExpectedAtoms))
	{
		Messenger::error("Failed to set up Box/Cells for Configuration.\n");
		return false;
	}

	
	/*
	 * Create Molecules
	 */
	Messenger::print("--> Setting up Molecules...\n");

	bool randomise = randomConfiguration_ && ((!useOutputCoordinatesAsInput_) || outputCoordinatesFile_.isEmpty() || (!DUQSys::fileExists(outputCoordinatesFile_)));
	if (randomise) procPool.initialiseRandomBuffer(ProcessPool::Pool);
	Vec3<double> r, cog, newCentre, fr;
	Matrix3 transform;

	ListIterator<SpeciesInfo> speciesInfoIterator(usedSpecies_);
	while (SpeciesInfo* spInfo = speciesInfoIterator.iterate())
	{
		// Determine the number of molecules of this component
		int count = spInfo->population() * multiplier_;

		// Add copies of Species as Molecules
		for (int n=0; n<count; ++n)
		{
			// Add the Molecule
			Molecule* mol = addMolecule(spInfo->species());

			// Generate random positions and orientations if needed
			if (randomise)
			{
				// Generate a new random centre of geometry for the molecule
				if (spInfo->translateOnInsertion())
				{
					fr.set(procPool.random(), procPool.random(), procPool.random());
					newCentre = box_->fracToReal(fr);
					mol->setCentre(box_, newCentre);
				}
			
				// Generate and apply a random rotation matrix
				if (spInfo->rotateOnInsertion())
				{
					transform.createRotationXY(procPool.randomPlusMinusOne()*180.0, procPool.randomPlusMinusOne()*180.0);
					mol->applyTransform(box_, transform);
				}

				// Explore conformation space within the molecule by rotating bonds
				// TODO
			}
		}
	}

	// Set fractional populations in usedAtomTypes_
	usedAtomTypes_.finalise();


	/*
	 * Determine maximal extent of RDF (from origin to centre of box)
	 */
	Vec3<double> half = box()->axes() * Vec3<double>(0.5,0.5,0.5);
	double maxR = half.magnitude(), inscribedSphereRadius = box()->inscribedSphereRadius();
	Messenger::print("\n");
	Messenger::print("--> Maximal extent for g(r) is %f Angstrom (half cell diagonal distance).\n", maxR);
	Messenger::print("--> Inscribed sphere radius (maximum RDF range avoiding periodic images) is %f Angstroms.\n", inscribedSphereRadius);
	if (requestedRDFRange_ < -1.5)
	{
		Messenger::print("--> Using maximal non-minimum image range for g(r).\n");
		rdfRange_ = inscribedSphereRadius;
	}
	else if (requestedRDFRange_ < -0.5)
	{
		Messenger::print("--> Using 90%% of maximal extent for g(r).\n");
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


	/*
	 * Load coordinates if necessary
	 */
	// 1) If useOutputCoordinatesAsInput_ is true and that file exists, this overrides everything else
	// 2) If randomConfiguration_ is true, we will have already generated random coordinates earlier on
	// 3) Load the inputCoordinatesFile_
	if (useOutputCoordinatesAsInput_ && (!outputCoordinatesFile_.isEmpty()) && DUQSys::fileExists(outputCoordinatesFile_))
	{
		Messenger::print("--> Loading initial coordinates from output coordinates file '%s'...\n", outputCoordinatesFile_.get());
		LineParser inputFileParser(&procPool);
		if (!inputFileParser.openInput(outputCoordinatesFile_)) return false;
		if (!loadCoordinates(inputFileParser, "xyz")) return false;
		inputFileParser.closeFiles();
	}
	else if (randomConfiguration_)
	{
		// Accounted for earlier on
	}
	else if (inputCoordinatesFile_.isEmpty())
	{
		Messenger::error("Random configuration generation is off, and no input coordinates file has been provided.\n");
		return false;
	}
	else
	{
		Messenger::print("--> Loading initial coordinates from file '%s'...\n", inputCoordinatesFile_.get());
		LineParser inputFileParser(&procPool);
		if (!inputFileParser.openInput(inputCoordinatesFile_)) return false;
		if (!loadCoordinates(inputFileParser, inputCoordinatesFormat_)) return false;
		inputFileParser.closeFiles();
	}

	// Update Cell contents / Atom locations
	updateCellContents();


	/*
	 * Load or calculate Box normalisation file (if we need one)
	 */
	if (rdfRange_ <= inscribedSphereRadius)
	{
		Messenger::print("--> No need for Box normalisation array since rdfRange is within periodic range.\n");
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
		// Attempt to load existing Box normalisation file
		if (!boxNormalisationFileName_.isEmpty())
		{
			// Master will open file and attempt to read it...
			bool loadResult;
			if (procPool.isMaster()) loadResult = boxNormalisation_.load(boxNormalisationFileName_);
			if (!procPool.broadcast(loadResult)) return false;

			// Did we successfully load the file?
			if (loadResult)
			{
				Messenger::print("--> Successfully loaded box normalisation data from file '%s'.\n", boxNormalisationFileName_.get());
				if (!boxNormalisation_.broadcast(procPool)) return false;
			}
			else Messenger::print("--> Couldn't load Box normalisation data - it will be calculated.\n");
		}

		// Did we successfully load the file?
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
				Messenger::print("--> Calculating box normalisation array for g(r)...\n");
				if (!box()->calculateRDFNormalisation(procPool, boxNormalisation_, rdfRange_, rdfBinWidth_, boxNormalisationNPoints)) return false;
				
				// Save normalisation file so we don't have to recalculate it next time
				if (procPool.isMaster()) boxNormalisation_.save(boxNormalisationFileName_);
			}
		}

		// Interpolate the Box normalisation array
		boxNormalisation_.interpolate(XYData::LinearInterpolation);
	}

	return true;
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
Module* Configuration::addModule(Module* masterInstance, bool autoAddDependents)
{
	return modules_.addModule(masterInstance, moduleData_, autoAddDependents);
}

// Return number of Modules associated to this Configuration
int Configuration::nModules() const
{
	return modules_.nModules();
}

// Return list of Modules associated to this Configuration
RefList<Module,bool>& Configuration::modules()
{
	return modules_.modules();
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
bool Configuration::appendEnsemble()
{
	return appendEnsemble_;
}

// Return frequency at which to append ensemble
int Configuration::setEnsembleFrequency(int frequency)
{
	ensembleFrequency_ = frequency;
}

// Return frequency at which to append ensemble
int Configuration::ensembleFrequency()
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
		Messenger::printVerbose("--> Process rank %i is assembling coordinate data...\n", procPool.poolRank());
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
