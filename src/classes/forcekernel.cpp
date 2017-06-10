/*
	*** ForceKernel
	*** src/classes/forcekernel.cpp
	Copyright T. Youngs 2012-2017

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

#include "classes/configuration.h"
#include "classes/forcekernel.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/potentialmap.h"
#include "classes/molecule.h"
#include "classes/species.h"
#include "base/processpool.h"

// Constructor
ForceKernel::ForceKernel(ProcessPool& procPool, const Configuration* cfg, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz, double cutoffDistance) : processPool_(procPool),  configuration_(cfg), potentialMap_(potentialMap), fx_(fx), fy_(fy), fz_(fz)
{
	box_ = configuration_->box();
	cutoffDistanceSquared_ = (cutoffDistance < 0.0 ? potentialMap_.rangeSquared() : cutoffDistance*cutoffDistance);
}

// Destructor
ForceKernel::~ForceKernel()
{
}

/*
 * Internal Force Calculation
 */

// Calculate PairPotential forces between Atoms provided (no minimum image calculation)
void ForceKernel::forcesWithoutMim(const Atom* i, const Atom* j, double scale)
{
	Vec3<double> force = j->r() - i->r();
	double distanceSq = force.magSqAndNormalise();
	force *= potentialMap_.force(i->globalTypeIndex(), j->globalTypeIndex(), distanceSq) * scale;
	int index = i->index();
	fx_[index] += force.x;
	fy_[index] += force.y;
	fz_[index] += force.z;
	index = j->index();
	fx_[index] -= force.x;
	fy_[index] -= force.y;
	fz_[index] -= force.z;
}

// Calculate PairPotential forces between Atom and Grain provided (no minimum image calculation)
void ForceKernel::forcesWithoutMim(const Atom* i, const Grain* grain, bool excludeIgtJ)
{
	int m, nAtomsJ = grain->nAtoms();
	if (excludeIgtJ)
	{
		for (m=0; m<nAtomsJ; ++m) if (i->index() <= grain->atom(m)->index()) forcesWithoutMim(i, grain->atom(m));
	}
	else
	{
		for (m=0; m<nAtomsJ; ++m) forcesWithoutMim(i, grain->atom(m));
	}
}

// Calculate PairPotential forces between Grains provided (no minimum image calculation)
void ForceKernel::forcesWithoutMim(const Grain* grainI, const Grain* grainJ)
{
	/*
	 * This calculates the forces between the individual Atoms of the two Grains provided, adding contributions into the three
	 * force arrays supplied. A check is made as to whether the two Grains have the same parent Molecule. If so, then force contributions
	 * are scaled such that intramolecular interactions are excluded.
	 */
	int n, m, nAtomsI = grainI->nAtoms(), nAtomsJ = grainJ->nAtoms();
	Atom* i, *j;
	if (grainI->parent() != grainJ->parent())
	{
		for (n=0; n<nAtomsI; ++n)
		{
			i = grainI->atom(n);
			for (m=0; m<nAtomsJ; ++m) forcesWithoutMim(i, grainJ->atom(m));
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
				forcesWithoutMim(i, j, scale);
			}
		}
	}
}

// Calculate PairPotential forces between Atoms provided (minimum image calculation)
void ForceKernel::forcesWithMim(const Atom* i, const Atom* j, double scale)
{
	Vec3<double> force = box_->minimumVector(i, j);
	double distanceSq = force.magSqAndNormalise();
	force *= potentialMap_.force(i->globalTypeIndex(), j->globalTypeIndex(), distanceSq) * scale;
	int index = i->index();
	fx_[index] += force.x;
	fy_[index] += force.y;
	fz_[index] += force.z;
	index = j->index();
	fx_[index] -= force.x;
	fy_[index] -= force.y;
	fz_[index] -= force.z;
}

