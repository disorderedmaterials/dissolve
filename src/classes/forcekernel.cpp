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
#include "classes/angle.h"
#include "classes/bond.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/potentialmap.h"
#include "classes/molecule.h"
#include "classes/species.h"
#include "base/processpool.h"

// Constructor
ForceKernel::ForceKernel(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz, double cutoffDistance) : processPool_(procPool),  configuration_(cfg), cells_(cfg->cells()), potentialMap_(potentialMap), fx_(fx), fy_(fy), fz_(fz)
{
	box_ = configuration_->box();
	cutoffDistanceSquared_ = (cutoffDistance < 0.0 ? potentialMap_.range()*potentialMap_.range() : cutoffDistance*cutoffDistance);
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
	double distanceSq = force.magnitudeSq();
	if (distanceSq > cutoffDistanceSquared_) return;
	double r = sqrt(distanceSq);
	force /= r;
	force *= potentialMap_.force(i, j, r) * scale;

	int index = i->arrayIndex();
	fx_[index] += force.x;
	fy_[index] += force.y;
	fz_[index] += force.z;
	index = j->arrayIndex();
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
		for (m=0; m<nAtomsJ; ++m) if (i->arrayIndex() <= grain->atom(m)->arrayIndex()) forcesWithoutMim(i, grain->atom(m));
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
	if (grainI->molecule() != grainJ->molecule())
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
				scale = i->scaling(j);
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
	double distanceSq = force.magnitudeSq();
	if (distanceSq > cutoffDistanceSquared_) return;
	double r = sqrt(distanceSq);
	force /= r;
	force *= potentialMap_.force(i, j, r) * scale;

	int index = i->arrayIndex();
	fx_[index] += force.x;
	fy_[index] += force.y;
	fz_[index] += force.z;
	index = j->arrayIndex();
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
		for (m=0; m<nAtomsJ; ++m) if (i->arrayIndex() <= grain->atom(m)->arrayIndex()) forcesWithMim(i, grain->atom(m));
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
	if (grainI->molecule() != grainJ->molecule())
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
				scale = i->scaling(j);
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
	if (excludeIgeJ && (i >= j)) return;

	if (applyMim) forcesWithMim(i, j);
	else forcesWithoutMim(i, j);
}

// Calculate forces between atoms in supplied cells
void ForceKernel::forces(Cell* centralCell, Cell* otherCell, bool applyMim, bool excludeIgeJ, ProcessPool::LoopContext loopContext)
{
#ifdef CHECKS
	if (centralCell == NULL)
	{
		Messenger::error("NULL_POINTER - NULL central Cell pointer passed to ForceKernel::forces(Cell,Cell,bool,bool).\n");
		return;
	}
	if (otherCell == NULL)
	{
		Messenger::error("NULL_POINTER - NULL other Cell pointer passed to ForceKernel::forces(Cell,Cell,bool,bool).\n");
		return;
	}
#endif
	Atom** centralAtoms = centralCell->atoms().objects(), **otherAtoms = otherCell->atoms().objects();
	Atom* ii, *jj;
	Vec3<double> rI;
	Molecule* molI;
	int i, j;
	double scale;

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

				// Check exclusion of I > J (pointer comparison)
				if (excludeIgeJ && (ii >= jj)) continue;

				// Check for atoms in the same Molecule
				if (molI != jj->molecule()) forcesWithMim(ii, jj);
				else
				{
					scale = ii->scaling(jj);
					if (scale > 1.0e-3) forcesWithMim(ii, jj, scale);
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
				
				// Check exclusion of I > J (pointer comparison)
				if (excludeIgeJ && (ii >= jj)) continue;

				// Check for atoms in the same molecule
				if (molI != jj->molecule()) forcesWithoutMim(ii, jj);
				else
				{
					scale = ii->scaling(jj);
					if (scale > 1.0e-3) forcesWithoutMim(ii, jj, scale);
				}
			}
		}
	}
}

// Calculate forces between Cell and its neighbours
void ForceKernel::forces(Cell* cell, bool excludeIgeJ, ProcessPool::LoopContext loopContext)
{
	Atom** centralAtoms = cell->atoms().objects();
	Atom** otherAtoms;
	Atom* ii, *jj;
	Vec3<double> rJ, v;
	double rSq, r;
	int i, j;
	Cell* otherCell;
	Molecule* molJ;
	double scale;

	// Get start/stride for specified loop context
	int start = processPool_.interleavedLoopStart(loopContext);
	int stride = processPool_.interleavedLoopStride(loopContext);

		// Straight loop over Cells *not* requiring mim
	Cell** neighbours = cell->cellNeighbours();
	for (int n = 0; n<cell->nCellNeighbours(); ++n)
	{
		otherCell = neighbours[n];
		forces(cell, otherCell, false, excludeIgeJ, loopContext);
	}

	// Straight loop over Cells requiring mim
	Cell** mimNeighbours = cell->mimCellNeighbours();
	for (int n = 0; n<cell->nMimCellNeighbours(); ++n)
	{
		otherCell = mimNeighbours[n];
		forces(cell, otherCell, true, excludeIgeJ, loopContext);
	}
}

