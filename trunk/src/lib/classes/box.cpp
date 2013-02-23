/*
	*** Box Definition
	*** src/lib/classes/box.cpp
	Copyright T. Youngs 2012-2013

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "classes/box.h"
#include "classes/cell.h"
#include "base/comms.h"
#include <base/data2d.h>
#include <string.h>

/*!
 * \brief Constructor
 * \details Constructor for Box. 
 */
Box::Box()
{
	type_ = Box::nBoxTypes;
	periodic_.set(TRUE, TRUE, TRUE);
	cells_ = NULL;
	cellIndex_ = NULL;
	cellFlag_ = NULL;
	imageMatrix_ = NULL;
	volume_ = 0.0;
	nCells_ = 0;
}

/*!
 * \brief Virtual Destructor
 * \details Virtual destructor for Box. 
 */
Box::~Box()
{
	clearCells();
}

// Box Type Keywords
const char *BoxTypeKeywords[] = { "Non-Periodic", "Cubic", "Orthorhombic", "Monoclinic", "Triclinic" };

/*!
 * \brief Convert text string to BoxType
 */
Box::BoxType Box::boxType(const char* s)
{
	for (int n=0; n<Box::nBoxTypes; ++n) if (strcmp(s,BoxTypeKeywords[n]) == 0) return (Box::BoxType) n;
	return Box::nBoxTypes;
}

/*!
 * \brief Convert BoxType to text string
 */
const char *Box::boxType(Box::BoxType id)
{
	return BoxTypeKeywords[id];
}

/*!
 * \brief Assignment operator
 */
void Box::operator=(const Box& source)
{
	// Basic Definition
	type_ = source.type_;
	a_ = source.a_;
	b_ = source.b_;
	c_ = source.c_;
	ra_ = source.ra_;
	rb_ = source.rb_;
	rc_ = source.rc_;
	alpha_ = source.alpha_;
	beta_ = source.beta_;
	gamma_ = source.gamma_;
	axes_ = source.axes_;
	inverseAxes_ = source.inverseAxes_;
	periodic_ = source.periodic_;
	
	// Cell data
	divisions_ = source.divisions_;
	cellSize_ = source.cellSize_;
	realCellSize_ = source.realCellSize_;
	cellExtents_ = source.cellExtents_;
	clearCells();
	nCells_ = source.nCells_;
	cellIndex_ = new Cell*[nCells_];
	cells_ = new Cell**[divisions_.x];
	Cell* oldCell;
	int n, m, x, y, z, count = 0;
	for (x = 0; x<divisions_.x; ++x)
	{
		cells_[x] = new Cell*[divisions_.y];
		for (y = 0; y<divisions_.y; ++y)
		{
			cells_[x][y] = new Cell[divisions_.z];
			for (z = 0; z < divisions_.z; ++z)
			{
				cellIndex_[count] = &cells_[x][y][z];
				cells_[x][y][z].setIndex(count++);
				cells_[x][y][z].setGridReference(x,y,z);
			}
		}
	}

	// Copy cellFlag_
	cellFlag_ = new bool[nCells_];
	for (n=0; n<nCells_; ++n) cellFlag_[n] = source.cellFlag_[n];

	// Copy neighbour information from source...
	RefListItem<Cell,bool>* ri;
	for (n=0; n<nCells_; ++n)
	{
		oldCell = source.cellIndex_[n];
		for (ri = oldCell->neighbours().first(); ri != NULL; ri = ri->next) cellIndex_[n]->addNeighbour(cellIndex_[ri->item->index()], ri->data);
	}

	// Copy imageMatrix_
	imageMatrix_ = new bool*[nCells_];
	for (n=0; n<nCells_; ++n)
	{
		imageMatrix_[n] = new bool[nCells_];
		for (m=0; m<nCells_; ++m) imageMatrix_[n][m] = source.imageMatrix_[n][m];
	}

	nCellsDistributed_ = source.nCellsDistributed_;
	lastCellDistributed_ = source.lastCellDistributed_;
}

/*
// Basic Definition
*/

/*!
 * \brief Return Box type
 */
Box::BoxType Box::type() const
{
	return type_;
}

/*!
 * \brief Set box volume (in cubic Angstroms)
 */