// Calculate PairPotential forces between Atom and Grain provided (minimum image calculation)
void ForceKernel::forcesWithMim(const Atom* i, const Grain* grain, bool excludeIgtJ)
{
	int m, nAtomsJ = grain->nAtoms();
	if (excludeIgtJ)
	{
		for (m=0; m<nAtomsJ; ++m) if (i->index() <= grain->atom(m)->index()) forcesWithMim(i, grain->atom(m));
	}
	else
	{
		for (m=0; m<nAtomsJ; ++m) forcesWithMim(i, grain->atom(m));
	}
}

// Calculate PairPotential forces between Grains provided (minimum image calculation)
void ForceKernel::forcesWithMim(const Grain* grainI, const Grain* grainJ)
{
	/*
	 * This calculates the forces between the individual Atoms of the two Grains provided, applying minimum image convention, adding
	 * contributions into the three force arrays supplied. A check is made as to whether the two Grains have the same parent Molecule. If so,
	 * then force contributions are scaled such that intramolecular interactions are excluded.
	 */
	int n, m, nAtomsI = grainI->nAtoms(), nAtomsJ = grainJ->nAtoms();
	Atom* i, *j;
	if (grainI->parent() != grainJ->parent())
	{
		for (n=0; n<nAtomsI; ++n)
		{
			i = grainI->atom(n);
			for (m=0; m<nAtomsJ; ++m) forcesWithMim(i, grainJ->atom(m));
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
				forcesWithMim(i, j, scale);
			}
		}
	}
}

/*
 * PairPotential Terms
 */

// Calculate forces between atoms (provided as pointers)
void ForceKernel::forces(const Atom* i, const Atom* j, bool applyMim, bool excludeIgeJ)
{
#ifdef CHECKS
	if (i == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Atom pointer (i) passed to ForceKernel::forces(Atom,Atom,bool,bool).\n");
		return;
	}
	if (j == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Atom pointer (j) passed to ForceKernel::forces(Atom,Atom,bool,bool).\n");
		return;
	}
#endif
	// If Atoms are the same, we refuse to calculate
	if (i == j)
	{
// 		printf("Warning: Refusing to calculate self-energy in ForceKernel::forces(Atom,Atom,bool,bool).\n");
		return;
	}
	
	// Check indices of Atoms if required
	if (excludeIgeJ && (i->index() >= j->index())) return;

	if (applyMim) forcesWithMim(i, j);
	else forcesWithoutMim(i, j);
}

