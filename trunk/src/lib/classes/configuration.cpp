/*
	*** Configuration
	*** src/lib/classes/configuration.cpp
	Copyright T. Youngs 2012-2014

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
#include "classes/molecule.h"
#include "classes/species.h"
#include "math/matrix3.h"
#include "base/comms.h"
#include <string.h>
#include <new>
using namespace std;

/*!
 * \brief Constructor
 * \details Constructor for Configuration. 
 */
Configuration::Configuration() : ListItem<Configuration>()
{
	// Contents
	atoms_ = NULL;
	grains_ = NULL;
	nAtoms_ = 0;
	nGrains_ = 0;

	// Box / Cells
	box_ = NULL;
	cells_ = NULL;
	cellFlag_ = NULL;
	nCells_ = 0;
	changeCount_ = 0;
}

/*!
 * \brief Destructor
 * \details Destructor for Configuration. 
 */
Configuration::~Configuration()
{
	clear();
}

/*!
 * \brief Assignment operator
 */
void Configuration::operator=(Configuration &source)
{
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
// 	changeCount_ = source.changeCount_;
// 
// 	return;
}

/*!
 * \brief Clear data
 */
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

/*
// Contents
*/

/*!
 * \brief Add space for a Molecule of the Species provided
 * Add an empty Molecule to the Configuration. The Molecule will contain array space for referencing the number of Atoms and Grains
 * defined in the supplied Species. No actual Atom or Grain information is created in this function (this is done by Molecule::instantiate()).
 */
void Configuration::addMolecule(Species* sp)
{
#ifdef CHECKS
	// Check for NULL pointer
	if (sp == NULL)
	{
		msg.error("NULL_POINTER - NULL Species pointer given to Configuration::addMolecule().\n");
		return;
	}
#endif
	Molecule* mol = molecules_.add();
	mol->initialise(sp, molecules_.nItems()-1);
	nGrains_ += mol->nGrains();
	nAtoms_ += mol->nAtoms();
}

/*!
 * \brief Return number of Molecules
 */
int Configuration::nMolecules() const
{
	return molecules_.nItems();
}

/*!
 * \brief Return first Molecule
 */
Molecule* Configuration::molecules()
{
	return molecules_.first();
}

/*!
 * \brief Return specified molecule
 */
