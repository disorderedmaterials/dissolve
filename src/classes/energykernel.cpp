/*
	*** EnergyKernel
	*** src/classes/energykernel.cpp
	Copyright T. Youngs 2012-2016

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

#include "classes/energykernel.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/potentialmap.h"
#include "classes/molecule.h"
#include "classes/species.h"
#include "base/processpool.h"

// Constructor
EnergyKernel::EnergyKernel(ProcessPool& procPool, const Configuration* config, const PotentialMap& potentialMap, double energyCutoff) : processPool_(procPool), configuration_(config), potentialMap_(potentialMap)
{
	box_ = configuration_->box();
	cutoffDistanceSquared_ = (energyCutoff < 0.0 ? potentialMap_.rangeSquared() : energyCutoff*energyCutoff);
}

// Destructor
EnergyKernel::~EnergyKernel()
{
}

/*
 * Internal Routines
 */

// Return PairPotential energy between atoms provided as pointers (no minimum image calculation)
double EnergyKernel::energyWithoutMim(const Atom* i, const Atom* j)
{
// 	printf("EnergyKernel:;atoms(*,*) - energy %i-%i is %f at %f lit\n", min(i->index(),j->index()), max(i->index(),j->index()), potentialMap_.energy(i->globalTypeIndex(), j->globalTypeIndex(), (i->r() - j->r()).magnitudeSq()), (i->r() - j->r()).magnitudeSq());
	return potentialMap_.energy(i->globalTypeIndex(), j->globalTypeIndex(), (i->r() - j->r()).magnitudeSq());
}

// Return PairPotential energy between atoms provided as info and reference (minimum image calculation)
double EnergyKernel::energyWithoutMim(const int typeI, const Vec3<double>& rI, const Atom* j)
{
	return potentialMap_.energy(typeI, j->globalTypeIndex(), (rI - j->r()).magnitudeSq());
}

// Return PairPotential energy between atoms provided as references (no minimum image calculation)
double EnergyKernel::energyWithoutMim(const Atom& i, const Atom& j)
{
// 	printf("EnergyKernel::atoms(&,&) - energy %i-%i is %f at %f lit\n", min(i.index(),j.index()), max(i.index(),j.index()), potentialMap_.energy(i.globalTypeIndex(), j.globalTypeIndex(), (i.r() - j.r()).magnitudeSq()), (i.r() - j.r()).magnitudeSq());
	return potentialMap_.energy(i.globalTypeIndex(), j.globalTypeIndex(), (i.r() - j.r()).magnitudeSq());
}

// Return PairPotential energy between atoms provided as reference/pointer (no minimum image calculation)
double EnergyKernel::energyWithoutMim(const Atom& i, const Atom* j)
{
// 	printf("EnergyKernel - energy %i-%i is %f at %f\n", i.index(), j.index(), potentialMap_.energy(i, j, (i.r() - j->r()).magnitudeSq()), (i.r() - j->r()).magnitudeSq());
	return potentialMap_.energy(i.globalTypeIndex(), j->globalTypeIndex(), (i.r() - j->r()).magnitudeSq());
}

// Return PairPotential energy between atom (pointer) and grain provided (no minimum image calculation)
double EnergyKernel::energyWithoutMim(const Atom* i, const Grain* grain, bool excludeIgeJ)
{
	double totalEnergy = 0.0;
	int m, nAtomsJ = grain->nAtoms();
	if (excludeIgeJ)
	{
		for (m=0; m<nAtomsJ; ++m) if (i->index() < grain->atom(m)->index()) totalEnergy += energyWithoutMim(i, grain->atom(m));
	}
	else
	{
		for (m=0; m<nAtomsJ; ++m) totalEnergy += energyWithoutMim(i, grain->atom(m));
	}
	return totalEnergy;
}