// Calculate forces between atoms in supplied cells
void ForceKernel::forces(Cell* centralCell, Cell* otherCell, bool applyMim, bool excludeIgeJ, ProcessPool::LoopContext loopContext, bool sumOverProcesses)
{
#ifdef CHECKS
	if (centralCell == NULL)
	{
		Messenger::error("NULL_POINTER - NULL central Cell pointer passed to ForceKernel::forces(Cell,Cell,bool,bool).\n");
		return 0.0;
	}
	if (otherCell == NULL)
	{
		Messenger::error("NULL_POINTER - NULL other Cell pointer passed to ForceKernel::forces(Cell,Cell,bool,bool).\n");
		return 0.0;
	}
#endif
	double totalEnergy = 0.0;
	Atom** centralAtoms = centralCell->atoms().objects(), **otherAtoms = otherCell->atoms().objects();
	Atom* ii, *jj;
	Vec3<double> rI;
	Molecule* molI;
	int i, typeI, indexI, j, start = 0, stride = 1;
	double rSq, scale;

	// Get start/stride for specified loop context
	start = processPool_.interleavedLoopStart(loopContext);
	stride = processPool_.interleavedLoopStride(loopContext);

	// Loop over central cell atoms
	if (applyMim)
	{
		for (i = start; i < centralCell->atoms().nItems(); i += stride)
		{
			ii = centralAtoms[i];
			molI = ii->molecule();
			indexI = ii->index();
			typeI = ii->globalTypeIndex();
			rI = ii->r();

			// Straight loop over other cell atoms
			for (j = 0; j < otherCell->atoms().nItems(); ++j)
			{
				jj = otherAtoms[j];

				// Check exclusion of I > J
				if (excludeIgeJ && (indexI >= jj->index())) continue;

				// Calculate rSquared distance betwenn atoms, and check it against the stored cutoff distance
				rSq = box_->minimumDistanceSquared(rI, jj->r());
				if (rSq > cutoffDistanceSquared_) continue;

				// Check for atoms in the same species
				if (molI != jj->molecule()) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq);
				else
				{
					scale = molI->species()->scaling(ii->moleculeAtomIndex(), jj->moleculeAtomIndex());
					if (scale > 1.0e-3) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq) * scale;
				}
			}
		}
	}
	else
	{
		for (i = start; i < centralCell->atoms().nItems(); i += stride)
		{
			ii = centralAtoms[i];
			molI = ii->molecule();
			indexI = ii->index();
			typeI = ii->globalTypeIndex();
			rI = ii->r();

			// Straight loop over other cell atoms
			for (j = 0; j < otherCell->atoms().nItems(); ++j)
			{
				jj = otherAtoms[j];
				
				// Check exclusion of I > J
				if (excludeIgeJ && (ii->index() >= jj->index())) continue;

				// Calculate rSquared distance betwenn atoms, and check it against the stored cutoff distance
				rSq = (rI - jj->r()).magnitudeSq();
				if (rSq > cutoffDistanceSquared_) continue;

				// Check for atoms in the same molecule
				if (molI != jj->molecule()) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq);
				else
				{
					scale = molI->species()->scaling(ii->moleculeAtomIndex(), jj->moleculeAtomIndex());
					if (scale > 1.0e-3) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq) * scale;
				}
			}
		}
	}

	// Sum over processes if necessary
	if (sumOverProcesses)
	{
		if (loopContext == ProcessPool::OverGroupProcesses) processPool_.allSum(&totalEnergy, 1, ProcessPool::Group);
		else if (loopContext == ProcessPool::OverPoolProcesses) processPool_.allSum(&totalEnergy, 1);
	}
}

// Calculate forces between cell and atomic neighbours
void ForceKernel::forces(Cell* centralCell, bool excludeIgeJ, ProcessPool::LoopContext loopContext, bool sumOverProcesses)
{
	double totalEnergy = 0.0;
	Atom** centralAtoms = centralCell->atoms().objects();
	Atom** neighbours = centralCell->atomNeighbours().objects();
	Atom** mimNeighbours = centralCell->mimAtomNeighbours().objects();
	Atom* ii, *jj;
	Vec3<double> rJ;
	int i, j, indexJ, typeJ, start = 0, stride = 1;
	Molecule* molJ;
	double rSq, scale;

	// Get start/stride for specified loop context
	start = processPool_.interleavedLoopStart(loopContext);
	stride = processPool_.interleavedLoopStride(loopContext);

	// Straight loop over atoms *not* requiring mim
	for (j = 0; j < centralCell->atomNeighbours().nItems(); ++j)
	{
		jj = neighbours[j];
		molJ = jj->molecule();
		indexJ = jj->index();
		typeJ = jj->globalTypeIndex();
		rJ = jj->r();

		// Loop over central cell atoms
		for (i = start; i < centralCell->atoms().nItems(); i += stride)
		{
			ii = centralAtoms[i];

			// Check exclusion of I > J
			if (excludeIgeJ && (ii->index() >= indexJ)) break;

			// Calculate rSquared distance betwenn atoms, and check it against the stored cutoff distance
			rSq = (ii->r() - rJ).magnitudeSq();
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (ii->molecule() != molJ) totalEnergy += potentialMap_.force(typeJ, ii->globalTypeIndex(), rSq);
			else
			{
				scale = ii->molecule()->species()->scaling(ii->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale > 1.0e-3) totalEnergy += potentialMap_.force(typeJ, ii->globalTypeIndex(), rSq);
			}
		}
	}

	// Straight loop over atoms requiring mim
	for (j = 0; j < centralCell->mimAtomNeighbours().nItems(); ++j)
	{
		jj = mimNeighbours[j];
		molJ = jj->molecule();
		indexJ = jj->index();
		typeJ = jj->globalTypeIndex();
		rJ = jj->r();

		// Loop over central cell atoms
		for (i = start; i < centralCell->atoms().nItems(); i += stride)
		{
			ii = centralAtoms[i];

			// Check exclusion of I > J
			if (excludeIgeJ && (ii->index() >= indexJ)) break;

			// Calculate rSquared distance betwenn atoms, and check it against the stored cutoff distance
			rSq = box_->minimumDistanceSquared(ii->r(), rJ);
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (ii->molecule() != molJ) totalEnergy += potentialMap_.force(typeJ, ii->globalTypeIndex(), rSq);
			else
			{
				scale = ii->molecule()->species()->scaling(ii->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale < 1.0e-3) totalEnergy += potentialMap_.force(typeJ, ii->globalTypeIndex(), rSq) * scale;
			}
		}
	}

	// Sum over processes if necessary
	if (sumOverProcesses)
	{
		if (loopContext == ProcessPool::OverGroupProcesses) processPool_.allSum(&totalEnergy, 1, ProcessPool::Group);
		else if (loopContext == ProcessPool::OverPoolProcesses) processPool_.allSum(&totalEnergy, 1);
	}
}