// Calculate forces between Atom and Cell
void ForceKernel::forces(const Atom* i, Cell* cell, int flags, ProcessPool::LoopContext loopContext)
{
#ifdef CHECKS
	if (i == NULL)
	{
		Messenger::error("NULL_POINTER - NULL atom pointer passed to ForceKernel::forces(Atom,OrderedPointerList,int,CommGroup).\n");
		return;
	}
#endif
	Atom* jj;
	int j;
	double scale;
	
	// Grab some information on the supplied atom
	Molecule* moleculeI = i->molecule();
	const Vec3<double> rI = i->r();

	// Grab the array of Atoms in the supplied Cell
	Atom** otherAtoms = cell->atoms().objects();
	int nOtherAtoms = cell->nAtoms();

	// Get start/stride for specified loop context
	int start = processPool_.interleavedLoopStart(loopContext);
	int stride = processPool_.interleavedLoopStride(loopContext);

	// Loop over cell atoms
	if (flags&KernelFlags::ApplyMinimumImageFlag)
	{
		// Loop over atom neighbours
		if (flags&KernelFlags::ExcludeSelfFlag) for (j=start; j<nOtherAtoms; j += stride)
		{
			// Grab other Atom pointer
			jj = otherAtoms[j];

			// Check for same atom
			if (i == jj) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) forcesWithMim(i, jj);
			else
			{
				scale = i->scaling(jj);
				if (scale > 1.0e-3) forcesWithMim(i, jj, scale);
			}
		}
		else if (flags&KernelFlags::ExcludeIGEJFlag) for (j=start; j<nOtherAtoms; j += stride)
		{
			// Grab other Atom pointer
			jj = otherAtoms[j];

			// Pointer comparison for i >= jj
			if (i >= jj) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) forcesWithMim(i, jj);
			else
			{
				scale = i->scaling(jj);
				if (scale > 1.0e-3) forcesWithMim(i, jj, scale);
			}
		}
		else if (flags&KernelFlags::ExcludeIntraIGEJFlag) for (j=start; j<nOtherAtoms; j += stride)
		{
			// Grab other Atom pointer
			jj = otherAtoms[j];

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) forcesWithMim(i, jj);
			else
			{
				// Pointer comparison for i >= jj
				if (i >= jj) continue;

				scale = i->scaling(jj);
				if (scale > 1.0e-3) forcesWithMim(i, jj, scale);
			}
		}
		else for (j=start; j<nOtherAtoms; j += stride)
		{
			// Grab other Atom pointer
			jj = otherAtoms[j];

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) forcesWithMim(i, jj);
			else
			{
				scale = i->scaling(jj);
				if (scale > 1.0e-3) forcesWithMim(i, jj, scale);
			}
		}
	}
	else
	{
		// Loop over atom neighbours
		if (flags&KernelFlags::ExcludeSelfFlag) for (j=start; j<nOtherAtoms; j += stride)
		{
			// Grab other Atom pointer
			jj = otherAtoms[j];

			// Check for same atom
			if (i == jj) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) forcesWithoutMim(i, jj);
			else
			{
				scale = i->scaling(jj);
				if (scale > 1.0e-3) forcesWithoutMim(i, jj, scale);
			}
		}
		else if (flags&KernelFlags::ExcludeIGEJFlag) for (j=start; j<nOtherAtoms; j += stride)
		{
			// Grab other Atom pointer
			jj = otherAtoms[j];

			// Pointer comparison for i >= jj
			if (i >= jj) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) forcesWithoutMim(i, jj);
			else
			{
				scale = i->scaling(jj);
				if (scale > 1.0e-3) forcesWithoutMim(i, jj, scale);
			}
		}
		else if (flags&KernelFlags::ExcludeIntraIGEJFlag) for (j=start; j<nOtherAtoms; j += stride)
		{
			// Grab other Atom pointer
			jj = otherAtoms[j];

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) forcesWithoutMim(i, jj);
			else
			{
				// Pointer comparison for i >= jj
				if (i >= jj) continue;

				scale = i->scaling(jj);
				if (scale > 1.0e-3) forcesWithoutMim(i, jj, scale);
			}
		}
		else for (j=start; j<nOtherAtoms; j += stride)
		{
			// Grab other Atom pointer
			jj = otherAtoms[j];

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) forcesWithoutMim(i, jj);
			else
			{
				scale = i->scaling(jj);
				if (scale > 1.0e-3) forcesWithoutMim(i, jj, scale);
			}
		}
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

	// This Atom with other Atoms in the same Cell
	forces(i, cellI, KernelFlags::ExcludeSelfFlag, loopContext);

	// This Atom with other Atoms in neighbour Cells
	Cell** neighbours = cellI->cellNeighbours();
	for (int n=0; n<cellI->nCellNeighbours(); ++n) forces(i, neighbours[n], KernelFlags::NoFlags, loopContext);

	// This Atom with other Atoms in neighbour Cells which require minimum image
	Cell** mimNeighbours = cellI->mimCellNeighbours();
	for (int n=0; n<cellI->nMimCellNeighbours(); ++n) forces(i, mimNeighbours[n], KernelFlags::ApplyMinimumImageFlag, loopContext);
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
		forces(ii, cellI, KernelFlags::ExcludeIGEJFlag, loopContext);

		// Cell neighbours not requiring minimum image
		Cell** neighbours = cellI->cellNeighbours();
		for (int n=0; n<cellI->nCellNeighbours(); ++n) forces(ii, neighbours[n], KernelFlags::ExcludeIGEJFlag, loopContext);

		// Cell neighbours requiring minimum image
		Cell** mimNeighbours = cellI->mimCellNeighbours();
		for (int n=0; n<cellI->nMimCellNeighbours(); ++n) forces(ii, mimNeighbours[n], KernelFlags::ApplyMinimumImageFlag | KernelFlags::ExcludeIGEJFlag, loopContext);
	}
	else for (i = 0; i<grain->nAtoms(); ++i)
	{
		ii = grain->atom(i);
		cellI = ii->cell();
		
		// This Atom with its own Cell
		forces(ii, cellI, KernelFlags::ExcludeSelfFlag, loopContext);

		// Cell neighbours not requiring minimum image
		Cell** neighbours = cellI->cellNeighbours();
		for (int n=0; n<cellI->nCellNeighbours(); ++n) forces(ii, neighbours[n], KernelFlags::NoFlags, loopContext);

		// Cell neighbours requiring minimum image
		Cell** mimNeighbours = cellI->mimCellNeighbours();
		for (int n=0; n<cellI->nMimCellNeighbours(); ++n) forces(ii, mimNeighbours[n], KernelFlags::ApplyMinimumImageFlag, loopContext);
	}
}