// Return PairPotential energy between atom (reference) and grain provided (no minimum image calculation)
double EnergyKernel::energyWithoutMim(const Atom& i, const Grain* grain, bool excludeIgeJ)
{
	double totalEnergy = 0.0;
	int m, nAtomsJ = grain->nAtoms();
	if (excludeIgeJ)
	{
		for (m=0; m<nAtomsJ; ++m) if (i.index() < grain->atom(m)->index()) totalEnergy += energyWithoutMim(i, grain->atom(m));
	}
	else
	{
		for (m=0; m<nAtomsJ; ++m) totalEnergy += energyWithoutMim(i, grain->atom(m));
	}
	return totalEnergy;
}

// Return PairPotential energy between Grains provided (no minimum image calculation)
double EnergyKernel::energyWithoutMim(const Grain* grainI, const Grain* grainJ)
{
	int n, m, nAtomsI = grainI->nAtoms(), nAtomsJ = grainJ->nAtoms();
	Atom* i, *j;
	double totalEnergy = 0.0;
	if (grainI->parent() != grainJ->parent())
	{
		for (n=0; n<nAtomsI; ++n)
		{
			i = grainI->atom(n);
			for (m=0; m<nAtomsJ; ++m) totalEnergy += energyWithoutMim(i, grainJ->atom(m));
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
				totalEnergy += energyWithoutMim(i, j) * scale;
			}
		}
	}
	return totalEnergy;
}

// Return PairPotential energy between atoms provided as pointers (minimum image calculation)
double EnergyKernel::energyWithMim(const Atom* i, const Atom* j)
{
// 	Messenger::print("EnergyKernel::atoms(*,*) - energy %i-%i is %f at %f mim\n", min(i->index(),j->index()), max(i->index(),j->index()), potentialMap_.energy(i->globalTypeIndex(), j->globalTypeIndex(), box_->minimumDistanceSquared(j, i)), box_->minimumDistanceSquared(j, i));
	return potentialMap_.energy(i->globalTypeIndex(), j->globalTypeIndex(), box_->minimumDistanceSquared(j, i));
}

// Return PairPotential energy between atoms provided as info and reference (minimum image calculation)
double EnergyKernel::energyWithMim(const int typeI, const Vec3<double>& rI, const Atom* j)
{
	return potentialMap_.energy(typeI, j->globalTypeIndex(), box_->minimumDistanceSquared(j->r(), rI));
}

// Return PairPotential energy between atoms provided as references (minimum image calculation)
double EnergyKernel::energyWithMim(const Atom& i, const Atom& j)
{
// 	Messenger::print("EnergyKernel::atoms(&,&) - energy %i-%i is %f at %f mim\n", min(i.index(),j.index()), max(i.index(), j.index()), potentialMap_.energy(i.globalTypeIndex(), j.globalTypeIndex(), box_->minimumDistanceSquared(j, i)), box_->minimumDistanceSquared(j, i));
	return potentialMap_.energy(i.globalTypeIndex(), j.globalTypeIndex(), box_->minimumDistanceSquared(j, i));
}

// eturn PairPotential energy between atoms provided as reference/pointer (minimum image calculation)
double EnergyKernel::energyWithMim(const Atom& i, const Atom* j)
{
// 	Messenger::print("EnergyKernel - energy %i-%i is %f at %f\n", i->index(), j->index(), potentialMap_.energy(i, j, box_->minimumDistanceSquared(j, i)), box_->minimumDistanceSquared(j, i));
	return potentialMap_.energy(i.globalTypeIndex(), j->globalTypeIndex(), box_->minimumDistanceSquared(j, i.r()));
}

// Return PairPotential energy between atom (pointer) and grain provided (minimum image calculation)
double EnergyKernel::energyWithMim(const Atom* i, const Grain* grain, bool excludeIgeJ)
{
	double totalEnergy = 0.0;
	int m, nAtomsJ = grain->nAtoms();

	if (excludeIgeJ)
	{
		for (m=0; m<nAtomsJ; ++m) if (i->index() < grain->atom(m)->index()) totalEnergy += energyWithMim(i, grain->atom(m));
	}
	else
	{
		for (m=0; m<nAtomsJ; ++m) totalEnergy += energyWithMim(i, grain->atom(m));
	}
	return totalEnergy;
}