Molecule* Configuration::molecule(int id)
{
#ifdef CHECKS
	static Molecule dummy;
	if ((id < 0) || (id >= molecules_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Molecule index %i passed to Configuration::molecule() is out of range (nMolecules = %i).\n", id, molecules_.nItems());
		return &dummy;
	}
#endif	
	return molecules_[id];
}

/*!
 * \brief Return number of grains
 */
int Configuration::nGrains() const
{
	return nGrains_;
}

/*!
 * \brief Return grain array
 */
Grain* Configuration::grains()
{
	return grains_;
}

/*!
 *  \brief Return nth grain
 */
Grain& Configuration::grain(int n)
{
#ifdef CHECKS
	static Grain dummy;
	if ((n < 0) || (n >= nGrains_))
	{
		msg.print("OUT_OF_RANGE - Grain index %i passed to Configuration::grain() is out of range (nGrains_ = %i).\n", n, nGrains_);
		return dummy;
	}
#endif
	return grains_[n];
}

/*!
 * \brief Return number of atoms
 */
int Configuration::nAtoms() const
{
	return nAtoms_;
}

/*!
 * \brief Return atom array
 */
Atom* Configuration::atoms()
{
	return atoms_;
}

/*!
 * \brief Return nth atom
 */
Atom* Configuration::atom(int n)
{
#ifdef CHECKS
	if ((n < 0) || (n >= nAtoms_))
	{
		msg.print("OUT_OF_RANGE - Atom index %i passed to Configuration::atom() is out of range (nAtoms_ = %i).\n", n, nAtoms_);
		return NULL;
	}
#endif
	return &atoms_[n];
}

/*!
 * \brief Setup Arrays
 * \details Allocates atom and grain arrays
 */
bool Configuration::setupArrays()
{
	if (molecules_.nItems() == 0)
	{
		msg.error("Configuration::setupArrays - No Molecules have been added to the Configuration - no arrays to be created.\n");
		return false;
	}
	if (nAtoms_== 0)
	{
		msg.error("Configuration::setupArrays - Molecules have been added to the Configuration, but no Atoms were counted.\n");
		return false;
	}
	if (nGrains_ == 0)
	{
		msg.error("Configuration::setupArrays - Molecules have been added to the Configuration, but no Grains were counted.\n");
		return false;
	}
	
	msg.print("--> Creating arrays for %i atoms and %i grains...\n", nAtoms_, nGrains_);
	try
	{
		atoms_ = new Atom[nAtoms_];
		grains_ = new Grain[nGrains_];
	}
	catch (bad_alloc& alloc)
	{
		msg.error("Configuration::setupArrays - Failed to allocate sufficient memory. Exception was : %s\n", alloc.what());
		return false;
	}

	// Setup grain indices
	for (int n=0; n<nGrains_; ++n) grains_[n].setIndex(n);

	return true;
}

/*!
 * \brief Setup Molecules
 * \details Once molecules have been added to the Configuration with addMolecule(), and the master arrays of atoms and grains have
 * been allocated by setupArrays(), the coordinates of the individual atoms can be set. If atoms are present in the supplied Species, setupMolecules()
 * takes coordinates from there and copies them to the molecules defined in the configuration. Otherwise, a random centre of geometry and orientation is
 * generated for each molecule.
 */
bool Configuration::setupMolecules(Species& sourceCoordinates)
{
	// If there are no atoms in the sourceCoordinates species, assume that we are creating a random configuration.
	int atomCount = 0, grainCount = 0;
	Cell* c;
	Matrix3 transform;
	Vec3<double> r, cog, newCentre;
	bool randomising = sourceCoordinates.nAtoms() == 0;
	Species* sp;
	SpeciesAtom* sourceI = sourceCoordinates.atoms();

	// Initialise a shared random number pool - this will ensure all processes generate the same coordinates
	Comm.initialiseRandomBuffer(DUQComm::World);

	// Loop over defined molecules
	for (Molecule* mol = molecules_.first(); mol != NULL; mol = mol->next)
	{
		// Grab pointer to parent species
		sp = mol->species();

		// If we are randomising the configuration, get the centre of geometry for the species and generate a random centre and rotation matrix
		if (randomising)
		{
			// Calculate the centre of geometry of the molecule's source species
			cog = mol->species()->centreOfGeometry(box_);

			// Generate a new random centre of geometry for the molecule
			newCentre = box_->randomCoordinate();
			
			// Generate a random rotation matrix
			transform.createRotationXY(Comm.randomPlusMinusOne()*180.0, Comm.randomPlusMinusOne()*180.0);
		}

		// Atoms
		SpeciesAtom* i = mol->species()->atoms();
		for (int n = 0; n<sp->nAtoms(); ++n)
		{
			// Get / generate atom coordinate
			if (randomising) r = (transform * (i->r() - cog)) + newCentre;
			else
			{
				r = sourceI->r();
				sourceI = sourceI->next;
			}

			// Get cell location of atom, and find an empty atom slot in that cell
			c = cell(r);

			// Set the master atom reference
			atoms_[atomCount].setIndex(atomCount);
			atoms_[atomCount].setCoordinatesNasty(r);
			atoms_[atomCount].setCell(c);
			if (!c->addAtom(&atoms_[atomCount])) return false;

			// Set atom pointer and character (charge, atomtype etc.) of atom in molecule from the source Species
			if (!mol->setupAtom(n, &atoms_[atomCount], i)) return false;
			
			++atomCount;
			i = i->next;
		}

		// Grains
		for (int n = 0; n<mol->nGrains(); ++n)
		{
			if (grainCount >= nGrains_)
			{
				msg.error("Mismatch between size of grain array in configuration, and number of grains needed by molecules.\n");
				return false;
			}
			if (!mol->setupGrain(n, &grains_[grainCount], sp->grain(n))) return false;

			++grainCount;
		}

	}

	return true;
}

/*!
 * \brief Return current change count
 */
int Configuration::changeCount()
{
	return changeCount_;
}

/*!
 * \brief Increment current change count
 */
void Configuration::incrementChangeCount()
{
	++changeCount_;
}

/*
// Box and Cells
*/

/*!
 * \brief Return Box
 */
const Box* Configuration::box() const
{
	return box_;
}

/*!
 * \brief Setup periodic Box
 */
bool Configuration::setupBox(double ppRange, Vec3<double> relativeLengths, Vec3<double> angles, double atomicDensity, bool nonPeriodic)
{
	// Remove old box if present
	if (box_ != NULL)
	{
		msg.print("Removing existing box definition...\n");
		delete box_;
	}

	// Determine volume
	double volume = nAtoms_ / atomicDensity;

	// Determine box type from supplied lengths / angles
	bool rightAlpha = (fabs(angles.x-90.0) < 0.001);
	bool rightBeta = (fabs(angles.y-90.0) < 0.001);
	bool rightGamma = (fabs(angles.z-90.0) < 0.001);
	if (nonPeriodic)
	{
		// Might need to increase box volume to accommodate three times the ppRange
		if (volume < pow(ppRange*3.0, 3.0)) volume = pow(ppRange*3.0, 3.0);
		box_ = new NonPeriodicBox(volume);
	}
	else if (rightAlpha && rightBeta && rightGamma)
	{
		// Cubic or orthorhombic
		bool abSame = (fabs(relativeLengths.x-relativeLengths.y) < 0.0001);
		bool acSame = (fabs(relativeLengths.x-relativeLengths.z) < 0.0001);
		if (abSame && acSame) box_ = new CubicBox(volume);
		else box_ = new OrthorhombicBox(volume, relativeLengths);
	}
	else if (rightAlpha && (!rightBeta) && rightGamma) box_ = new MonoclinicBox(volume, relativeLengths, angles.y);	// Monoclinic
	else
	{
		// Triclinic
		box_ = new TriclinicBox(volume, relativeLengths, angles);
	}
	
	msg.print("--> %s xox created for system.\n", Box::boxType(box_->type()));
	Matrix3 axes = box_->axes();
	msg.print("--> Axes Matrix : A = %10.4e %10.4e %10.4e, length = %10.4e Angstroms\n", axes[0], axes[1], axes[2], box_->axisLength(0));
	msg.print("-->               B = %10.4e %10.4e %10.4e, length = %10.4e Angstroms\n", axes[3], axes[4], axes[5], box_->axisLength(1));
	msg.print("-->               C = %10.4e %10.4e %10.4e, length = %10.4e Angstroms\n", axes[6], axes[7], axes[8], box_->axisLength(2));
	
	// Check cell lengths against pair potential range
	if (ppRange > box_->inscribedSphereRadius())
	{
		msg.error("PairPotential range (%f) is longer than the shortest non-minimum image distance (%f).\n", ppRange, box_->inscribedSphereRadius());
		return false;
	}

	generateCells(5.0, ppRange, atomicDensity);
	 
	return true;
}

/*!
 * \brief Clear Cell arrays
 */
void Configuration::clearCells()
{
	if (cells_ != NULL) delete[] cells_;
	cells_ = NULL;
	if (cellFlag_ != NULL) delete[] cellFlag_;
	cellFlag_ = NULL;
	nCells_ = 0;
}

/*!
 * \brief Return whether the two Cells should be mimd in calculations
 * \details Since we partition the simulation Box up into subcells, for speed we can determine and store whether any
 * minimum image operations are required between the contents of the two cells. If *twice* the difference between any 
 * component of any grid reference is greater than or equal to the value of the number of Cells along the relevant side
 * then minimum image calculations should be performed. A secondary check is made to account for small systems, in which
 * the Cell 'b' is mirrored onto the opposite side of the Cell 'a' - if the resulting difference, minus one, between
 * gridReference coordinates is greater than or equal to the cellExtent in any direction, again minimum image must
 * be performed.
 */
bool Configuration::minimumImageRequired(Cell* a, Cell* b) const
{
#ifdef CHECKS
	// Check for NULL cell pointers
	if (a == NULL)
	{
		msg.error("NULL_POINTER - NULL Cell pointer 'a' given to Configuration::imagesNeeded().\n");
		return false;
	}
	if (b == NULL)
	{
		msg.error("NULL_POINTER - NULL Cell pointer 'b' given to Configuration::imagesNeeded().\n");
		return false;
	}
#endif
	// Never require images for the same Cell
	if (a == b) return false;

	// Do straight check on integer Cell distance
	Vec3<int> u = a->gridReference() - b->gridReference();
	if (abs(u.x)*2 >= divisions_.x) return true;
	if (abs(u.y)*2 >= divisions_.y) return true;
	if (abs(u.z)*2 >= divisions_.z) return true;

	// Second check - mirrored Cell 'b'
	u.x -= DUQMath::sgn(u.x) * divisions_.x;
	u.y -= DUQMath::sgn(u.y) * divisions_.y;
	u.z -= DUQMath::sgn(u.z) * divisions_.z;
	if (abs(u.x)-1 <= cellExtents_.x) return true;
	if (abs(u.y)-1 <= cellExtents_.y) return true;
	if (abs(u.z)-1 <= cellExtents_.z) return true;
	return false;
}

// Generate Cells for Box
bool Configuration::generateCells(double cellSize, double pairPotentialRange, double atomicDensity)
{
	// There are some conditions to the partitioning:
	// 1) We require a minimum number of cells (minCellsPerSide) per box side
	// 2) TODO Need to check distances properly for monoclinic and triclinic cells
	// 3) Require Cells to be roughly 'cubic' as far as is possible.
	
	const int minCellsPerSide = 3;
	const double tolerance = 0.01;
	int n, m, x, y, z;

	msg.print("--> Generating cells for box - minimum cells per side is %i, cell size is %f...\n", minCellsPerSide, cellSize);

	// Get Box axis lengths and divide through by cellSize
	Vec3<double> boxLengths(box_->axisLength(0), box_->axisLength(1),  box_->axisLength(2));
	Vec3<int> divisions(boxLengths.x / cellSize, boxLengths.y / cellSize, boxLengths.z / cellSize);
	int minEl;
	divisions_.zero();
	realCellSize_.zero();

	msg.print("--> Initial divisions based on cell size are (x,y,z) = (%i,%i,%i)\n", divisions.x, divisions.y, divisions.z);
	
	// How does the smalles length compare with the PairPotential range?
	if (divisions.min() < minCellsPerSide)
	{
		msg.print("Warning: Box size only allows for %i whole divisions of the cell size (%f) along one or more axes, while we require at least %i.\n", divisions.min(), cellSize, minCellsPerSide);
		
		// We must now take the shortest box length and divide by 3 to get the absolute maximum length to use on that side
		minEl = boxLengths.minElement();
		realCellSize_[minEl] = boxLengths[minEl] / minCellsPerSide;
		divisions_[minEl] = minCellsPerSide;
	}
	else
	{
		// All Box lengths are long enough to be partitioned in to at least minCellsPerSide
		// Just find shortest of them and set its exact distance
		minEl = boxLengths.minElement();
		realCellSize_[minEl] = boxLengths[minEl] / divisions[minEl];
		divisions_[minEl] = divisions[minEl];
	}
	
	msg.print("--> Shortest side (axis %i) will have cell length of %f Angstroms.\n", minEl, realCellSize_[minEl]);

	// Now, set our other cellLengths_ based on the minimum value we have just set
	// We try to get all lengths as similar as possible
	for (n=1; n<3; ++n)
	{
		int el = (minEl+n)%3;
		double x = boxLengths[el] / realCellSize_[minEl];
		double remainder = x - int(x);
		
		// If we're within a tolerance to the nearest integer, use this number....
		// If not, we can round 'x' to the nearest integer, but can only round up if ppRange allows us to...
		if (remainder > (1.0-tolerance))
		{
			divisions_[el] = int(x) + 1;
			realCellSize_[el] = boxLengths[el] / divisions_[el];
			msg.print("--> Accepted cell length of %f Angstroms (%i divisions) for axis %i, since it was within tolerance (-%e).\n", realCellSize_[minEl], divisions_[el], el, remainder);
		}
		else if (remainder < tolerance)
		{
			divisions_[el] = int(x);
			realCellSize_[el] = boxLengths[el] / divisions_[el];
			msg.print("--> Accepted cell length of %f Angstroms (%i divisions) for axis %i, since it was within tolerance (+%e).\n", realCellSize_[minEl], divisions_[el], el, remainder);
		}
		else if (remainder < 0.5)
		{
			// Can't fit more than half another cell in, so reduce number of divisions...
			divisions_[el] = int(x);
			realCellSize_[el] = boxLengths[el] / divisions_[el];
			msg.print("--> Decreased cell length for axis %i to %f Angstroms (%i divisions).\n", el, realCellSize_[el], divisions_[el]);
		}
		else
		{
			// Can fit more than half another Cell in - can we increase to the next integer?
			divisions_[el] = int(x) + 1;
			realCellSize_[el] = boxLengths[el] / divisions_[el];
			if (realCellSize_[el] < cellSize)
			{
				--divisions_[el];
				realCellSize_[el] = boxLengths[el] / divisions_[el];
				msg.print("--> Forced decrease of cell length for axis %i to %f Angstroms (%i divisions) since increasing it gave a length larger than the cell size.\n", el, realCellSize_[el], divisions_[el]);
			}
			else msg.print("--> Increased cell length for axis %i to %f Angstroms (%i divisions).\n", el, realCellSize_[el], divisions_[el]);
		}
	}

	// Summarise
	cellSize_.set(1.0 / divisions_.x, 1.0 / divisions_.y, 1.0 / divisions_.z);
	msg.print("--> Final cell partitioning is (x,y,z) = (%i,%i,%i), giving %i cells in total.\n", divisions_.x, divisions_.y, divisions_.z, divisions_.x*divisions_.y*divisions_.z);
	msg.print("--> Fractional cell size is (%f,%f,%f).\n", cellSize_.x, cellSize_.y, cellSize_.z);

	// Construct Cell arrays
	clearCells();
	nCells_ = divisions_.x*divisions_.y*divisions_.z;
	cellFlag_ = new bool[nCells_];
	msg.print("--> Constructing array of %i cells...\n", nCells_);
	cells_ = new Cell[nCells_];
	Vec3<double> fracCentre_(cellSize_.x*0.5, 0.0, 0.0);
	int count = 0;
	for (x = 0; x<divisions_.x; ++x)
	{
		fracCentre_.y = cellSize_.y*0.5;
		for (y = 0; y<divisions_.y; ++y)
		{
			fracCentre_.z = cellSize_.z*0.5;
			for (z = 0; z<divisions_.z; ++z)
			{
				cells_[count].setIndex(count);
				cells_[count].setGridReference(x,y,z);
				cells_[count].setCentre(box_->fracToReal(fracCentre_));
				fracCentre_.z += cellSize_.z;
				++count;
			}
			fracCentre_.y += cellSize_.y;
		}
		fracCentre_.x += cellSize_.x;
	}

	// Construct Cell neighbour lists
	msg.print("--> Creating cell neighbour lists...\n");
	// Make a list of integer vectors which we'll then use to pick Cells for the neighbour lists
	Vec3<double> r;
	Matrix3 cellAxes = box_->axes();
	cellAxes.applyScaling(cellSize_.x, cellSize_.y, cellSize_.z);
	cellExtents_.zero();

	// First, establish a maximal extent in principal directions...
	for (n=0; n<3; ++n)
	{
		do
		{
			r.zero();
			++cellExtents_[n];
			r[n] = cellExtents_[n];
			r = cellAxes * r;
		} while (r[n] < pairPotentialRange);
	}
	msg.print("--> Cell extents required to cover PairPotential range are (x,y,z) = (%i,%i,%i).\n", cellExtents_.x, cellExtents_.y, cellExtents_.z);
	
	// Check Cell extents, comparing with the actual number of Cells available in each direction.
	for (n=0; n<3; ++n)
	{
		if ((cellExtents_[n]*2+1) > divisions_[n])
		{
			msg.warn("--> Cells required along axis %i is %i (2*%i + 1) exceeds number of available cells (%i). Parallelism will be affected!\n", n, cellExtents_[n]*2+1, cellExtents_[n], divisions_[n]);
			// We do not decrease the value of cellExtents_, even though there are not enough cells along one or more sides of the box to satisfy that required for the pairPotentialRange.
			// When constructing, the loops below check the negative cellExtents_ indices for overlap which would cause the same cell to be added to the list twice.
		}
	}

	// Now, loop over cubic matrix of cellExtents_ and construct list of gridReferences within range
	// In case the number of cells required to cover the pairpotential range exceeds the number across the box, we need to check
	// for and trim off negative indices which would in reality be equivalent to a folded, positive index.
	List< ListVec3<int> > nbrs;
	RefList<Cell,int> cellNbrs;
	Cell* nbr;
	for (x=-cellExtents_.x; x<=cellExtents_.x; ++x)
	{
		// Check for extent exceeding available cells across box in x-direction
// 		if ((x < 0) && (x+divisions_[0] <=cellExtents_.x)) continue;

		for (y=-cellExtents_.y; y<=cellExtents_.y; ++y)
		{
			// Check for extent exceeding available cells across box in y-direction
// 			if ((y < 0) && (y+divisions_[1] <= cellExtents_.y)) continue;

			for (z=-cellExtents_.z; z<=cellExtents_.z; ++z)
			{
				// Check for extent exceeding available cells across box in z-direction
// 				if ((z < 0) && (z+divisions_[2] <=cellExtents_.z)) continue;

				if ((x == 0) && (y == 0) && (z == 0)) continue;
				// Set the grid reference of the cell to check, but reduce the extent by one
				// unit towards the central box since we need to check the distance of the closest
				// edge to this central Cell.
				r.set(x - DUQMath::sgn(x), y - DUQMath::sgn(y), z - DUQMath::sgn(z));
				r = cellAxes * r;
				if (r.magnitude() < pairPotentialRange)
				{
					// Check that the cell is not already in the list by querying the cellNbrs RefList
					nbr = cell(x, y, z);
					if (cellNbrs.contains(nbr)) continue;
					nbrs.add()->set(x, y, z);
					cellNbrs.add(nbr);
				}
			}
		}
	}
	msg.print("--> Added %i Cells to representative neighbour list.\n", nbrs.nItems());

	// Finally, loop over Cells and set neighbours, and construct neighbour matrix
	msg.print("--> Constructing neighbour lists for individual Cells...\n");
	bool mimRequired;
	OrderedList<Cell> neighbours, mimNeighbours;
	Vec3<int> gridRef;
	for (n=0; n<nCells_; ++n)
	{
		// Grab grid reference of central cell
		gridRef = cells_[n].gridReference();

		// Clear neighbour lists
		neighbours.clear();
		mimNeighbours.clear();

		// Loop over list of (relative) neighbour cell indices
		for (ListVec3<int>* item = nbrs.first(); item != NULL; item = item->next)
		{
			// Retrieve Cell pointers
			nbr = cell(gridRef.x+item->x, gridRef.y+item->y, gridRef.z+item->z);
			mimRequired = box_->type() == Box::NonPeriodicBox ? false : minimumImageRequired(&cells_[n], nbr);
			if (mimRequired) mimNeighbours.add(nbr);
			else neighbours.add(nbr);
		}

		// Setup lists in the cell
		cells_[n].addCellNeighbours(neighbours, mimNeighbours, nCells_);
	}

	// Send Cell info to Comm so suitable parallel strategy can be deduced
	if (!Comm.setupStrategy(divisions_, cellExtents_, nbrs)) return false;   // TODO Move to setupComms()?

	return true;
}

/*!
 * \brief Return number of Cells for box
 */
int Configuration::nCells() const
{
	return nCells_;
}

/*!
 * \brief Return real Cell dimensions
 */
Vec3<double> Configuration::realCellSize() const
{
	return realCellSize_;
}

/*!
 * \brief Retrieve Cell with (wrapped) grid reference specified
 */
Cell* Configuration::cell(int x, int y, int z) const
{
	x = x%divisions_.x;
	if (x < 0) x += divisions_.x;
	y = y%divisions_.y;
	if (y < 0) y += divisions_.y;
	z = z%divisions_.z;
	if (z < 0) z += divisions_.z;
	return &cells_[x*divisions_.y*divisions_.z + y*divisions_.z + z];
}

/*!
 * \brief Retrieve Cell with id specified
 */
Cell* Configuration::cell(int id) const
{
#ifdef CHECKS
	if ((id < 0) || (id >= nCells_))
	{
		msg.print("OUT_OF_RANGE - Cell ID %i is out of range (nCells = %i)\n.", id, nCells_);
		return 0;
	}
#endif
	return &cells_[id];
}

/*!
 * \brief Return Cell which contains specified coordinate
 */
Cell* Configuration::cell(const Vec3<double> r) const
{
	Vec3<double> foldFracR = box_->foldFrac(r);
	Vec3<int> indices;
	indices.x = foldFracR.x / cellSize_.x;
	indices.y = foldFracR.y / cellSize_.y;
	indices.z = foldFracR.z / cellSize_.z;
	indices.x %= divisions_.x;
	indices.y %= divisions_.y;
	indices.z %= divisions_.z;
	
	return &cells_[indices.x*divisions_.y*divisions_.z + indices.y*divisions_.z + indices.z];
}

/*!
 * \brief Return whether two Cells need minimum image calculation
 */
bool Configuration::useMim(Cell* a, Cell* b) const
{
#ifdef CHECKS
	// Check for NULL pointers
	if (a == NULL)
	{
		msg.error("NULL_POINTER - NULL Cell pointer 'a' passed to Configuration::useMim().\n");
		return false;
	}
	if (b == NULL)
	{
		msg.error("NULL_POINTER - NULL Cell pointer 'b' passed to Configuration::useMim().\n");
		return false;
	}
#endif
	return minimumImageRequired(a, b);
}

/*!
 * \brief Initialise Cells for distribution
 */
void Configuration::initialiseCellDistribution()
{
	nCellsDistributed_ = 0;
	lastCellDistributed_ = -1;
	for (int n=0; n<nCells_; ++n)
	{
		// Clear usage flag, and remove any remaining locks
		while (cells_[n].lockCount() > 0) cells_[n].removeLock();
		cellFlag_[n] = false;
	}
}

/*!
 * \brief Return next available Cell for calculation
 * \details All processes should call here together. The master process will distribute the index of the next available Cell to each process.
 * If all Cells have been distributed once then Cell::AllCellsComplete is returned to all processes. Otherwise, the next available 'unused' Cell index
 * is returned. If the routine runs out of available cells during a distribution cycle, then what is returned is determined by the value of
 * 'allowRepeats'. If 'allowRepeats' is true then Cells which have already been sent once will be sent again, in order for all processes to be
 * doing useful work at all times. If 'allowRepeats' is false, then repeat calculations on Cells are not allowed, and NoCellsAvailable is returned
 * instead. The latter is relevant when property calculation is being performed, and each Cell must be subjected to calculation once and once only.
 * 
 * Any routine which employs Cell distribution in this way implicitly operates in parallel with process groups.
 */
int Configuration::nextAvailableCell(bool willBeModified, bool allowRepeats)
{
#ifdef PARALLEL
	int proc, group,m, cellId, rootCellId = -1;
	int* procList;
	if (Comm.master())
	{
		msg.printVerbose("NextAvailableCell: nCellsDistributed_ = %i\n", nCellsDistributed_);
		// All cells already distributed?
		if (nCellsDistributed_ == nCells_)
		{
			for (int n=1; n<Comm.nProcesses(); ++n) if (!Comm.send(Cell::AllCellsComplete, n)) printf("MPI error in Configuration::nextAvailableCell() when all cells were distributed.\n");
			return Cell::AllCellsComplete;
		}
		
		// There are still Cells which haven't been worked on yet, so find and distribute them.
		for (group = 0; group<Comm.nProcessGroups(); ++group)
		{
			msg.printVerbose("Finding next Cell for process group %i\n", group);

			// Loop from 1 to nCells_
			for (m=1; m<=nCells_; ++m)
			{
				// Get (wrapped) Cell index and check availability
				cellId = (lastCellDistributed_+m)%nCells_;
				if ((cells_[cellId].lockCount() != 0) || (willBeModified && !cells_[cellId].canLock()))
				{
					// Reset cellId and continue
					cellId = Cell::NoCellsAvailable;
					continue;
				}

				// If we have not distributed all cells...
				if (nCellsDistributed_ < nCells_)
				{
					// ... then we want one we've not yet used, and which isn't currently locked by another process
					if (cellFlag_[cellId]) continue;
					
					msg.printVerbose("Cell id %i will be sent to process group %i.\n", cellId, group);
					++nCellsDistributed_;
					lastCellDistributed_ = cellId;
					break;
				}
				else if (allowRepeats)
				{
					// ... otherwise if 'allowRepeats' we're allowed to send an unlocked one which has already been used
					msg.printVerbose("Cell id %i will be sent to process group %i (although it has already been used).\n", cellId, group);
					break;
				}
				
				// Reset id if we get here...
				cellId = Cell::NoCellsAvailable;
			}
			
			// Did we find a suitable Cell?
			if (cellId == Cell::NoCellsAvailable)
			{
				msg.printVerbose("Warning: Couldn't find valid Cell to send to process group %i. Probably an odd number of Cells and an even number of process groups.\n", group);
			}
			else
			{
				// Lock the Cell and set its flag
				cells_[cellId].lock(willBeModified);
				cellFlag_[cellId] = true;
			}
			
			// Broadcast Cell index to all processes in current ProcessGroup
			procList = Comm.processes(group);
			for (proc=0; proc<Comm.nProcesses(group); ++proc)
			{
				if (procList[proc] == 0) rootCellId = cellId;
				else if (!Comm.send(cellId, procList[proc])) printf("MPI error in Configuration::nextAvailableCell() sending next cell to process %i.\n", procList[proc]);
			}
		}

		// All done - return our own cellId
		return rootCellId;
	}
	else
	{
		// Slaves just receive the next Cell index
		if (!Comm.receive(cellId, 0)) printf("MPI error in Configuration::nextAvailableCell() when all cells were distributed.\n");
		msg.printVerbose("Slave with rank %i received Cell id %i\n", Comm.rank(), cellId);
		return cellId;
	}
#else
	// If we have distributed all Cells already, return -1
	if (nCellsDistributed_ == nCells_) return Cell::AllCellsComplete;

	++lastCellDistributed_;
	++nCellsDistributed_;
	cells_[lastCellDistributed_].lock(willBeModified);
	cellFlag_[lastCellDistributed_] = true;

	return lastCellDistributed_;
#endif
}

/*!
 * \brief Unlock Cell specified, once calculation is complete
 * \details All processes should call this routine once they have finished performing manipulations of the Cell they were given from
 * Configuration::nextAvailableCell(), passing the cellId as the single argument.
 */
bool Configuration::finishedWithCell(bool willBeModified, int cellId)
{
#ifdef PARALLEL
	if (Comm.master())
	{
		// Receive all used cellId's from process group leaders
		for (int group=0; group<Comm.nProcessGroups(); ++group)
		{
			// If this is *not* the masters group, receive data from the slave process leader
			if (group != Comm.localGroupIndex())
			{
				if (!Comm.receive(cellId, Comm.processes(group)[0])) msg.print("MPI error in Configuration::finishedWithCell() receiving from process with rank %i.\n", group);
			}
			if (cellId >= 0) cells_[cellId].unlock(willBeModified);
		}
	}
	else
	{
		// Slaves just send their id to the master, provided they are process group leader
		if (Comm.processGroupLeader())
		{
			if (!Comm.send(cellId, 0)) msg.print("MPI error in Configuration::finishedWithCell().\n");
		}
	}
	
	// Collect all processes together
	if (!Comm.wait(DUQComm::World)) return false;
#else
	if (cellId >= 0) cells_[cellId].unlock(willBeModified);
#endif
	return true;
}

/*!
 * \brief Update cell locations of all atoms
 */
bool Configuration::updateAtomsInCells()
{
	// Fold the coordinates of each atom into the box, and then check its cell location, moving if necessary.
	// Loop over cells, focussing on one atom index at a time. Do it this way to try and avoid moving large numbers of atoms into one cell at once, causing an overflow.
	int n, atomId = 0;
	Cell* currentCell, *targetCell;
	Vec3<double> foldedR;
	Atom* i;
	for (int c = 0; c < nCells_; ++c)
	{
		// Grab cell pointer
		currentCell = &cells_[c];

		// TODO Overload cell() to take a pointer to a Vec3<> in which the folded r can be returned
		for (OrderedListItem<Atom>* item = currentCell->atoms().first(); item != NULL; item = item->next)
		{
			i = item->object();
			foldedR = box_->fold(i->r());
			i->setCoordinates(foldedR);
			targetCell = cell(i->r());

			// Need to move?
			if (targetCell != currentCell)
			{
				// We first must remove
				if (!currentCell->moveAtom(i, targetCell)) return false;
			}
		}
	}

	return true;
}

/*!
 * \brief Update cell locations of specified atom index
 */
bool Configuration::updateAtomInCell(int id)
{
	// Fold the coordinates of specified atom into the box, and then check its cell location, moving if necessary.
	Cell* currentCell, *targetCell;
	Vec3<double> foldedR;

	// Grab atom pointers
	Atom* i = &atoms_[id];

	// TODO Overload cell() to take a pointer to a Vec3<> in which the folded r can be returned
	// Grab current cell pointer, and calculate folded coordinate and new cell location
	currentCell = i->cell();
	foldedR = box_->fold(i->r());
	i->setCoordinates(foldedR);
	targetCell = cell(foldedR);

	// Need to move?
	if (targetCell != currentCell) currentCell->moveAtom(i, targetCell);

	return true;
}

/*
// Parallel Comms
*/

/*!
 * \brief Broadcast coordinates
 */
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
	
	// Broadcast changeCount_
	if (!Comm.broadcast(&changeCount_, 1)) return false;
#endif
	return true;
}

