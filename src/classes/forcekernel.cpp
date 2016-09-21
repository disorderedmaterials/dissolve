/*
	*** ForceKernel
	*** src/classes/forcekernel.cpp
	Copyright T. Youngs 2012-2014

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

#include "classes/forcekernel.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/potentialmap.h"
#include "classes/molecule.h"
#include "classes/species.h"
#include "base/comms.h"

// Constructor
ForceKernel::ForceKernel(const Box* box, const PotentialMap& potentialMap) : box_(box), potentialMap_(potentialMap)
{
}

// Destructor
ForceKernel::~ForceKernel()
{
}

/*
 * Internal Force Calculation
 */

// Calculate PairPotential forces between Atoms provided (no minimum image calculation)
void ForceKernel::forcesWithoutMim(const Atom* i, const Atom* j, double* fx, double* fy, double* fz, double scale)
{
	Vec3<double> force = j->r() - i->r();
	double distanceSq = force.magSqAndNormalise();
	force *= potentialMap_.force(i->globalTypeIndex(), j->globalTypeIndex(), distanceSq) * scale;
	int index = i->index();
	fx[index] += force.x;
	fy[index] += force.y;
	fz[index] += force.z;
	index = j->index();
	fx[index] -= force.x;
	fy[index] -= force.y;
	fz[index] -= force.z;
}

// Calculate PairPotential forces between Atom and Grain provided (no minimum image calculation)
void ForceKernel::forcesWithoutMim(const Atom* i, const Grain* grain, bool excludeIgtJ, double* fx, double* fy, double* fz)
{
	int m, nAtomsJ = grain->nAtoms();
	if (excludeIgtJ)
	{
		for (m=0; m<nAtomsJ; ++m) if (i->index() <= grain->atom(m)->index()) forcesWithoutMim(i, grain->atom(m), fx, fy, fz);
	}
	else
	{
		for (m=0; m<nAtomsJ; ++m) forcesWithoutMim(i, grain->atom(m), fx, fy, fz);
	}
}

/*
 * \brief Calculate PairPotential forces between Grains provided (no minimum image calculation)
 * \details This calculates the forces between the individual Atoms of the two Grains provided, adding contributions into the three
 * force arrays supplied. A check is made as to whether the two Grains have the same parent Molecule. If so, then force contributions
 * are scaled such that intramolecular interactions are excluded.
 */
void ForceKernel::forcesWithoutMim(const Grain* grainI, const Grain* grainJ, double* fx, double* fy, double* fz)
{
	int n, m, nAtomsI = grainI->nAtoms(), nAtomsJ = grainJ->nAtoms();
	Atom* i, *j;
	if (grainI->parent() != grainJ->parent())
	{
		for (n=0; n<nAtomsI; ++n)
		{
			i = grainI->atom(n);
			for (m=0; m<nAtomsJ; ++m) forcesWithoutMim(i, grainJ->atom(m), fx, fy, fz);
		}
	}
	else
	{
		double scale;
		for (n=0; n<nAtomsI; ++n)
		{
			i = grainI->atom(n);
			for (m=0; m<nAtomsJ; ++m)
			{
				j = grainJ->atom(m);
				scale = grainI->parent()->species()->scaling(i->moleculeAtomIndex(), j->moleculeAtomIndex());
				if (scale < 1.0e-3) continue;
				forcesWithoutMim(i, j, fx, fy, fz, scale);
			}
		}
	}
}

// Calculate PairPotential forces between Atoms provided (minimum image calculation)
void ForceKernel::forcesWithMim(const Atom* i, const Atom* j, double* fx, double* fy, double* fz, double scale)
{
	Vec3<double> force = box_->minimumVector(i, j);
	double distanceSq = force.magSqAndNormalise();
	force *= potentialMap_.force(i->globalTypeIndex(), j->globalTypeIndex(), distanceSq) * scale;
	int index = i->index();
	fx[index] += force.x;
	fy[index] += force.y;
	fz[index] += force.z;
	index = j->index();
	fx[index] -= force.x;
	fy[index] -= force.y;
	fz[index] -= force.z;
}

// Calculate PairPotential forces between Atom and Grain provided (minimum image calculation)
void ForceKernel::forcesWithMim(const Atom* i, const Grain* grain, bool excludeIgtJ, double* fx, double* fy, double* fz)
{
	int m, nAtomsJ = grain->nAtoms();
	if (excludeIgtJ)
	{
		for (m=0; m<nAtomsJ; ++m) if (i->index() <= grain->atom(m)->index()) forcesWithMim(i, grain->atom(m), fx, fy, fz);
	}
	else
	{
		for (m=0; m<nAtomsJ; ++m) forcesWithMim(i, grain->atom(m), fx, fy, fz);
	}
}

