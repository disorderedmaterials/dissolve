/*
	*** EnergyKernel
	*** src/classes/energykernel.cpp
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

#include "classes/energykernel.h"
#include "classes/angle.h"
#include "classes/bond.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/potentialmap.h"
#include "classes/molecule.h"
#include "classes/species.h"
#include "base/processpool.h"

// Constructor
EnergyKernel::EnergyKernel(ProcessPool& procPool, Configuration* config, const PotentialMap& potentialMap, double energyCutoff) : processPool_(procPool), configuration_(config), cells_(config->cells()), potentialMap_(potentialMap)
{
	box_ = configuration_->box();
	cutoffDistanceSquared_ = (energyCutoff < 0.0 ? potentialMap_.range()*potentialMap_.range() : energyCutoff*energyCutoff);
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
// 	printf("EnergyKernel:;atoms(*,*) - energy %i-%i is %f at %f lit\n", min(i->arrayIndex(),j->arrayIndex()), max(i->arrayIndex(),j->arrayIndex()), potentialMap_.energy(i->masterTypeIndex(), j->masterTypeIndex(), (i->r() - j->r()).magnitude()), (i->r() - j->r()).magnitude());
	return potentialMap_.energy(i, j, (i->r() - j->r()).magnitude());
}

// Return PairPotential energy between atom (pointer) and grain provided (no minimum image calculation)
double EnergyKernel::energyWithoutMim(const Atom* i, const Grain* grain, bool excludeIgeJ)
{
	double totalEnergy = 0.0;
	int m, nAtomsJ = grain->nAtoms();
	if (excludeIgeJ)
	{
		for (m=0; m<nAtomsJ; ++m) if (i->arrayIndex() < grain->atom(m)->arrayIndex()) totalEnergy += energyWithoutMim(i, grain->atom(m));
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
	if (grainI->molecule() != grainJ->molecule())
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
				scale = i->scaling(j);
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
// 	Messenger::print("EnergyKernel::atoms(*,*) - energy %i-%i is %f at %f mim\n", min(i->arrayIndex(),j->arrayIndex()), max(i->arrayIndex(),j->arrayIndex()), potentialMap_.energy(i->masterTypeIndex(), j->masterTypeIndex(), box_->minimumDistance(j, i)), box_->minimumDistance(j, i));
	return potentialMap_.energy(i, j, box_->minimumDistance(j, i));
}

// Return PairPotential energy between atom (pointer) and grain provided (minimum image calculation)
double EnergyKernel::energyWithMim(const Atom* i, const Grain* grain, bool excludeIgeJ)
{
	double totalEnergy = 0.0;
	int m, nAtomsJ = grain->nAtoms();

	if (excludeIgeJ)
	{
		for (m=0; m<nAtomsJ; ++m) if (i->arrayIndex() < grain->atom(m)->arrayIndex()) totalEnergy += energyWithMim(i, grain->atom(m));
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
	if (grainI->molecule() != grainJ->molecule())
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
				scale = i->scaling(j);
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
	if (excludeIgeJ && (i->arrayIndex() >= j->arrayIndex())) return 0.0;

	if (applyMim) return energyWithMim(i, j);
	else return energyWithoutMim(i, j);
}

// Return PairPotential energy between atoms in supplied cells
double EnergyKernel::energy(Cell* centralCell, Cell* otherCell, bool applyMim, bool excludeIgeJ, ProcessPool::LoopContext loopContext, bool sumOverProcesses)
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
	int i, typeI, indexI, j;
	double rSq, scale;

	// Get start/stride for specified loop context
	int start = processPool_.interleavedLoopStart(loopContext);
	int stride = processPool_.interleavedLoopStride(loopContext);

	// Loop over central cell atoms
	if (applyMim)
	{
		for (i = start; i < centralCell->atoms().nItems(); i += stride)
		{
			ii = centralAtoms[i];
			molI = ii->molecule();
			rI = ii->r();

			// Straight loop over other cell atoms
			for (j = 0; j < otherCell->atoms().nItems(); ++j)
			{
				jj = otherAtoms[j];

				// Check exclusion of I >= J
				if (excludeIgeJ && (ii >= jj)) continue;

				// Calculate rSquared distance betwenn atoms, and check it against the stored cutoff distance
				rSq = box_->minimumDistanceSquared(rI, jj->r());
				if (rSq > cutoffDistanceSquared_) continue;

				// Check for atoms in the same species
				if (molI != jj->molecule()) totalEnergy += potentialMap_.energy(ii, jj, sqrt(rSq));
				else
				{
					scale = ii->scaling(jj);
					if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(ii, jj, sqrt(rSq)) * scale;
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
			rI = ii->r();

			// Straight loop over other cell atoms
			for (j = 0; j < otherCell->atoms().nItems(); ++j)
			{
				jj = otherAtoms[j];
				
				// Check exclusion of I >= J
				if (excludeIgeJ && (ii >= jj)) continue;

				// Calculate rSquared distance betwenn atoms, and check it against the stored cutoff distance
				rSq = (rI - jj->r()).magnitudeSq();
				if (rSq > cutoffDistanceSquared_) continue;

				// Check for atoms in the same molecule
				if (molI != jj->molecule()) totalEnergy += potentialMap_.energy(ii, jj, sqrt(rSq));
				else
				{
					scale = ii->scaling(jj);
					if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(ii, jj, sqrt(rSq)) * scale;
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

	return totalEnergy;
}

// Return PairPotential energy between cell and atomic neighbours
double EnergyKernel::energy(Cell* centralCell, bool excludeIgeJ, ProcessPool::LoopContext loopContext, bool sumOverProcesses)
{
	double totalEnergy = 0.0;
	Atom** centralAtoms = centralCell->atoms().objects();
	Atom** otherAtoms;
	Atom* ii, *jj;
	Vec3<double> rJ;
	int i, j;
	Cell* otherCell;
	Molecule* molJ;
	double rSq, scale;

	// Get start/stride for specified loop context
	int start = processPool_.interleavedLoopStart(loopContext);
	int stride = processPool_.interleavedLoopStride(loopContext);

	// Straight loop over Cells *not* requiring mim
	Cell** neighbours = centralCell->cellNeighbours();
	for (int n = 0; n<centralCell->nCellNeighbours(); ++n)
	{
		otherCell = neighbours[n];
		otherAtoms = otherCell->atoms().objects();

		for (j = 0; j < otherCell->nAtoms(); ++j)
		{
			jj = otherAtoms[j];
			molJ = jj->molecule();
			rJ = jj->r();

			// Loop over central cell atoms
			for (i = start; i < centralCell->atoms().nItems(); i += stride)
			{
				ii = centralAtoms[i];

				// Check exclusion of I >= J (comparison by pointer)
				if (excludeIgeJ && (ii >= jj)) continue;

				// Calculate rSquared distance betwenn atoms, and check it against the stored cutoff distance
				rSq = (ii->r() - rJ).magnitudeSq();
				if (rSq > cutoffDistanceSquared_) continue;

				// Check for atoms in the same species
				if (ii->molecule() != molJ) totalEnergy += potentialMap_.energy(jj, ii, sqrt(rSq));
				else
				{
					scale = ii->scaling(jj);
					if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(jj, ii, sqrt(rSq));
				}
			}
		}
	}

	// Straight loop over Cells requiring mim
	Cell** mimNeighbours = centralCell->mimCellNeighbours();
	for (int n = 0; n<centralCell->nMimCellNeighbours(); ++n)
	{
		otherCell = mimNeighbours[n];
		otherAtoms = otherCell->atoms().objects();

		for (j = 0; j < otherCell->nAtoms(); ++j)
		{
			jj = otherAtoms[j];
			molJ = jj->molecule();
			rJ = jj->r();

			// Loop over central cell atoms
			for (i = start; i < centralCell->atoms().nItems(); i += stride)
			{
				ii = centralAtoms[i];

				// Check exclusion of I >= J (comparison by pointer)
				if (excludeIgeJ && (ii >= jj)) continue;

				// Calculate rSquared distance betwenn atoms, and check it against the stored cutoff distance
				rSq = box_->minimumDistanceSquared(ii->r(), rJ);
				if (rSq > cutoffDistanceSquared_) continue;

				// Check for atoms in the same species
				if (ii->molecule() != molJ) totalEnergy += potentialMap_.energy(jj, ii, sqrt(rSq));
				else
				{
					scale = ii->scaling(jj);
					if (scale < 1.0e-3) totalEnergy += potentialMap_.energy(jj, ii, sqrt(rSq)) * scale;
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

	return totalEnergy;
}

// Return PairPotential energy between Atom and Cell contents
double EnergyKernel::energy(const Atom* i, Cell* cell, int flags, ProcessPool::LoopContext loopContext, bool sumOverProcesses)
{
#ifdef CHECKS
	if (i == NULL)
	{
		Messenger::error("NULL_POINTER - NULL atom pointer passed to EnergyKernel::energy(Atom,Cell).\n");
		return 0.0;
	}
	if (cell == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Cell pointer passed to EnergyKernel::energy(Atom,Cell).\n");
		return 0.0;
	}
#endif
	double totalEnergy = 0.0;
	Atom* jj;
	int j;
	double rSq, scale;
	Atom** otherAtoms = cell->atoms().objects();
	int nOtherAtoms = cell->nAtoms();
	
	// Grab some information on the supplied Atom
	Molecule* moleculeI = i->molecule();
	const Vec3<double> rI = i->r();

	// Get start/stride for specified loop context
	int start = processPool_.interleavedLoopStart(loopContext);
	int stride = processPool_.interleavedLoopStride(loopContext);

	// Loop over cell atoms
	if (flags&KernelFlags::ApplyMinimumImageFlag)
	{
		// Loop over other Atoms
		if (flags&KernelFlags::ExcludeSelfFlag) for (j=start; j<nOtherAtoms; j += stride)
		{
			jj = otherAtoms[j];
			if (i == jj) continue;
			
			// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
			rSq = box_->minimumDistanceSquared(rI, jj->r());
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.energy(i, jj, sqrt(rSq));
			else
			{
				scale = i->scaling(jj);
				if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(i, jj, sqrt(rSq)) * scale;
			}
		}
		else if (flags&KernelFlags::ExcludeIGEJFlag) for (j=start; j<nOtherAtoms; j += stride)
		{
			jj = otherAtoms[j];

			// Pointer comparison
			if (i >= jj) continue;

			// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
			rSq = box_->minimumDistanceSquared(rI, jj->r());
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.energy(i, jj, sqrt(rSq));
			else
			{
				scale = i->scaling(jj);
				if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(i, jj, sqrt(rSq)) * scale;
			}
		}
		else for (j=start; j<nOtherAtoms; j += stride)
		{
			jj = otherAtoms[j];

			// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
			rSq = box_->minimumDistanceSquared(rI, jj->r());
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.energy(i, jj, sqrt(rSq));
			else
			{
				scale = i->scaling(jj);
				if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(i, jj, sqrt(rSq)) * scale;
			}
		}
	}
	else
	{
		// Loop over atom neighbours
		if (flags&KernelFlags::ExcludeSelfFlag) for (j=start; j<nOtherAtoms; j += stride)
		{
			jj = otherAtoms[j];
			if (i == jj) continue;

			// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
			rSq = (rI - jj->r()).magnitudeSq();
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.energy(i, jj, sqrt(rSq));
			else
			{
				scale = i->scaling(jj);
				if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(i, jj, sqrt(rSq)) * scale;
			}
		}
		else if (flags&KernelFlags::ExcludeIGEJFlag) for (j=start; j<nOtherAtoms; j += stride)
		{
			jj = otherAtoms[j];
			// Compare pointers
			if (i >= jj) continue;

			// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
			rSq = (rI - jj->r()).magnitudeSq();
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.energy(i, jj, sqrt(rSq));
			else
			{
				scale = i->scaling(jj);
				if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(i, jj, sqrt(rSq)) * scale;
			}
		}
		else for (j=start; j<nOtherAtoms; j += stride)
		{
			jj = otherAtoms[j];

			// Calculate rSquared distance between atoms, and check it against the stored cutoff distance
			rSq = (rI - jj->r()).magnitudeSq();
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.energy(i, jj, sqrt(rSq));
			else
			{
				scale = i->scaling(jj);
				if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(i, jj, sqrt(rSq)) * scale;
			}
		}
	}

	// Sum over processes if necessary
	if (sumOverProcesses)
	{
		if (loopContext == ProcessPool::OverGroupProcesses) processPool_.allSum(&totalEnergy, 1, ProcessPool::Group);
		else if (loopContext == ProcessPool::OverPoolProcesses) processPool_.allSum(&totalEnergy, 1);
	}

	return totalEnergy;
}

// Return PairPotential energy of Atom with world
double EnergyKernel::energy(const Atom* i, ProcessPool::LoopContext loopContext)
{
#ifdef CHECKS
	if (i == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Atom pointer passed to EnergyKernel::energy(Atom,ParallelStyle).\n");
		return 0.0;
	}
#endif
	Cell* cellI = i->cell();

	// This Atom with its own Cell
	double totalEnergy = energy(i, cellI, KernelFlags::ExcludeSelfFlag, loopContext);

	// Cell neighbours not requiring minimum image
	Cell** neighbours = cellI->cellNeighbours();
	for (int n=0; n<cellI->nCellNeighbours(); ++n) totalEnergy += energy(i, neighbours[n], KernelFlags::NoFlags, loopContext);

	// Cell neighbours requiring minimum image
	Cell** mimNeighbours = cellI->mimCellNeighbours();
	for (int n=0; n<cellI->nMimCellNeighbours(); ++n) totalEnergy += energy(i, mimNeighbours[n], KernelFlags::ApplyMinimumImageFlag, loopContext);

	return totalEnergy;
}

// Return PairPotential energy of Grain with world
double EnergyKernel::energy(const Grain* grain, bool excludeIgtJ, ProcessPool::LoopContext loopContext)
{
	// If no Grain is given, return zero
	if (grain == NULL) return 0.0;

	double totalEnergy = 0.0;
	int i, j, nAtoms = grain->nAtoms();
	Vec3<double> rI;
	Molecule* grainMol = grain->molecule();
	Atom* ii;
	Cell* cellI;
	double scale;

	// Loop over grain atoms
	if (excludeIgtJ) for (i = 0; i<grain->nAtoms(); ++i)
	{
		ii = grain->atom(i);
		cellI = ii->cell();

		// This Atom with its own Cell
		double totalEnergy = energy(ii, cellI, KernelFlags::ExcludeIGEJFlag, loopContext);

		// Cell neighbours not requiring minimum image
		Cell** neighbours = cellI->cellNeighbours();
		for (int n=0; n<cellI->nCellNeighbours(); ++n) totalEnergy += energy(ii, neighbours[n], KernelFlags::ExcludeIGEJFlag, loopContext);

		// Cell neighbours requiring minimum image
		Cell** mimNeighbours = cellI->mimCellNeighbours();
		for (int n=0; n<cellI->nMimCellNeighbours(); ++n) totalEnergy += energy(ii, mimNeighbours[n], KernelFlags::ApplyMinimumImageFlag | KernelFlags::ExcludeIGEJFlag, loopContext);
	}
	else for (i = 0; i<grain->nAtoms(); ++i)
	{
		ii = grain->atom(i);
		cellI = ii->cell();
		// This Atom with its own Cell
		double totalEnergy = energy(ii, cellI, KernelFlags::ExcludeSelfFlag, loopContext);

		// Cell neighbours not requiring minimum image
		Cell** neighbours = cellI->cellNeighbours();
		for (int n=0; n<cellI->nCellNeighbours(); ++n) totalEnergy += energy(ii, neighbours[n], KernelFlags::NoFlags, loopContext);

		// Cell neighbours requiring minimum image
		Cell** mimNeighbours = cellI->mimCellNeighbours();
		for (int n=0; n<cellI->nMimCellNeighbours(); ++n) totalEnergy += energy(ii, mimNeighbours[n], KernelFlags::ApplyMinimumImageFlag, loopContext);
	}

	return totalEnergy;
}

// Return molecular correction energy related to intramolecular terms involving supplied atom
double EnergyKernel::correct(const Atom* i)
{
	// Loop over atoms in molecule
	int nMolAtoms = i->molecule()->nAtoms();
	Atom* j, **atoms = i->molecule()->atoms();
	double scale, r, correctionEnergy = 0.0;
	Vec3<double> rI = i->r();

	for (int n = 0; n < nMolAtoms; ++n)
	{
		j = atoms[n];
		if (i == j) continue;
		scale = 1.0 - i->scaling(j);
		if (scale > 1.0e-3)
		{
			
			r = box_->minimumDistance(rI, j->r());
			correctionEnergy += potentialMap_.energy(i, j, r) * scale;
		}
	}

	return -correctionEnergy;
}

/*
 * Intramolecular Terms
 */