// Return PairPotential energy between atom (reference) and grain provided (minimum image calculation)
double EnergyKernel::energyWithMim(const Atom& i, const Grain* grain, bool excludeIgeJ)
{
	double totalEnergy = 0.0;
	int m, nAtomsJ = grain->nAtoms();

	if (excludeIgeJ)
	{
		for (m=0; m<nAtomsJ; ++m) if (i.index() < grain->atom(m)->index()) totalEnergy += energyWithMim(i, grain->atom(m));
	}
	else
	{
		for (m=0; m<nAtomsJ; ++m) totalEnergy += energyWithMim(i, grain->atom(m));
	}
	return totalEnergy;
}

// Return PairPotential energy between Grains provided (minimum image calculation)
double EnergyKernel::energyWithMim(const Grain* grainI, const Grain* grainJ)
{
	int n, m, nAtomsI = grainI->nAtoms(), nAtomsJ = grainJ->nAtoms();
	Atom* i, *j;
	double totalEnergy = 0.0;
	if (grainI->parent() != grainJ->parent())
	{
		for (n=0; n<nAtomsI; ++n)
		{
			i = grainI->atom(n);
			for (m=0; m<nAtomsJ; ++m) totalEnergy += energyWithMim(i, grainJ->atom(m));
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
				totalEnergy += energyWithMim(i, j) * scale;
			}
		}
	}
	return totalEnergy;
}

/*
 * PairPotential Terms
 */

// Return PairPotential energy between atoms (provided as pointers)
double EnergyKernel::energy(const Atom* i, const Atom* j, bool applyMim, bool excludeIgeJ)
{
#ifdef CHECKS
	if (i == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Atom pointer (i) passed to EnergyKernel::energy(Atom,Atom,bool,bool).\n");
		return 0.0;
	}
	if (j == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Atom pointer (j) passed to EnergyKernel::energy(Atom,Atom,bool,bool).\n");
		return 0.0;
	}
#endif
	// If Atoms are the same, we refuse to calculate
	if (i == j)
	{
// 		printf("Warning: Refusing to calculate self-energy in EnergyKernel::energy(Atom,Atom,bool,bool).\n");
		return 0.0;
	}
	
	// Check indices of Atoms if required
	if (excludeIgeJ && (i->index() >= j->index())) return 0.0;

	if (applyMim) return energyWithMim(i, j);
	else return energyWithoutMim(i, j);
}

// Return PairPotential energy between atoms in supplied cells
double EnergyKernel::energy(Cell* centralCell, Cell* otherCell, bool applyMim, bool excludeIgeJ, ProcessPool::CommGroup group)
{
#ifdef CHECKS
	if (centralCell == NULL)
	{
		Messenger::error("NULL_POINTER - NULL central Cell pointer passed to EnergyKernel::energy(Cell,Cell,bool,bool).\n");
		return 0.0;
	}
	if (otherCell == NULL)
	{
		Messenger::error("NULL_POINTER - NULL other Cell pointer passed to EnergyKernel::energy(Cell,Cell,bool,bool).\n");
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

	// Communication group determines loop/summation style
	start = processPool_.interleavedLoopStart(group);
	stride = processPool_.interleavedLoopStride(group);

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
				if (molI != jj->molecule()) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq);
				else
				{
					scale = molI->species()->scaling(ii->moleculeAtomIndex(), jj->moleculeAtomIndex());
					if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq) * scale;
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
				if (molI != jj->molecule()) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq);
				else
				{
					scale = molI->species()->scaling(ii->moleculeAtomIndex(), jj->moleculeAtomIndex());
					if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq) * scale;
				}
			}
		}
	}
	
	// Sum over processes if necessary
	if (group == ProcessPool::Group) processPool_.allSum(&totalEnergy, 1, ProcessPool::Group);
	else if (group == ProcessPool::Pool) processPool_.allSum(&totalEnergy, 1);

	return totalEnergy;
}