/*
 * \brief Calculate PairPotential forces between Grains provided (minimum image calculation)
 * \details This calculates the forces between the individual Atoms of the two Grains provided, applying minimum image convention, adding
 * contributions into the three force arrays supplied. A check is made as to whether the two Grains have the same parent Molecule. If so,
 * then force contributions are scaled such that intramolecular interactions are excluded.
 */
void ForceKernel::forcesWithMim(const Grain* grainI, const Grain* grainJ, double* fx, double* fy, double* fz)
{
	int n, m, nAtomsI = grainI->nAtoms(), nAtomsJ = grainJ->nAtoms();
	Atom* i, *j;
	if (grainI->parent() != grainJ->parent())
	{
		for (n=0; n<nAtomsI; ++n)
		{
			i = grainI->atom(n);
			for (m=0; m<nAtomsJ; ++m) forcesWithMim(i, grainJ->atom(m), fx, fy, fz);
		}
	}
	else
	{
		double scale;
		for (n=0; n<nAtomsI; ++n)
		{
			i = grainI->atom(n);
			for (m=0; m<nAtomsJ; ++m)
			{
				j = grainJ->atom(m);
				scale = grainI->parent()->species()->scaling(i->moleculeAtomIndex(), j->moleculeAtomIndex());
				if (scale < 1.0e-3) continue;
				forcesWithMim(i, j, fx, fy, fz, scale);
			}
		}
	}
}

/*
 * Force Calculation
 */

// Calculate PairPotential forces between Atoms provided
void ForceKernel::forces(const Atom* i, const Atom* j, bool applyMim, bool excludeIgtJ, double* fx, double* fy, double* fz)
{
#ifdef CHECKS
	if (i == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer (i) passed to ForceKernel::force(Atom,Atom,bool,bool).\n");
		return;
	}
	if (j == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer (j) passed to ForceKernel::force(Atom,Atom,bool,bool).\n");
		return;
	}
	// If Atoms are the same, we refuse to calculate
	if (i == j)
	{
		printf("Warning: Refusing to calculate self-force in ForceKernel::force(Atom,Atom,bool,bool).\n");
		return;
	}
#endif
	// Check indices of Atoms if required
	if (excludeIgtJ && (i->index() > j->index())) return;

	if (applyMim) forcesWithMim(i, j, fx, fy, fz);
	else forcesWithoutMim(i, j, fx, fy, fz);
}

// Calculate PairPotential forces between Atom and Grain provided
void ForceKernel::forces(const Atom* i, const Grain* grain, bool applyMim, bool excludeIgtJ, double* fx, double* fy, double* fz)
{
#ifdef CHECKS
	if (i == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer passed to ForceKernel::force(Atom,Grain,bool,bool).\n");
		return;
	}
	if (grain == NULL)
	{
		msg.error("NULL_POINTER - NULL Grain pointer passed to ForceKernel::force(Atom,Grain,bool,bool).\n");
		return;
	}
#endif
	if (applyMim) forcesWithMim(i, grain, excludeIgtJ, fx, fy, fz);
	else forcesWithoutMim(i, grain, excludeIgtJ, fx, fy, fz);
}

// Calculate PairPotential forces between Grains provided
void ForceKernel::forces(const Grain* grainI, const Grain* grainJ, bool applyMim, bool excludeIgtJ, double* fx, double* fy, double* fz)
{
#ifdef CHECKS
	if (grainI == NULL)
	{
		msg.error("NULL_POINTER - NULL Grain pointer (grainI) passed to ForceKernel::force(Grain,Grain,bool,bool).\n");
		return;
	}
	if (grainJ == NULL)
	{
		msg.error("NULL_POINTER - NULL Grain pointer (grainJ) passed to ForceKernel::force(Grain,Grain,bool,bool).\n");
		return;
	}
	// If Grains are the same, we refuse to calculate
	if (grainI == grainJ)
	{
		printf("Warning: Refusing to calculate self-energy in ForceKernel::force(Grain,Grain,bool,bool).\n");
		return;
	}
#endif
	// Check Grain-Grain distance
// 	double rSq;
// 	if (applyMim) rSq = box_->minimumDistanceSquared(grainI->centre(), grainJ->centre());
// 	else rSq = (grainI->centre() - grainJ->centre()).magnitudeSq();
// // 	msg.print("GG %3i %3i MIM=%i EXC=%i rSq=%f (%f,%f,%f) (%f,%f,%f)\n", grainI->index(), grainJ->index(), applyMim, excludeIgtJ, rSq, grainI->centre().x, grainI->centre().y, grainI->centre().z, grainJ->centre().x, grainJ->centre().y, grainJ->centre().z);
// 	if (rSq > cutoffSq) return;
	
	// Check exclusion of I > J
	if (excludeIgtJ && (grainI->index() > grainJ->index())) return;

	if (applyMim) forcesWithMim(grainI, grainJ, fx, fy, fz);
	else forcesWithoutMim(grainI, grainJ, fx, fy, fz);
}