// Return Bond energy
double EnergyKernel::energy(const Bond* b)
{
	// Determine whether we need to apply minimum image to the distance calculation
	Atom* i = b->i(), *j = b->j();

	if (cells_.useMim(i->cell(), j->cell())) return b->energy(box_->minimumDistance(i, j));
	else return b->energy((i->r() - j->r()).magnitude());
}

// Return Angle energy
double EnergyKernel::energy(const Angle* a)
{
	Vec3<double> vecji, vecjk;

	// Determine whether we need to apply minimum image between 'j-i' and 'j-k'
	Atom* i = a->i(), *j = a->j(), *k = a->k();
	if (cells_.useMim(j->cell(), i->cell())) vecji = box_->minimumVector(j, i);
	else vecji = i->r() - j->r();
	if (cells_.useMim(j->cell(), k->cell())) vecjk = box_->minimumVector(j, k);
	else vecjk = k->r() - j->r();
	
	// Normalise vectors
	vecji.normalise();
	vecjk.normalise();

	// Determine Angle energy
	return a->energy(Box::angleInDegrees(vecji, vecjk));
}

// Return Torsion energy
double EnergyKernel::energy(const Torsion* t)
{
	Vec3<double> vecji, vecjk, veckl, xpj, xpk, dcos_dxpj, dcos_dxpk, temp, force;
	Matrix3 dxpj_dij, dxpj_dkj, dxpk_dkj, dxpk_dlk;
	double magxpj, magxpk, dp, phi, du_dphi;
	
	// Grab pointers to atoms involved in angle
	Atom* i = t->i(), *j = t->j(), *k = t->k(), *l = t->l();

	// Calculate vectors, ensuring we account for minimum image
	if (cells_.useMim(j->cell(), i->cell())) vecji = box_->minimumVector(j, i);
	else vecji = i->r() - j->r();
	if (cells_.useMim(j->cell(), k->cell())) vecjk = box_->minimumVector(j, k);
	else vecjk = k->r() - j->r();
	if (cells_.useMim(k->cell(), l->cell())) veckl = box_->minimumVector(k, l);
	else veckl = l->r() - k->r();

	return t->energy(Box::torsionInDegrees(vecji, vecjk, veckl));
}

// Return intramolecular energy for the supplied Atom
double EnergyKernel::intraEnergy(const Atom* i)
{
	// If no Atom is given, return zero
	if (i == NULL) return 0.0;

	double intraEnergy = 0.0;

	// Add energy from Bond terms
	RefListIterator<Bond,bool> bondIterator(i->bonds());
	while (Bond* b = bondIterator.iterate()) intraEnergy += energy(b);

	// Add energy from Angle terms
	RefListIterator<Angle,bool> angleIterator(i->angles());
	while (Angle* a = angleIterator.iterate()) intraEnergy += energy(a);

	// Add energy from Torsion terms
	RefListIterator<Torsion,double> torsionIterator(i->torsions());
	while (Torsion* t = torsionIterator.iterate()) intraEnergy += energy(t);

	return intraEnergy;
}