/*
 * Intramolecular Terms
 */

// Calculate Bond forces
void ForceKernel::forces(const Bond* b)
{
	// Grab pointers to atoms involved in bond
	Atom* i = b->i(), *j = b->j();

	// Determine whether we need to apply minimum image to the vector calculation
	Vec3<double> vecji;
	if (cells_.useMim(i->cell(), j->cell())) vecji = box_->minimumVector(i, j);
	else vecji = j->r() - i->r();
	
	// Get distance and normalise vector ready for force calculation
	double distance = vecji.magAndNormalise();

#ifdef CHECKS
	if (distance > 5.0) printf("!!! Long bond: %i-%i = %f Angstroms\n", i->arrayIndex(), j->arrayIndex(), distance);
#endif

	// Determine final forces
	vecji *= b->force(distance);

	// Calculate forces
	int index = i->arrayIndex();
	fx_[index] -= vecji.x;
	fy_[index] -= vecji.y;
	fz_[index] -= vecji.z;
	index = j->arrayIndex();
	fx_[index] += vecji.x;
	fy_[index] += vecji.y;
	fz_[index] += vecji.z;
}

// Calculate Angle forces
void ForceKernel::forces(const Angle* a)
{
	double distance, angle, force, dp, magji, magjk;
	Vec3<double> vecji, vecjk, forcei, forcek;

	// Grab pointers to atoms involved in angle
	Atom* i = a->i(), *j = a->j(), *k = a->k();

	// Determine whether we need to apply minimum image between 'j-i' and 'j-k'
	if (cells_.useMim(j->cell(), i->cell())) vecji = box_->minimumVector(j, i);
	else vecji = i->r() - j->r();
	if (cells_.useMim(j->cell(), k->cell())) vecjk = box_->minimumVector(j, k);
	else vecjk = k->r() - j->r();
	
	// Calculate angle
	magji = vecji.magAndNormalise();
	magjk = vecjk.magAndNormalise();
	angle = Box::angleInDegrees(vecji, vecjk, dp);

	// Determine Angle force vectors for atoms
	force = a->force(angle);
	forcei = vecjk - vecji * dp;
	forcei *= force / magji;
	forcek = vecji - vecjk * dp;
	forcek *= force / magjk;
	
	// Store forces
	int index = i->arrayIndex();
	fx_[index] += forcei.x;
	fy_[index] += forcei.y;
	fz_[index] += forcei.z;
	index = j->arrayIndex();
	fx_[index] -= forcei.x + forcek.x;
	fy_[index] -= forcei.y + forcek.y;
	fz_[index] -= forcei.z + forcek.z;
	index = k->arrayIndex();
	fx_[index] += forcek.x;
	fy_[index] += forcek.y;
	fz_[index] += forcek.z;
}