// Calculate forces between atom and list of neighbouring cells
void ForceKernel::forces(const Atom* i, OrderedPointerList<Atom>& neighbours, int flags, ProcessPool::LoopContext loopContext, bool sumOverProcesses)
{
	/*
	 * Calculate the energy between the supplied atom and list of neighbouring cells. Note that it is assumed that the supplied atom
	 * is in a cell which does *not* appear in the list.
	 */
#ifdef CHECKS
	if (i == NULL)
	{
		Messenger::error("NULL_POINTER - NULL atom pointer passed to ForceKernel::forces(Atom,OrderedPointerList,int,CommGroup).\n");
		return 0.0;
	}
#endif
	double totalEnergy = 0.0;
	Atom* jj;
	int j, start = 0, stride = 1;
	double scale;
	double rSq;
	Atom** neighbourAtoms = neighbours.objects();
	int nNeighbourAtoms = neighbours.nItems();
	
	// Grab some information on the supplied atom
	const int indexI = i->index(), typeI = i->globalTypeIndex();
	Molecule* moleculeI = i->molecule();
	const Species* spI = moleculeI->species();
	const Vec3<double> rI = i->r();

	// Get start/stride for specified loop context
	start = processPool_.interleavedLoopStart(loopContext);
	stride = processPool_.interleavedLoopStride(loopContext);

	// Loop over cell atoms
	if (flags&ForceKernel::ApplyMinimumImage)
	{
		// Loop over atom neighbours
		if (flags&ForceKernel::ExcludeSelfFlag) for (j=start; j<nNeighbourAtoms; j += stride)
		{
			jj = neighbourAtoms[j];
			if (i == jj) continue;

			// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
			rSq = box_->minimumDistanceSquared(rI, jj->r());
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq);
			else
			{
				if ((flags&ForceKernel::ExcludeIntraGreaterThan) && (i->moleculeAtomIndex() > jj->moleculeAtomIndex())) continue;
				scale = spI->scaling(i->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale > 1.0e-3) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq) * scale;
			}
		}
		else if (flags&ForceKernel::ExcludeGreaterThanEqualTo) for (j=start; j<nNeighbourAtoms; j += stride)
		{
			jj = neighbourAtoms[j];
			if (indexI >= jj->index()) continue;

			// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
			rSq = box_->minimumDistanceSquared(rI, jj->r());
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq);
			else
			{
				if ((flags&ForceKernel::ExcludeIntraGreaterThan) && (i->moleculeAtomIndex() > jj->moleculeAtomIndex())) continue;
				scale = spI->scaling(i->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale > 1.0e-3) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq) * scale;
			}
		}
		else for (j=start; j<nNeighbourAtoms; j += stride)
		{
			jj = neighbourAtoms[j];

			// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
			rSq = box_->minimumDistanceSquared(rI, jj->r());
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq);
			else
			{
				if ((flags&ForceKernel::ExcludeIntraGreaterThan) && (i->moleculeAtomIndex() > jj->moleculeAtomIndex())) continue;
				scale = spI->scaling(i->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale > 1.0e-3) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq) * scale;
			}
		}
	}
	else
	{
		// Loop over atom neighbours
		if (flags&ForceKernel::ExcludeSelfFlag) for (j=start; j<nNeighbourAtoms; j += stride)
		{
			jj = neighbourAtoms[j];
			if (i == jj) continue;

			// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
			rSq = (rI - jj->r()).magnitudeSq();
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq);
			else
			{
				scale = moleculeI->species()->scaling(i->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale > 1.0e-3) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq) * scale;
			}
		}
		else if (flags&ForceKernel::ExcludeGreaterThanEqualTo) for (j=start; j<nNeighbourAtoms; j += stride)
		{
			jj = neighbourAtoms[j];
			if (indexI >= jj->index()) continue;

			// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
			rSq = (rI - jj->r()).magnitudeSq();
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq);
			else
			{
				scale = moleculeI->species()->scaling(i->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale > 1.0e-3) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq) * scale;
			}
		}
		else for (j=start; j<nNeighbourAtoms; j += stride)
		{
			jj = neighbourAtoms[j];

			// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
			rSq = (rI - jj->r()).magnitudeSq();
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq);
			else
			{
				scale = moleculeI->species()->scaling(i->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale > 1.0e-3) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq) * scale;
			}
		}
	}

	// Sum over processes if necessary
	if (sumOverProcesses)
	{
		if (loopContext == ProcessPool::OverGroupProcesses) processPool_.allSum(&totalEnergy, 1, ProcessPool::Group);
		else if (loopContext == ProcessPool::OverPoolProcesses) processPool_.allSum(&totalEnergy, 1);
	}
}