// Return PairPotential energy between cell and atomic neighbours
double EnergyKernel::energy(Cell* centralCell, bool excludeIgeJ, ProcessPool::CommGroup group)
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

	// Communication group determines loop/summation style
	start = processPool_.interleavedLoopStart(group);
	stride = processPool_.interleavedLoopStride(group);

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
			if (ii->molecule() != molJ) totalEnergy += potentialMap_.energy(typeJ, ii->globalTypeIndex(), rSq);
			else
			{
				scale = ii->molecule()->species()->scaling(ii->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(typeJ, ii->globalTypeIndex(), rSq);
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
			if (ii->molecule() != molJ) totalEnergy += potentialMap_.energy(typeJ, ii->globalTypeIndex(), rSq);
			else
			{
				scale = ii->molecule()->species()->scaling(ii->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale < 1.0e-3) totalEnergy += potentialMap_.energy(typeJ, ii->globalTypeIndex(), rSq) * scale;
			}
		}
	}

	// Sum over processes if necessary
	if (group == ProcessPool::Group) processPool_.allSum(&totalEnergy, 1, ProcessPool::Group);
	else if (group == ProcessPool::Pool) processPool_.allSum(&totalEnergy, 1);

	return totalEnergy;
}

// Return PairPotential energy between atom and list of neighbouring cells
double EnergyKernel::energy(const Atom* i, OrderedPointerList<Atom>& neighbours, int flags, ProcessPool::CommGroup group)
{
	/*
	 * Calculate the energy between the supplied atom and list of neighbouring cells. Note that it is assumed that the supplied atom
	 * is in a cell which does *not* appear in the list.
	 */
#ifdef CHECKS
	if (i == NULL)
	{
		Messenger::error("NULL_POINTER - NULL atom pointer passed to EnergyKernel::energy(Atom,OrderedPointerList,int,CommGroup).\n");
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

	// Communication group determines loop/summation style
	start = processPool_.interleavedLoopStart(group);
	stride = processPool_.interleavedLoopStride(group);

	// Loop over cell atoms
	if (flags&EnergyKernel::ApplyMinimumImage)
	{
		// Loop over atom neighbours
		if (flags&EnergyKernel::ExcludeSelfFlag) for (j=start; j<nNeighbourAtoms; j += stride)
		{
			jj = neighbourAtoms[j];
			if (i == jj) continue;

			// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
			rSq = box_->minimumDistanceSquared(rI, jj->r());
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq);
			else
			{
				if ((flags&EnergyKernel::ExcludeIntraGreaterThan) && (i->moleculeAtomIndex() > jj->moleculeAtomIndex())) continue;
				scale = spI->scaling(i->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq) * scale;
			}
		}
		else if (flags&EnergyKernel::ExcludeGreaterThanEqualTo) for (j=start; j<nNeighbourAtoms; j += stride)
		{
			jj = neighbourAtoms[j];
			if (indexI >= jj->index()) continue;

			// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
			rSq = box_->minimumDistanceSquared(rI, jj->r());
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq);
			else
			{
				if ((flags&EnergyKernel::ExcludeIntraGreaterThan) && (i->moleculeAtomIndex() > jj->moleculeAtomIndex())) continue;
				scale = spI->scaling(i->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq) * scale;
			}
		}
		else for (j=start; j<nNeighbourAtoms; j += stride)
		{
			jj = neighbourAtoms[j];

			// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
			rSq = box_->minimumDistanceSquared(rI, jj->r());
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq);
			else
			{
				if ((flags&EnergyKernel::ExcludeIntraGreaterThan) && (i->moleculeAtomIndex() > jj->moleculeAtomIndex())) continue;
				scale = spI->scaling(i->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq) * scale;
			}
		}
	}
	else
	{
		// Loop over atom neighbours
		if (flags&EnergyKernel::ExcludeSelfFlag) for (j=start; j<nNeighbourAtoms; j += stride)
		{
			jj = neighbourAtoms[j];
			if (i == jj) continue;

			// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
			rSq = (rI - jj->r()).magnitudeSq();
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq);
			else
			{
				scale = moleculeI->species()->scaling(i->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq) * scale;
			}
		}
		else if (flags&EnergyKernel::ExcludeGreaterThanEqualTo) for (j=start; j<nNeighbourAtoms; j += stride)
		{
			jj = neighbourAtoms[j];
			if (indexI >= jj->index()) continue;

			// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
			rSq = (rI - jj->r()).magnitudeSq();
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq);
			else
			{
				scale = moleculeI->species()->scaling(i->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq) * scale;
			}
		}
		else for (j=start; j<nNeighbourAtoms; j += stride)
		{
			jj = neighbourAtoms[j];

			// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
			rSq = (rI - jj->r()).magnitudeSq();
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq);
			else
			{
				scale = moleculeI->species()->scaling(i->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq) * scale;
			}
		}
	}

	// Sum over processes if necessary
	if (group == ProcessPool::Group) processPool_.allSum(&totalEnergy, 1, ProcessPool::Group);
	else if (group == ProcessPool::Pool) processPool_.allSum(&totalEnergy, 1);

	return totalEnergy;
}