void Box::setVolume(double volume)
{
	msg.print("--> Current box volume is %f cubic Angstroms, requested = %f\n", axes_.determinant(), volume);

	double factor = pow(volume,1.0/3.0) / pow(axes_.determinant(),1.0/3.0);
	msg.print("--> Scaling factor = %f\n", factor);
	axes_.applyScaling(factor, factor, factor);
	volume_ = axes_.determinant();
	
	msg.print("--> Final box volume is %f cubic Angstroms\n", volume_);
}

/*!
 * \brief Return volume
 */
double Box::volume() const
{
	return volume_;
}

/*!
 * \brief Return axis length specified
 */
double Box::axisLength(int n) const
{
#ifdef CHECKS
	if ((n < 0) || (n > 2))
	{
		msg.print("OUT_OF_RANGE - Requested length for a bad axis (%i) in Box::axisLength().\n", n);
		return 0.0;
	}
#endif
	return axes_.columnMagnitude(n);
}

/*!
 * \brief Return axis angle specified
 */
double Box::axisAngle(int n) const
{
#ifdef CHECKS
	if ((n < 0) || (n > 2))
	{
		msg.print("OUT_OF_RANGE - Requested angle for a bad axis (%i) in Box::axisAngle().\n", n);
		return 0.0;
	}
#endif
	Vec3<double> u, v;
	u = axes_.columnAsVec3((n+1)%3);
	v = axes_.columnAsVec3((n+2)%3);
	u.normalise();
	v.normalise();
	double dp = u.dp(v);
	return acos(dp)*DEGRAD;
}

/*!
 * \brief Return axes matrix
 */
Matrix3 Box::axes() const
{
	return axes_;
}

/*
// Cell Partitioning
*/

/*!
 * \brief Clear Cell arrays
 */