// Calculate forces between Grain and list of Cells
void ForceKernel::forces(const Grain* grain, OrderedPointerList<Atom>& neighbours, bool applyMim, bool excludeIgeJ, ProcessPool::LoopContext loopContext, bool sumOverProcesses)
{
#ifdef CHECKS
	if (grain == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Grain pointer passed to ForceKernel::forces(Grain,RefList<Cell>,bool,bool,ParallelStyle).\n");
		return;
	}
#endif
	double totalEnergy = 0.0;
	Atom* ii, *jj;
	int i, j, start = 0, stride = 1;
	int indexI, typeI;
	double scale, rSq;
	Molecule* grainMol = grain->parent();
	Atom** neighbourAtoms = neighbours.objects();
	int nNeighbourAtoms = neighbours.nItems();
	Vec3<double> rI;

	// Get start/stride for specified loop context
	start = processPool_.interleavedLoopStart(loopContext);
	stride = processPool_.interleavedLoopStride(loopContext);

	if (applyMim)
	{
		// Loop over grain atoms
		for (i = 0; i<grain->nAtoms(); ++i)
		{
			ii = grain->atom(i);
			indexI = ii->index();
			rI = ii->r();
			typeI = ii->globalTypeIndex();

			// Loop over atom neighbours
			if (excludeIgeJ) for (j=start; j<nNeighbourAtoms; j += stride)
			{
				jj = neighbourAtoms[j];
				if (indexI >= jj->index()) continue;

				// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
				rSq = box_->minimumDistanceSquared(rI, jj->r());
				if (rSq > cutoffDistanceSquared_) continue;

				// Check for atoms in the same species
				if (grainMol != jj->molecule()) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq);
				else
				{
					scale = grainMol->species()->scaling(ii->moleculeAtomIndex(), jj->moleculeAtomIndex());
					if (scale > 1.0e-3) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq) * scale;
				}
			}
			else for (j=start; j<nNeighbourAtoms; j += stride)
			{
				jj = neighbourAtoms[j];
				if (indexI == jj->index()) continue;

				// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
				rSq = box_->minimumDistanceSquared(rI, jj->r());
				if (rSq > cutoffDistanceSquared_) continue;

				// Check for atoms in the same species
				if (grainMol != jj->molecule()) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq);
				else
				{
					scale = grainMol->species()->scaling(ii->moleculeAtomIndex(), jj->moleculeAtomIndex());
					if (scale > 1.0e-3) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq) * scale;
				}
			}
		}
	}
	else
	{
		// Loop over grain atoms
		for (i = 0; i<grain->nAtoms(); ++i)
		{
			ii = grain->atom(i);
			indexI = ii->index();
			rI = ii->r();
			typeI = ii->globalTypeIndex();

			// Loop over atom neighbours
			if (excludeIgeJ) for (j=start; j<nNeighbourAtoms; j += stride)
			{
				jj = neighbourAtoms[j];
				if (indexI >= jj->index()) continue;

				// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
				rSq = (rI - jj->r()).magnitudeSq();
				if (rSq > cutoffDistanceSquared_) continue;

				// Check for atoms in the same species
				if (grainMol != jj->molecule()) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq);
				else
				{
					scale = grainMol->species()->scaling(ii->moleculeAtomIndex(), jj->moleculeAtomIndex());
					if (scale > 1.0e-3) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq) * scale;
				}
			}
			else for (j=start; j<nNeighbourAtoms; j += stride)
			{
				jj = neighbourAtoms[j];
				if (indexI == jj->index()) continue;

				// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
				rSq = (rI - jj->r()).magnitudeSq();
				if (rSq > cutoffDistanceSquared_) continue;

				// Check for atoms in the same species
				if (grainMol != jj->molecule()) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq);
				else
				{
					scale = grainMol->species()->scaling(ii->moleculeAtomIndex(), jj->moleculeAtomIndex());
					if (scale > 1.0e-3) totalEnergy += potentialMap_.force(typeI, jj->globalTypeIndex(), rSq) * scale;
				}
			}
		}
	}

	// Sum over processes if necessary
	if (sumOverProcesses)
	{
		if (loopContext == ProcessPool::OverGroupProcesses) processPool_.allSum(&totalEnergy, 1, ProcessPool::Group);
		else if (loopContext == ProcessPool::OverPoolProcesses) processPool_.allSum(&totalEnergy, 1);
	}
}