// Return intermolecular energy between Grain and list of Cells
double EnergyKernel::energy(const Grain* grain, OrderedPointerList< Atom >& neighbours, bool applyMim, bool excludeIgeJ, ProcessPool::CommGroup group)
{
#ifdef CHECKS
	if (grain == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Grain pointer passed to EnergyKernel::energy(Grain,RefList<Cell>,bool,bool,ParallelStyle).\n");
		return 0.0;
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

	// Communication group determines loop/summation style
	start = processPool_.interleavedLoopStart(group);
	stride = processPool_.interleavedLoopStride(group);

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
				if (grainMol != jj->molecule()) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq);
				else
				{
					scale = grainMol->species()->scaling(ii->moleculeAtomIndex(), jj->moleculeAtomIndex());
					if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq) * scale;
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
				if (grainMol != jj->molecule()) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq);
				else
				{
					scale = grainMol->species()->scaling(ii->moleculeAtomIndex(), jj->moleculeAtomIndex());
					if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq) * scale;
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
				if (grainMol != jj->molecule()) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq);
				else
				{
					scale = grainMol->species()->scaling(ii->moleculeAtomIndex(), jj->moleculeAtomIndex());
					if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq) * scale;
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
				if (grainMol != jj->molecule()) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq);
				else
				{
					scale = grainMol->species()->scaling(ii->moleculeAtomIndex(), jj->moleculeAtomIndex());
					if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq) * scale;
				}
			}
		}
	}

	// Sum over processes if necessary
	if (group == ProcessPool::Group) processPool_.allSum(&totalEnergy, 1, ProcessPool::Group);
	else if (group == ProcessPool::Pool) processPool_.allSum(&totalEnergy, 1);

	return totalEnergy;
}

// Return PairPotential energy of atom with world
double EnergyKernel::energy(const Atom* i, ProcessPool::CommGroup group)
{
#ifdef CHECKS
	if (i == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Atom pointer passed to EnergyKernel::energy(Atom,ParallelStyle).\n");
		return 0.0;
	}
#endif
	double totalEnergy = 0.0;
	Cell* cellI = i->cell();
	totalEnergy = energy(i, cellI->atoms(), EnergyKernel::ExcludeSelfFlag, group);
	totalEnergy += energy(i, cellI->atomNeighbours(), EnergyKernel::NoFlags, group);
	totalEnergy += energy(i, cellI->mimAtomNeighbours(), EnergyKernel::ApplyMinimumImage, group);
	return totalEnergy;
}