void Box::clearCells()
{
	if (cells_ != NULL)
	{
		for (int x = 0; x<divisions_.x; ++x)
		{
			for (int y = 0; y<divisions_.y; ++y)
			{
				delete[] cells_[x][y];
			}
			delete[] cells_[x];
		}
		delete[] cells_;
	}
	cells_ = NULL;
	if (cellIndex_ != NULL) delete[] cellIndex_;
	cellIndex_ = NULL;
	if (cellFlag_ != NULL) delete[] cellFlag_;
	cellFlag_ = NULL;
	if (imageMatrix_ != NULL)
	{
		for (int n=0; n<nCells_; ++n) delete[] imageMatrix_[n];
		delete[] imageMatrix_;
	}
	imageMatrix_ = NULL;
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
bool Box::imagesNeeded(Cell* a, Cell* b) const
{
#ifdef CHECKS
	// Check for NULL cell pointers
	if (a == NULL)
	{
		msg.error("NULL_POINTER - NULL Cell pointer 'a' given to Box::imagesNeeded().\n");
		return FALSE;
	}
	if (b == NULL)
	{
		msg.error("NULL_POINTER - NULL Cell pointer 'b' given to Box::imagesNeeded().\n");
		return FALSE;
	}
#endif
	// Never require images for the same Cell
	if (a == b) return FALSE;

	// Do straight check on integer Cell distance
	Vec3<int> u = a->gridReference() - b->gridReference();
	if (abs(u.x)*2 >= divisions_.x) return TRUE;
	if (abs(u.y)*2 >= divisions_.y) return TRUE;
	if (abs(u.z)*2 >= divisions_.z) return TRUE;

	// Second check - mirrored Cell 'b'
	u.x -= dUQMath::sgn(u.x) * divisions_.x;
	u.y -= dUQMath::sgn(u.y) * divisions_.y;
	u.z -= dUQMath::sgn(u.z) * divisions_.z;
	if (abs(u.x)-1 <= cellExtents_.x) return TRUE;
	if (abs(u.y)-1 <= cellExtents_.y) return TRUE;
	if (abs(u.z)-1 <= cellExtents_.z) return TRUE;
	return FALSE;
}

// Generate Cells for Box
bool Box::generateCells(double cellSize, double pairPotentialRange)
{
	// There are some conditions to the partitioning:
	// 1) We require a minimum number of cells (minCellsPerSide) per box side
	// 2) TODO Need to check distances properly for monoclinic and triclinic cells
	// 3) Require Cells to be roughly 'cubic' as far as is possible.
	
	const int minCellsPerSide = 3;
	const double tolerance = 0.01;
	int n, m, x, y, z;

	msg.print("--> Generating Cells for Box - minimum Cells per side is %i, PairPotential range is %f...\n", minCellsPerSide, cellSize);

	// Get Box axis lengths and divide through by cellSize
	Vec3<double> boxLengths(axisLength(0), axisLength(1),  axisLength(2));
	Vec3<int> divisions(boxLengths.x / cellSize, boxLengths.y / cellSize, boxLengths.z / cellSize);
	int minEl;
	divisions_.zero();
	realCellSize_.zero();

	msg.print("--> Initial divisions based on PairPotential range are (x,y,z) = (%i,%i,%i)\n", divisions.x, divisions.y, divisions.z);
	
	// How does the smalles length compare with the PairPotential range?
	if (divisions.min() < minCellsPerSide)
	{
		msg.print("Warning: Box size only allows for %i whole divisions of the PairPotential range (%f) along one or more axes, while we require at least %i.\n", divisions.min(), cellSize, minCellsPerSide);
		
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
	
	msg.print("--> Shortest side (axis %i) will have Cell length of %f Angstroms.\n", minEl, realCellSize_[minEl]);

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
			msg.print("--> Accepted Cell length of %f Angstroms (%i divisions) for axis %i, since it was within tolerance (-%e).\n", realCellSize_[minEl], divisions_[el], el, remainder);
		}
		else if (remainder < tolerance)
		{
			divisions_[el] = int(x);
			realCellSize_[el] = boxLengths[el] / divisions_[el];
			msg.print("--> Accepted Cell length of %f Angstroms (%i divisions) for axis %i, since it was within tolerance (+%e).\n", realCellSize_[minEl], divisions_[el], el, remainder);
		}
		else if (remainder < 0.5)
		{
			// Can't fit more than half another cell in, so reduce number of divisions...
			divisions_[el] = int(x);
			realCellSize_[el] = boxLengths[el] / divisions_[el];
			msg.print("--> Decreased Cell length for axis %i to %f Angstroms (%i divisions).\n", el, realCellSize_[el], divisions_[el]);
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
				msg.print("--> Forced decrease of Cell length for axis %i to %f Angstroms (%i divisions) since increasing it gave a length larger than the PairPotential range.\n", el, realCellSize_[el], divisions_[el]);
			}
			else msg.print("--> Increased Cell length for axis %i to %f Angstroms (%i divisions).\n", el, realCellSize_[el], divisions_[el]);
		}
	}

	// Summarise
	cellSize_.set(1.0 / divisions_.x, 1.0 / divisions_.y, 1.0 / divisions_.z);
	msg.print("--> Final Cell partitioning is (x,y,z) = (%i,%i,%i), giving %i cells in total.\n", divisions_.x, divisions_.y, divisions_.z, divisions_.x*divisions_.y*divisions_.z);

	// Construct Cell arrays
	clearCells();
	nCells_ = divisions_.x*divisions_.y*divisions_.z;
	cellIndex_ = new Cell*[nCells_];
	cellFlag_ = new bool[nCells_];
	msg.print("--> Constructing 3D array of %i cells...\n", nCells_);
	cells_ = new Cell**[divisions_.x];
	int count = 0;
	for (x = 0; x<divisions_.x; ++x)
	{
		cells_[x] = new Cell*[divisions_.y];
		for (y = 0; y<divisions_.y; ++y)
		{
			cells_[x][y] = new Cell[divisions_.z];
			for (z = 0; z < divisions_.z; ++z)
			{
				cellIndex_[count] = &cells_[x][y][z];
				cells_[x][y][z].setIndex(count++);
				cells_[x][y][z].setGridReference(x,y,z);
			}
		}
	}
	
	// Construct Cell neighbour lists
	msg.print("--> Creating Cell neighbour lists...\n");
	// Make a list of integer vectors which we'll then use to pick Cells for the neighbour lists
	Vec3<double> r;
	Matrix3 cellAxes = axes_;
	cellAxes.applyScaling(cellSize_.x, cellSize_.y, cellSize_.z);
	cellExtents_.zero();

	// First, establish a maximal extent in cardinal directions...
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
			msg.print("--> Cell extent along axis %i (%i) exceeds number of available cells (%i) - reduced to %i.\n", n, cellExtents_[n], divisions_[n], (divisions_[n]-1)/2);
			cellExtents_[n] = (divisions_[n]-1)/2;
		}
	}

	// Now, loop over cubic matrix of cellExtents_ and construct list of gridReferences within range
	List< ListVec3<int> > nbrs;
	ListVec3<int> item;
	for (x=-cellExtents_.x; x<=cellExtents_.x; ++x)
	{
		for (y=-cellExtents_.y; y<=cellExtents_.y; ++y)
		{
			for (z=-cellExtents_.z; z<=cellExtents_.z; ++z)
			{
				if ((x == 0) && (y == 0) && (z == 0)) continue;
				// Set the grid reference of the cell to check, but reduce the extent by one
				// unit towards the central box since we need to check the distance of the closest
				// edge to this central Cell.
				r.set(x - dUQMath::sgn(x), y - dUQMath::sgn(y), z - dUQMath::sgn(z));
				r = cellAxes * r;
				if (r.magnitude() < pairPotentialRange) nbrs.add()->set(x, y, z);
			}
		}
	}
	msg.print("--> Added %i Cells to representative neighbour list.\n", nbrs.nItems());

	// Finally, loop over Cells and set neighbours, and construct neighbour matrix
	msg.print("--> Constructing neighbour lists for individual Cells...\n");
	Cell* nbr, *centre;
	bool isImage;
	for (x=0; x<divisions_.x; ++x)
	{
		for (y=0; y<divisions_.y; ++y)
		{
			for (z=0; z<divisions_.z; ++z)
			{
				for (ListVec3<int>* item = nbrs.first(); item != NULL; item = item->next)
				{
					// Retrieve Cell pointers
					nbr = cell(x+item->x, y+item->y, z+item->z);
					centre = &cells_[x][y][z];
					isImage = type_ == Box::NonPeriodicBox ? FALSE : imagesNeeded(centre, nbr);
					centre->addNeighbour(nbr, isImage);
				}
			}
		}
	}
	imageMatrix_ = new bool*[nCells_];
	for (n=0; n<nCells_; ++n)
	{
		imageMatrix_[n] = new bool[nCells_];
		centre = cell(n);
		for (m=0; m<nCells_; ++m) imageMatrix_[n][m] = imagesNeeded(centre, cell(m));
	}

	// Send Cell info to Comm so suitable parallel strategy can be deduced
	if (!Comm.setupStrategy(divisions_, cellExtents_, nbrs)) return FALSE;

	return TRUE;
}