// Calculate PairPotential forces between Atom and Cell contents
void ForceKernel::forces(const Atom* i, const Cell* cell, bool applyMim, bool excludeIgtJ, double* fx, double* fy, double* fz)
{
#ifdef CHECKS
	if (i == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer passed to ForceKernel::force(Atom,Cell,bool,bool).\n");
		return;
	}
	if (cell == NULL)
	{
		msg.error("NULL_POINTER - NULL Cell pointer passed to ForceKernel::force(Atom,Cell,bool,bool).\n");
		return;
	}
#endif
	// Loop over Cell Grains
	for (RefListItem<Grain,int>* grainRef = cell->grains(); grainRef != NULL; grainRef = grainRef->next)
	{
		forces(i, grainRef->item, applyMim, excludeIgtJ, fx, fy, fz);
	}
}

// Calculate PairPotential forces between Grain and Cell contents
void ForceKernel::forces(const Grain* grain, const Cell* cell, bool applyMim, bool excludeIgtJ, double* fx, double* fy, double* fz)
{
#ifdef CHECKS
	if (grain == NULL)
	{
		msg.error("NULL_POINTER - NULL Grain pointer passed to ForceKernel::force(Grain,Cell,bool).\n");
		return;
	}
	if (cell == NULL)
	{
		msg.error("NULL_POINTER - NULL Cell pointer passed to ForceKernel::force(Grain,Cell,bool).\n");
		return;
	}
#endif
	// Loop over Cell Grains
	for (RefListItem<Grain,int>* grainRef = cell->grains(); grainRef != NULL; grainRef = grainRef->next) forces(grain, grainRef->item, applyMim, excludeIgtJ, fx, fy, fz);
}

/*
 * \brief Calculate PairPotential forces between Atom and list of Cells
 * \details Calculate the PairPotential forces between the supplied Atom and a list of Cells, storing (adding) the results to
 * the three force component arrays provided. Because partial forces are added to the supplied arrays which are defined
 * elsewhere, no parallel summation of the calculated data is performed by this routine, and must be handled externally by
 * the calling function.
 */
void ForceKernel::forces(const Atom* i, int nNeighbours, Cell** neighbours, bool applyMim, bool excludeIgtJ, double* fx, double* fy, double* fz, DUQComm::CommGroup group)
{
#ifdef CHECKS
	if (i == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer passed to ForceKernel::force(Atom,RefList<Cell>,bool,ParallelStyle).\n");
		return;
	}
#endif
	// Communication group determines loop/summation style
	if (group == DUQComm::Solo)
	{
		// Straight loop over Cell neighbours
		for (int n=0; n<nNeighbours; ++n) forces(i, neighbours[n], applyMim, excludeIgtJ, fx, fy, fz);
	}
	else if (group == DUQComm::Group)
	{
		// Striped loop over Cell neighbours (Process Groups)
		for (int n=Comm.localGroupRank(); n<nNeighbours; n += Comm.localGroupSize()) forces(i, neighbours[n], applyMim, excludeIgtJ, fx, fy, fz);
	}
	else
	{
		// Striped loop over Cell neighbours (individual processes)
		for (int n=Comm.rank(); n<nNeighbours; n += Comm.nProcesses()) forces(i, neighbours[n], applyMim, excludeIgtJ, fx, fy, fz);
	}
}

/*
 * \brief Calculate PairPotential forces between Grain and list of Cells
 * \details Calculate the PairPotential forces between the supplied Grain and a list of Cells, storing (adding) the results to
 * the three force component arrays provided. Because partial forces are added to the supplied arrays which are defined
 * elsewhere, no parallel summation of the calculated data is performed by this routine, and must be handled externally by
 * the calling function.
 */
void ForceKernel::forces(const Grain* grain, int nNeighbours, Cell** neighbours, bool applyMim, bool excludeIgtJ, double* fx, double* fy, double* fz, DUQComm::CommGroup group)
{
#ifdef CHECKS
	if (grain == NULL)
	{
		msg.error("NULL_POINTER - NULL Grain pointer passed to ForceKernel::force(Grain,RefList<Cell>,bool,ParallelStyle).\n");
		return;
	}
#endif
	// Communication group determines loop/summation style
	if (group == DUQComm::Solo)
	{
		// Straight loop over Cell neighbours
		for (int n=0; n<nNeighbours; ++n) forces(grain, neighbours[n], applyMim, excludeIgtJ, fx, fy, fz);
	}
	else if (group == DUQComm::Group)
	{
		// Striped loop over Cell neighbours (Process Groups)
		for (int n=Comm.localGroupRank(); n<nNeighbours; n += Comm.localGroupSize()) forces(grain, neighbours[n], applyMim, excludeIgtJ, fx, fy, fz);
	}
	else
	{
		// Striped loop over Cell neighbours (individual processes)
		for (int n=Comm.rank(); n<nNeighbours; n += Comm.nProcesses()) forces(grain, neighbours[n], applyMim, excludeIgtJ, fx, fy, fz);
	}
}