// Return PairPotential energy of grain with world
double EnergyKernel::energy(const Grain* grain, bool excludeIgtJ, ProcessPool::CommGroup group)
{
#ifdef CHECKS
	if (grain == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Grain pointer passed to EnergyKernel::energy(Grain,ParallelStyle).\n");
		return 0.0;
	}
#endif

	double totalEnergy = 0.0;
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
		totalEnergy += energy(ii, cellI->atoms(), EnergyKernel::ExcludeGreaterThanEqualTo | EnergyKernel::ExcludeIntraGreaterThan, group);
		totalEnergy += energy(ii, cellI->atomNeighbours(), EnergyKernel::ExcludeGreaterThanEqualTo, group);
		totalEnergy += energy(ii, cellI->mimAtomNeighbours(), EnergyKernel::ApplyMinimumImage | EnergyKernel::ExcludeGreaterThanEqualTo, group);
	}
	else for (i = 0; i<grain->nAtoms(); ++i)
	{
		ii = grain->atom(i);
		cellI = ii->cell();
		totalEnergy += energy(ii, cellI->atoms(), EnergyKernel::ExcludeSelfFlag | EnergyKernel::ExcludeIntraGreaterThan, group);
		totalEnergy += energy(ii, cellI->atomNeighbours(), EnergyKernel::ExcludeIntraGreaterThan, group);
		totalEnergy += energy(ii, cellI->mimAtomNeighbours(), EnergyKernel::ApplyMinimumImage | EnergyKernel::ExcludeIntraGreaterThan, group);
	}

	return totalEnergy;
}

// Return molecular correction energy related to intramolecular terms involving supplied atom
double EnergyKernel::correct(const Atom* i)
{
	// Loop over atoms in molecule
	int j;
	int nMolAtoms = i->molecule()->nAtoms();
	int indexI = i->moleculeAtomIndex();
	int typeI = i->globalTypeIndex();
	Atom* jj, **atoms = i->molecule()->atoms();
	Species* sp = i->molecule()->species();
	double scale, rSq, correctionEnergy = 0.0;
	Vec3<double> rI = i->r();

	for (j = 0; j < nMolAtoms; ++j)
	{
		if (indexI == j) continue;
		scale = (1.0 - sp->scaling(indexI, j));
		if (scale > 1.0e-3)
		{
			jj = atoms[j];
			rSq = box_->minimumDistanceSquared(rI, jj->r());
			correctionEnergy += potentialMap_.energy(typeI, jj->globalTypeIndex(), rSq) * scale;
		}
	}

	return -correctionEnergy;
}

/*
 * Intramolecular Terms
 */

// Return Bond energy
double EnergyKernel::energy(const Molecule* mol, const SpeciesBond* b)
{
	// Determine whether we need to apply minimum image to the distance calculation
	Atom* i, *j;
	i = mol->atom(b->indexI());
	j = mol->atom(b->indexJ());
	if (configuration_->useMim(i->cell(), j->cell())) return b->energy(box_->minimumDistance(i, j));
	else return b->energy((i->r() - j->r()).magnitude());
}

// Return Angle energy
double EnergyKernel::energy(const Molecule* mol, const SpeciesAngle* a)
{
	Vec3<double> vecji, vecjk;

	// Determine whether we need to apply minimum image between 'j-i' and 'j-k'
	Atom* i, *j, *k;
	i = mol->atom(a->indexI());
	j = mol->atom(a->indexJ());
	k = mol->atom(a->indexK());
	if (configuration_->useMim(j->cell(), i->cell())) vecji = box_->minimumVector(j, i);
	else vecji = i->r() - j->r();
	if (configuration_->useMim(j->cell(), k->cell())) vecjk = box_->minimumVector(j, k);
	else vecjk = k->r() - j->r();
	
	// Normalise vectors
	vecji.normalise();
	vecjk.normalise();

	// Determine Angle energy
	return a->energy(Box::angle(vecji, vecjk));
}