// Calculate forces between atom and world
void ForceKernel::forces(const Atom* i, ProcessPool::LoopContext loopContext)
{
#ifdef CHECKS
	if (i == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Atom pointer passed to ForceKernel::forces(Atom,ParallelStyle).\n");
		return;
	}
#endif
	Cell* cellI = i->cell();
	forces(i, cellI->atoms(), ForceKernel::ExcludeSelfFlag, loopContext);
	forces(i, cellI->atomNeighbours(), ForceKernel::NoFlags, loopContext);
	forces(i, cellI->mimAtomNeighbours(), ForceKernel::ApplyMinimumImage, loopContext);
}

// Calculate forces between grain and world
void ForceKernel::forces(const Grain* grain, bool excludeIgtJ, ProcessPool::LoopContext loopContext)
{
#ifdef CHECKS
	if (grain == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Grain pointer passed to ForceKernel::forces(Grain,ParallelStyle).\n");
		return;
	}
#endif

	int i, j, nAtoms = grain->nAtoms();
	Vec3<double> rI;
	Molecule* grainMol = grain->parent();
	Species* grainSp = grainMol->species();
	Atom* ii, **atoms = grain->atoms();
	Cell* cellI;
	double scale;

	// Loop over grain atoms
	if (excludeIgtJ) for (i = 0; i<grain->nAtoms(); ++i)
	{
		ii = grain->atom(i);
		cellI = ii->cell();
		forces(ii, cellI->atoms(), ForceKernel::ExcludeGreaterThanEqualTo | ForceKernel::ExcludeIntraGreaterThan, loopContext);
		forces(ii, cellI->atomNeighbours(), ForceKernel::ExcludeGreaterThanEqualTo, loopContext);
		forces(ii, cellI->mimAtomNeighbours(), ForceKernel::ApplyMinimumImage | ForceKernel::ExcludeGreaterThanEqualTo, loopContext);
	}
	else for (i = 0; i<grain->nAtoms(); ++i)
	{
		ii = grain->atom(i);
		cellI = ii->cell();
		forces(ii, cellI->atoms(), ForceKernel::ExcludeSelfFlag | ForceKernel::ExcludeIntraGreaterThan, loopContext);
		forces(ii, cellI->atomNeighbours(), ForceKernel::ExcludeIntraGreaterThan, loopContext);
		forces(ii, cellI->mimAtomNeighbours(), ForceKernel::ApplyMinimumImage | ForceKernel::ExcludeIntraGreaterThan, loopContext);
	}
}