/*!
 * \brief Return number of Cells for box
 */
int Box::nCells() const
{
	return nCells_;
}

/*!
 * \brief Return real Cell dimensions
 */
Vec3<double> Box::realCellSize() const
{
	return realCellSize_;
}

/*!
 * \brief Retrieve Cell with (wrapped) grid reference specified
 */
Cell* Box::cell(int x, int y, int z) const
{
	x = x%divisions_.x;
	if (x < 0) x += divisions_.x;
	y = y%divisions_.y;
	if (y < 0) y += divisions_.y;
	z = z%divisions_.z;
	if (z < 0) z += divisions_.z;
	return &cells_[x][y][z];
}

/*!
 * \brief Retrieve Cell with id specified
 */
Cell* Box::cell(int id) const
{
#ifdef CHECKS
	if ((id < 0) || (id >= nCells_))
	{
		msg.print("OUT_OF_RANGE - Cell ID %i is out of range (nCells = %i)\n.", id, nCells_);
		return 0;
	}
#endif
	return cellIndex_[id];
}

/*!
 * \brief Return whether two Cells need minimum image calculation
 */
bool Box::useMim(Cell* a, Cell* b) const
{
#ifdef CHECKS
	// Check for NULL pointers
	if (a == NULL)
	{
		msg.error("NULL_POINTER - NULL Cell pointer 'a' passed to Box::useMim().\n");
		return FALSE;
	}
	if (b == NULL)
	{
		msg.error("NULL_POINTER - NULL Cell pointer 'b' passed to Box::useMim().\n");
		return FALSE;
	}
#endif
	return imageMatrix_[a->index()][b->index()];
}

/*!
 * \brief Initialise Cells for distribution
 */