// Return full intramolecular energy
double EnergyKernel::fullIntraEnergy(const Grain* grain, double termFactor)
{
	double intraEnergy = 0.0;
	Molecule* mol = grain->parent();

	// Add inter-Grain Bond terms
	for (RefListItem<SpeciesBond,int>* refBond = grain->source()->bondConnections(); refBond != NULL; refBond = refBond->next) intraEnergy += energy(mol, refBond->item) * termFactor;

	// Add inter-Grain Angle terms
	for (RefListItem<SpeciesAngle,int>* refAngle = grain->source()->angleConnections(); refAngle != NULL; refAngle = refAngle->next) intraEnergy += energy(mol, refAngle->item) * termFactor;

	// Add intra-Grain Bond terms
	for (RefListItem<SpeciesBond,int>* refBond = grain->source()->internalBonds(); refBond != NULL; refBond = refBond->next) intraEnergy += energy(mol, refBond->item) * termFactor;

	// Add intra-Grain Angle terms
	for (RefListItem<SpeciesAngle,int>* refAngle = grain->source()->internalAngles(); refAngle != NULL; refAngle = refAngle->next) intraEnergy += energy(mol, refAngle->item) * termFactor;

	return intraEnergy;
}

/*
 * Molecule Terms
 */

// Return total Molecule energy
double EnergyKernel::energy(Molecule* mol, ProcessPool::CommGroup group, bool halfPP, double ppFactorIntra, double termFactor)
{
	/*
	 * Calculates the total interaction energy of a Molecule with the rest of the system, and includes PairPotential and corrected intramolecular terms.
	 * The argument 'halfPP' controls whether the total energy returned is suitable for summation into a total system energy (halfPP = true) or whether a single
	 * Molecule energy is required (halfPP = false) and controls whether intermolecular Grain corrections are performed, as well as halving the total Grain energy.
	 */

	double totalEnergy = 0.0, grainEnergy = 0.0, intraEnergy = 0.0, interMolGrainCorrect = 0.0;

	// Accumulate total Grain energy with Cells.
	// This will double-count all Grain-Grain interactions within the Molecule, so we accumulate interMolGrainCorrect
	// to correct for this as we go along. Note that this part of the Grain energy is *not* multiplied by ppFactorGrain.
	Grain* grain;
	for (int n=0; n<mol->nGrains(); ++n)
	{
		grain = mol->grain(n);
// 		grainEnergy += energy(grain, grain->cell(), false, false, group);
// 		grainEnergy += energy(grain, grain->cell()->neighbours(), cutoffSq, false, group);
		printf("EnergyKernel::energy(Molecule*) is horribly broken......\n");
		
// 		if (!halfPP) for (int m=n+1; m<mol->nGrains(); ++m) interMolGrainCorrect -= energy(grain, mol->grain(m), false, false);
	}
	totalEnergy += (halfPP ? 0.5 : 1.0)*grainEnergy + interMolGrainCorrect;

	// Bond energy
	for (SpeciesBond* b = mol->species()->bonds(); b != NULL; b = b->next) intraEnergy += energy(mol, b) * termFactor;

	// Angle energy
	for (SpeciesAngle* a = mol->species()->angles(); a != NULL; a = a->next) intraEnergy += energy(mol, a) * termFactor;
// 	printf("grainEnergy = %f, corrIntraEnergy = %f, interMolGrainCorrect = %f\n", (halfPP ? 0.5 : 1.0)*grainEnergy, intraEnergy, interMolGrainCorrect);
	
	totalEnergy += termFactor*intraEnergy;

	return totalEnergy;
}