/*
 * Intramolecular Terms
 */

// Calculate Bond forces
void ForceKernel::forces(const Molecule* mol, const SpeciesBond* b)
{
	// Grab pointers to atoms involved in bond
	Atom* i = mol->atom(b->indexI());
	Atom* j = mol->atom(b->indexJ());

	// Determine whether we need to apply minimum image to the vector calculation
	Vec3<double> vecji;
	if (configuration_->useMim(i->cell(), j->cell())) vecji = box_->minimumVector(i, j);
	else vecji = j->r() - i->r();
	
	// Get distance and normalise vector ready for force calculation
	double distance = vecji.magAndNormalise();
	if (distance > 5.0) printf("SHITTTTT! %f\n", distance);

	// Determine final forces
	vecji *= b->force(distance);

	// Calculate forces
	int index = i->index();
	fx_[index] -= vecji.x;
	fy_[index] -= vecji.y;
	fz_[index] -= vecji.z;
	index = j->index();
	fx_[index] += vecji.x;
	fy_[index] += vecji.y;
	fz_[index] += vecji.z;
}

// Calculate Angle forces
void ForceKernel::forces(const Molecule* mol, const SpeciesAngle* a)
{
	double distance, angle, force, dp, magji, magjk;
	Vec3<double> vecji, vecjk, forcei, forcek;

	// Grab pointers to atoms involved in angle
	Atom* i = mol->atom(a->indexI());
	Atom* j = mol->atom(a->indexJ());
	Atom* k = mol->atom(a->indexK());

	// Determine whether we need to apply minimum image between 'j-i' and 'j-k'
	if (configuration_->useMim(j->cell(), i->cell())) vecji = configuration_->box()->minimumVector(j, i);
	else vecji = i->r() - j->r();
	if (configuration_->useMim(j->cell(), k->cell())) vecjk = configuration_->box()->minimumVector(j, k);
	else vecjk = k->r() - j->r();
	
	// Calculate angle
	magji = vecji.magAndNormalise();
	magjk = vecjk.magAndNormalise();
	angle = Box::angle(vecji, vecjk, dp);

	// Determine Angle force vectors for atoms
	force = a->force(angle);
	forcei = vecjk - vecji * dp;
	forcei *= force / magji;
	forcek = vecji - vecjk * dp;
	forcek *= force / magjk;
	
	// Store forces
	int index = i->index();
	fx_[index] += forcei.x;
	fy_[index] += forcei.y;
	fz_[index] += forcei.z;
	index = j->index();
	fx_[index] -= forcei.x + forcek.x;
	fy_[index] -= forcei.y + forcek.y;
	fz_[index] -= forcei.z + forcek.z;
	index = k->index();
	fx_[index] += forcek.x;
	fy_[index] += forcek.y;
	fz_[index] += forcek.z;
}