void Box::initialiseCellDistribution()
{
	nCellsDistributed_ = 0;
	lastCellDistributed_ = -1;
	for (int n=0; n<nCells_; ++n)
	{
		// Clear usage flag, and remove any remaining locks
		while (cellIndex_[n]->lockCount() > 0) cellIndex_[n]->removeLock();
		cellFlag_[n] = FALSE;
	}
}

/*!
 * \brief Return next available Cell for calculation
 * \details All processes should call here together. The master process will distribute the index of the next available Cell to each process.
 * If all Cells have been distributed once then Cell::AllCellsComplete is returned to all processes. Otherwise, the next available 'unused' Cell index
 * is returned. If the routine runs out of available cells during a distribution cycle, then what is returned is determined by the value of
 * 'allowRepeats'. If 'allowRepeats' is TRUE then Cells which have already been sent once will be sent again, in order for all processes to be
 * doing useful work at all times. If 'allowRepeats' is FALSE, then repeat calculations on Cells are not allowed, and NoCellsAvailable is returned
 * instead. The latter is relevant when property calculation is being performed, and each Cell must be subjected to calculation once and once only.
 * 
 * Any routine which employs Cell distribution in this way implicitly operates in parallel with process groups.
 */
int Box::nextAvailableCell(bool willBeModified, bool allowRepeats)
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
			for (int n=1; n<Comm.nProcesses(); ++n) if (!Comm.send(Cell::AllCellsComplete, n)) printf("MPI error in Box::nextAvailableCell() when all cells were distributed.\n");
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
				if ((cellIndex_[cellId]->lockCount() != 0) || (willBeModified && !cellIndex_[cellId]->canLock()))
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
				cellIndex_[cellId]->lock(willBeModified);
				cellFlag_[cellId] = TRUE;
			}
			
			// Broadcast Cell index to all processes in current ProcessGroup
			procList = Comm.processes(group);
			for (proc=0; proc<Comm.nProcesses(group); ++proc)
			{
				if (procList[proc] == 0) rootCellId = cellId;
				else if (!Comm.send(cellId, procList[proc])) printf("MPI error in Box::nextAvailableCell() sending next cell to process %i.\n", procList[proc]);
			}
		}

		// All done - return our own cellId
		return rootCellId;
	}
	else
	{
		// Slaves just receive the next Cell index
		if (!Comm.receive(cellId, 0)) printf("MPI error in Box::nextAvailableCell() when all cells were distributed.\n");
		msg.printVerbose("Slave with rank %i received Cell id %i\n", Comm.rank(), cellId);
		return cellId;
	}
#else
	// If we have distributed all Cells already, return -1
	if (nCellsDistributed_ == nCells_) return Cell::AllCellsComplete;

	++lastCellDistributed_;
	++nCellsDistributed_;
	cellIndex_[lastCellDistributed_]->lock(willBeModified);
	cellFlag_[lastCellDistributed_] = TRUE;

	return lastCellDistributed_;
#endif
}

/*!
 * \brief Unlock Cell specified, once calculation is complete
 * \details All processes should call this routine once they have finished performing manipulations of the Cell they were given from
 * Box::nextAvailableCell(), passing the cellId as the single argument.
 */
bool Box::finishedWithCell(bool willBeModified, int cellId)
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
				if (!Comm.receive(cellId, Comm.processes(group)[0])) msg.print("MPI error in Box::finishedWithCell() receiving from process with rank %i.\n", group);
			}
			if (cellId >= 0) cellIndex_[cellId]->unlock(willBeModified);
		}
	}
	else
	{
		// Slaves just send their id to the master, provided they are process group leader
		if (Comm.processGroupLeader())
		{
			if (!Comm.send(cellId, 0)) msg.print("MPI error in Box::finishedWithCell().\n");
		}
	}
	
	// Collect all processes together
	if (!Comm.wait(DUQComm::World)) return FALSE;
#else
	if (cellId >= 0) cellIndex_[cellId]->unlock(willBeModified);
#endif
	return TRUE;
}

/*
// Utility Routines
*/

/*!
 * \brief Return radius of largest possible inscribed sphere for box
 */
