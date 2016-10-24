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
#include "base/comms.h"
#include <string.h>

// RDFMethod keywords
const char* RDFMethodKeywords[] = { "Simple" };

// Convert text string to RDFMethod
Configuration::RDFMethod Configuration::rdfMethod(const char* s)
{
	for (int n=0; n<Configuration::nRDFMethods; ++n) if (strcmp(s,RDFMethodKeywords[n]) == 0) return (Configuration::RDFMethod) n;
	return Configuration::nRDFMethods;
}

// Convert RDFMethod to text string
const char* Configuration::rdfMethod(Configuration::RDFMethod rm)
{
	return RDFMethodKeywords[rm];
}

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
	rdfMethod_ = Configuration::SimpleMethod;

	// Setup
	rdfBinWidth_ = 0.025;
	rdfRange_ = -1.0;
	rdfSmoothing_ = 0;
	requestedRDFRange_ = -1.0;
	qDelta_ = 0.05;
	qMax_ = 50.0;
	temperature_ = 300.0;
	braggCalculationOn_ = false;
	braggMaximumQ_ = 1.0;
	braggBroadening_ = 0.0063;
}

// Destructor
Configuration::~Configuration()
{
	clear();
}

// Assignment operator
void Configuration::operator=(Configuration &source)
{
	msg.error("XXX CONFIGURATION COPY (via ASSIGNMENT OPERATOR) IS NOT YET IMPLEMENTED.\n");
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
// // // 				msg.error("Mismatch between size of Atom array in Configuration, and number of Atoms needed by Molecules (in assignment operator).\n");
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
// // // 				msg.error("Mismatch between size of Grain array in Configuration, and number of Grains needed by Molecules (in assignment operator).\n");
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
bool Configuration::setup(const List<AtomType>& atomTypes, double pairPotentialRange, int boxNormalisationNPoints)
{
	/*
	 * Order of business:
	 * 1) Check for a sensible multiplier for the system
	 * 2) Create all molecules in the system, based on the defined multiplier and Species
	 * 3) Add a periodic box (since we now know the total number of atoms in the system, and hence can work out the number density)
	 * 4) Create atom/grain arrays
	 * 5) Everything else...
	 */

	// Check Configuration multiplier
	if (multiplier_ < 1)
	{
		msg.error("Configuration multiplier is zero or negative (%i).\n", multiplier_);
		return false;
	}

	// Create 'empty' molecules in the main Configuration
	msg.print("--> Creating space for molecules...\n");
	int count;
	for (RefListItem<Species,double>* refSp = usedSpecies_.first(); refSp != NULL; refSp = refSp->next)
	{
		// For safety, only one process will determine the number of molecules of each component
		if (Comm.master()) count = refSp->data * multiplier_;
		if (!Comm.broadcast(&count, 1)) return false;

		// Check for zero count
		if (count == 0)
		{
			msg.error("Relative population for Species '%s' is too low (%e) to provide any Molecules in this Configuration.\n", refSp->item->name(), refSp->data);
			return false;
		}
		
		for (int n = 0; n < count; ++n) addMolecule(refSp->item);
	}

	// Create a Box
	msg.print("--> Creating periodic box...\n");
	if (!setupBox(pairPotentialRange))
	{
		msg.error("Failed to set-up Box/Cells for Configuration.\n");
		return false;
	}

	// Setup arrays for atoms and grains, based on the local molecules list
	msg.print("--> Setting up Atom/Grain arrays...\n");
	if (!setupArrays())
	{
		msg.error("Failed to set-up arrays in Configuration.\n");
		return false;
	}

	// Now, we need some coordinates - either we are creating a random configuration of molecules, or we are loading in a set of coordinates from a file
	Species sourceCoordinates;
	if (!randomConfiguration())
	{
		msg.print("--> Loading initial coordinates from file '%s'...\n", initialCoordinatesFile_.get());
		// Construct a temporary Species to load the source coordinates into
		if (!MPIRunMaster(sourceCoordinates.load(initialCoordinatesFile_.get()))) return false;
		sourceCoordinates.broadcast(atomTypes);

		// Species now contains some number of atoms - does it match the number in the configuration's molecules?
		if (nAtoms() != sourceCoordinates.nAtoms())
		{
			msg.error("Number of atoms in initial coordinates file (%i) does not match that in configuration (%i).\n", sourceCoordinates.nAtoms(), nAtoms());
			return false;
		}
	}

	// Create Atom and Grain arrays, and Molecule copies
	msg.print("--> Setting up Molecules, Atoms, and Grains...\n");
	if (!setupMolecules(sourceCoordinates))
	{
		msg.error("Failed to setup molecules.\n");
		return false;
	}

	// Initialise neighbour lists and update grains
	msg.print("--> Initialising Cell neighbour lists and Grains\n");
	if (!updateAtomsInCells()) return false;
	updateGrains();
	
	// Initialise cell atom neighbour lists
	recreateCellAtomNeighbourLists(pairPotentialRange);

	// Setup parallel comms / limits etc.
	msg.print("\n");
	msg.print("Setting up parallel comms...\n");

	// -- Assign Atom/Grain limits to processes
	if (!Comm.calculateLimits(nAtoms(), nGrains())) return false;

	// Construct RDF/S(Q) arrays
	// -- Determine maximal extent of RDF (from origin to centre of box)
	msg.print("\n");
	msg.print("Setting up RDF/S(Q) data...\n");
	Vec3<double> half = box()->axes() * Vec3<double>(0.5,0.5,0.5);
	double maxR = half.magnitude(), inscribedSphereRadius = box()->inscribedSphereRadius();
	msg.print("--> Maximal extent for RDFs is %f Angstrom (half cell diagonal distance).\n", maxR);
	msg.print("--> Inscribed sphere radius (maximum RDF range avoiding periodic images) is %f Angstroms.\n", inscribedSphereRadius);
	if (requestedRDFRange_ < -1.5)
	{
		msg.print("--> Using maximal non-minimum image range for RDF/S(Q).\n");
		rdfRange_ = inscribedSphereRadius;
	}
	else if (requestedRDFRange_ < -0.5)
	{
		msg.print("--> Using 90%% of maximal extent for RDF/S(Q).\n");
		rdfRange_ = 0.90*maxR;
	}
	else
	{
		msg.print("--> Specific RDF range supplied (%f Angstroms).\n", requestedRDFRange_);
		rdfRange_ = requestedRDFRange_;
		if (rdfRange_ < 0.0)
		{
			msg.error("Negative RDF range requested.\n");
			return false;
		}
		else if (rdfRange_ > maxR)
		{
			msg.error("Requested RDF range is greater then the maximum possible extent for the Box.\n");
			return false;
		}
		else if (rdfRange_ > (0.90*maxR)) msg.warn("Requested RDF range is greater than 90%% of the maximum possible extent for the Box. FT may be suspect!\n");
	}
	// 'Snap' rdfRange_ to nearest bin width...
	rdfRange_ = int(rdfRange_/rdfBinWidth_) * rdfBinWidth_;
	msg.print("--> RDF range (snapped to bin width) is %f Angstroms.\n", rdfRange_);

	// Does a box normalisation already exist (i.e. has been loaded)
	// Create/load Box normalisation array
	if (!boxNormalisationFileName_.isEmpty())
	{
		if (boxNormalisation_.load(boxNormalisationFileName_))
		{
			msg.print("--> Successfully loaded box normalisation data from file '%s'.\n", boxNormalisationFileName_.get());
			boxNormalisation_.interpolate();
		}
		else msg.print("--> Couldn't load Box normalisation data - it will be calculated.\n");
	}
	if (boxNormalisation_.nPoints() <= 1)
	{
		// Only calculate if RDF range is greater than the inscribed sphere radius
		if (rdfRange_ <= inscribedSphereRadius)
		{
			msg.print("--> No need to calculate Box normalisation array since rdfRange is within periodic range.\n");
			boxNormalisation_.clear();
			double x = rdfBinWidth_*0.5;
			while (x < rdfRange_)
			{
				boxNormalisation_.addPoint(x, 1.0);
				x += rdfBinWidth_;
			}
			boxNormalisation_.interpolate();
		}
		else
		{
			msg.print("--> Calculating box normalisation array for RDFs...\n");
			if (!box()->calculateRDFNormalisation(boxNormalisation_, rdfRange_, rdfBinWidth_, boxNormalisationNPoints)) return false;
		}
	}
	msg.print("\n");
	msg.print("--> Constructing RDF/F(Q) lists and matrices...\n");
	if (!setupPartials())
	{
		msg.error("Failed to create RDF/F(Q) lists/matrices.\n");
		return false;
	}

	// Prepare Samples
	msg.print("\n");
	msg.print("Preparing Samples...\n");
	if (!setupReferenceSamples())
	{
		msg.error("Failed to setup reference Samples.\n");
		return false;
	}

	return true;
}

/*
 * Parallel Comms
 */

// Broadcast coordinates
bool Configuration::broadcastCoordinates()
{
#ifdef PARALLEL
	double* x, *y, *z;
	x = new double[nAtoms_];
	y = new double[nAtoms_];
	z = new double[nAtoms_];
	
	// Master assembles Atom coordinate arrays...
	if (Comm.master())
	{
		msg.print("--> Sending Configuration to slaves...\n");
		for (int n=0; n<nAtoms_; ++n)
		{
			x[n] = atomReferences_[n]->r().x;
			y[n] = atomReferences_[n]->r().y;
			z[n] = atomReferences_[n]->r().z;
		}
	}
	else msg.print("--> Waiting for Master to send Configuration.\n");

	if (!Comm.broadcast(x, nAtoms_)) return false;
	if (!Comm.broadcast(y, nAtoms_)) return false;
	if (!Comm.broadcast(z, nAtoms_)) return false;
	
	// Slaves then store values into Atoms...
	if (Comm.slave()) for (int n=0; n<nAtoms_; ++n) atomReferences_[n]->setCoordinates(x[n], y[n], z[n]);

	delete[] x;
	delete[] y;
	delete[] z;
	
	// Broadcast coordinateIndex_
	if (!Comm.broadcast(&coordinateIndex_, 1)) return false;
#endif
	return true;
}