// Return Torsion force
void ForceKernel::forces(const Torsion* t)
{
	Vec3<double> vecji, vecjk, veckl, xpj, xpk, dcos_dxpj, dcos_dxpk, temp;
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

	// Calculate cross products and torsion angle formed (in radians)
	xpj = vecji * vecjk;
	xpk = veckl * vecjk;
	magxpj = xpj.magAndNormalise();
	magxpk = xpk.magAndNormalise();
	dp = xpj.dp(xpk);
	if (dp < -1.0) dp = -1.0;
	else if (dp > 1.0) dp = 1.0;
	phi = acos(dp);
	du_dphi = t->force(phi*DEGRAD);

	/* Construct derivatives of perpendicular axis (cross product) w.r.t. component vectors.
	 * E.g.
	 *	d (rij x rkj) 
	 *	------------- = rij[cp(n+2)] * U[cp(n+1)] - rij[cp(n+1)] * U[cp(n+2)]
	 *	d rkj[n]  
	 *
	 * where cp is a cylic permutation spanning {0,1,2} == {x,y,z}, and U[n] is a unit vector in the n direction.
	 * So,
	 *	d (rij x rkj) 
	 *	------------- = rij[2] * U[1] - rij[1] * U[2]
	 *	d rkj[0]  
	 *			= rij[z] * (0,1,0) - rij[y] * (0,0,1)
	 *
	 *			= (0,rij[z],0) - (0,0,rij[y])
	 *
	 *			= (0,rij[z],-rij[y])
	 */

	dxpj_dij.makeCrossProductMatrix(vecjk);
	temp = -vecji;
	dxpj_dkj.makeCrossProductMatrix(temp);
	temp = -veckl;
	dxpk_dkj.makeCrossProductMatrix(temp);
	dxpk_dlk.makeCrossProductMatrix(vecjk);

	// Construct derivatives of cos(phi) w.r.t. perpendicular axes
	dcos_dxpj = (xpk - xpj * dp) / magxpj;
	dcos_dxpk = (xpj - xpk * dp) / magxpk;

// 			printf("i-j-k-l %i-%i-%i-%i %f %f %f\n",i,j,k,l, phi, dphi_dcosphi, du_dphi);
	// Calculate forces on atom i
	int index = i->arrayIndex();
	fx_[index] += du_dphi * dcos_dxpj.dp(dxpj_dij.columnAsVec3(0));
	fy_[index] += du_dphi * dcos_dxpj.dp(dxpj_dij.columnAsVec3(1));
	fz_[index] += du_dphi * dcos_dxpj.dp(dxpj_dij.columnAsVec3(2));

	index = j->arrayIndex();
	fx_[index] += du_dphi * ( dcos_dxpj.dp( -dxpj_dij.columnAsVec3(0) - dxpj_dkj.columnAsVec3(0) ) - dcos_dxpk.dp(dxpk_dkj.columnAsVec3(0)) );
	fy_[index] += du_dphi * ( dcos_dxpj.dp( -dxpj_dij.columnAsVec3(1) - dxpj_dkj.columnAsVec3(1) ) - dcos_dxpk.dp(dxpk_dkj.columnAsVec3(1)) );
	fz_[index] += du_dphi * ( dcos_dxpj.dp( -dxpj_dij.columnAsVec3(2) - dxpj_dkj.columnAsVec3(2) ) - dcos_dxpk.dp(dxpk_dkj.columnAsVec3(2)) );

	index = k->arrayIndex();
	fx_[index] += du_dphi * ( dcos_dxpk.dp( dxpk_dkj.columnAsVec3(0) - dxpk_dlk.columnAsVec3(0) ) + dcos_dxpj.dp(dxpj_dkj.columnAsVec3(0)) );
	fy_[index] += du_dphi * ( dcos_dxpk.dp( dxpk_dkj.columnAsVec3(1) - dxpk_dlk.columnAsVec3(1) ) + dcos_dxpj.dp(dxpj_dkj.columnAsVec3(1)) );
	fz_[index] += du_dphi * ( dcos_dxpk.dp( dxpk_dkj.columnAsVec3(2) - dxpk_dlk.columnAsVec3(2) ) + dcos_dxpj.dp(dxpj_dkj.columnAsVec3(2)) );

	index = l->arrayIndex();
	fx_[index] += du_dphi * dcos_dxpk.dp(dxpk_dlk.columnAsVec3(0));
	fy_[index] += du_dphi * dcos_dxpk.dp(dxpk_dlk.columnAsVec3(1));
	fz_[index] += du_dphi * dcos_dxpk.dp(dxpk_dlk.columnAsVec3(2));

	return;
}