double Box::inscribedSphereRadius() const
{
	// Radius of largest inscribed sphere is the smallest of the three calculated values....
	double mag, diameter, result = 0.0;
	Vec3<double> cross;
	int n, i, j;
	for (n=0; n<3; ++n)
	{
		i = (n+1)%3;
		j = (n+2)%3;
		cross = axes_.columnAsVec3(i) * axes_.columnAsVec3(j);
		mag = cross.magnitude();
		diameter = fabs(axes_.columnAsVec3(n).dp(cross / mag));
		if (n == 0) result = diameter;
		else if (diameter < result) result = diameter;
	}
	return result*0.5;
}

/*!
 * \brief Calculate the RDF normalisation for the Box
 */
bool Box::calculateRDFNormalisation(Data2D& boxNorm, double rdfRange, double rdfBinWidth, int nPoints) const
{
	// Setup array - we will use a nominal bin width of 0.1 Angstroms and then interpolate to the rdfBinWidth afterwards
	const double binWidth = 0.1;
	const double rBinWidth = 1.0/binWidth;
	int bin, nBins = rdfRange / binWidth;
	Data2D normData;
	normData.initialise(nBins);
	Array<double>& y = normData.arrayY();
	for (int n=0; n<nBins; ++n) normData.arrayX()[n] = (n+0.5)*binWidth;
	
	Vec3<double> centre = axes_*Vec3<double>(0.5,0.5,0.5);

	// Insert enough points to give decent statistics - approx. 50,000,000
	const int nPointsPerProcess = nPoints / Comm.nProcesses();
	msg.print("--> Number of insertion points (per process) is %i\n", nPointsPerProcess);
	y = 0.0;
	for (int n=0; n<nPointsPerProcess; ++n)
	{
		bin = (randomCoordinate() - centre).magnitude() * rBinWidth;
		if (bin < nBins) y[bin] += 1.0;
	}
	if (!Comm.allSum(y.array(), nBins)) return FALSE;

	// Normalise histogram data, and scale by volume and binWidth ratio
	y /= double(nPointsPerProcess*Comm.nProcesses());
	y *= volume_ * (rdfBinWidth / binWidth);
	normData.interpolate();

	// Write histogram data for random distribution
	if (Comm.master()) normData.save("duq.box.random");
	
	// Now we have the interpolated data, create the proper interpolated data
	nBins = rdfRange/rdfBinWidth;
	boxNorm.clear();

	// Rescale against expected volume for spherical shells
	double shellVolume, r = 0.0, maxHalf = inscribedSphereRadius(), x = 0.5*rdfBinWidth;
	for (int n=0; n<nBins; ++n)
	{
		// We know that up to the maximum (orthogonal) half-cell width the normalisation should be exactly 1.0...
		if (x < maxHalf) boxNorm.addPoint(x, 1.0);
		else
		{
			shellVolume = (4.0/3.0)*PI*(pow(r+rdfBinWidth,3.0) - pow(r,3.0));
			boxNorm.addPoint(x,  shellVolume / normData.interpolated(x));
// 			boxNorm[n] = normData.interpolated(r);
		}
		r += rdfBinWidth;
		x += rdfBinWidth;
	}
	
	// Interpolate normalisation array
	boxNorm.interpolate();

	// Write final box normalisation file
	if (Comm.master()) boxNorm.save("duq.box.norm");

	return TRUE;
}

/*
// Utility Routines (Static)
*/

/*!
 * \brief Return angle (in degrees, no MIM) between coordinates
 */
double Box::angle(const Vec3<double>& i, const Vec3<double>& j, const Vec3<double>& k)
{
	static Vec3<double> vecji, vecjk;
	vecji = i - j;
	vecji.normalise();
	vecjk = k - j;
	vecjk.normalise();
	return acos(vecji.dp(vecjk)) * DEGRAD;
}

/*!
 * \brief Return angle (in degrees) between supplied normalised vectors
 */
double Box::angle(const Vec3<double>& normji, const Vec3<double>& normjk)
{
	return acos(normji.dp(normjk)) * DEGRAD;
}

/*!
 * \brief Return angle (in degrees) between supplied normalised vectors
 */
double Box::angle(const Vec3<double>& normji, const Vec3<double>& normjk, double& dotProduct)
{
	dotProduct = normji.dp(normjk);
	return acos(dotProduct) * DEGRAD;
}
