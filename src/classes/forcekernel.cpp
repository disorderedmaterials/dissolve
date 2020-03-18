/*
	*** ForceKernel
	*** src/classes/forcekernel.cpp
	Copyright T. Youngs 2012-2020

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iterator>
#include "classes/configuration.h"
#include "classes/forcekernel.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/potentialmap.h"
#include "classes/molecule.h"
#include "classes/species.h"
#include "base/processpool.h"
#include "templates/orderedvector.h"

// Constructor
ForceKernel::ForceKernel(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz, double cutoffDistance) : configuration_(cfg), cells_(cfg->cells()), potentialMap_(potentialMap), fx_(fx), fy_(fy), fz_(fz), processPool_(procPool)
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

/*
 * PairPotential Terms
 */

// Calculate forces between atoms
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
void ForceKernel::forces(Cell* centralCell, Cell* otherCell, bool applyMim, bool excludeIgeJ, ProcessPool::DivisionStrategy strategy)
{
#ifdef CHECKS
	if (centralCell == NULL)
	{
		Messenger::error("NULL_POINTER - NULL central Cell pointer passed to ForceKernel::forces(Cell,Cell,bool,bool,DivisionStrategy).\n");
		return;
	}
	if (otherCell == NULL)
	{
		Messenger::error("NULL_POINTER - NULL other Cell pointer passed to ForceKernel::forces(Cell,Cell,bool,bool,DivisionStrategy).\n");
		return;
	}
#endif
	OrderedVector<Atom*>& centralAtoms = centralCell->atoms();
	OrderedVector<Atom*>& otherAtoms = otherCell->atoms();
	Atom* ii, *jj;
	Vec3<double> rI;
	Molecule* molI;
	int i, j, index;
	double scale;

	// Get start/stride for specified loop context
	int start = processPool_.interleavedLoopStart(strategy);
	int stride = processPool_.interleavedLoopStride(strategy);

	// Loop over central cell atoms
	if (applyMim)
	{
		for (auto indexI = centralAtoms.begin()+start;
		     indexI < centralAtoms.end();
		     indexI += stride)
		{
			ii = *indexI;
			molI = ii->molecule();
			rI = ii->r();

			// Straight loop over other cell atoms
			for (auto* jj : otherAtoms)
			{
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
	  for (auto indexI = centralCell->atoms().begin()+start;
	       indexI< centralCell->atoms().end();
	       indexI+= stride)
		{
			ii = *indexI;
			molI = ii->molecule();
			rI = ii->r();

			// Straight loop over other cell atoms
			for (auto* jj : otherAtoms)
			{
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
void ForceKernel::forces(Cell* cell, bool excludeIgeJ, ProcessPool::DivisionStrategy strategy)
{
	Vec3<double> rJ, v;

	// Straight loop over Cells *not* requiring mim
	for (auto* otherCell : cell->cellNeighbours())
	{
		forces(cell, otherCell, false, excludeIgeJ, strategy);
	}

	// Straight loop over Cells requiring mim
	for (auto* otherCell : cell->mimCellNeighbours())
	{
		forces(cell, otherCell, true, excludeIgeJ, strategy);
	}
}

// Calculate forces between Atom and Cell
void ForceKernel::forces(const Atom* i, Cell* cell, int flags, ProcessPool::DivisionStrategy strategy)
{
#ifdef CHECKS
	if (i == NULL)
	{
		Messenger::error("NULL_POINTER - NULL atom pointer passed to ForceKernel::forces(Atom,Cell,int,DivisionStrategy).\n");
		return;
	}
#endif
	Atom* jj;
	int j, index;
	double scale;

	// Grab some information on the supplied atom
	Molecule* moleculeI = i->molecule();

	// Grab the array of Atoms in the supplied Cell
	OrderedVector<Atom*>& otherAtoms = cell->atoms();
	int nOtherAtoms = cell->nAtoms();

	// Get start/stride for specified loop context
	int start = processPool_.interleavedLoopStart(strategy);
	int stride = processPool_.interleavedLoopStride(strategy);

	// Loop over cell atoms
	if (flags&KernelFlags::ApplyMinimumImageFlag)
	{
		// Loop over atom neighbours
		if (flags&KernelFlags::ExcludeSelfFlag) for (auto* jj : otherAtoms)
		{
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
		else if (flags&KernelFlags::ExcludeIGEJFlag) for (auto* jj : otherAtoms)
		{
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
		else if (flags&KernelFlags::ExcludeIntraIGEJFlag) {
                  for (auto indexJ = otherAtoms.begin()+start;
		       indexJ < otherAtoms.end();
		       indexJ += stride)
		  {
			// Grab other Atom pointer
                        jj = *indexJ;

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
		}
		else {
		    index = 0;
                    auto indexJ = otherAtoms.begin();
                    for (auto indexJ = otherAtoms.begin()+start;
			 indexJ < otherAtoms.end();
			 indexJ += stride)
		    {
			// Grab other Atom pointer
                        jj = *indexJ;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) forcesWithMim(i, jj);
			else
			{
				scale = i->scaling(jj);
				if (scale > 1.0e-3) forcesWithMim(i, jj, scale);
			}
		    }
		  }
	}
	else
	{
		// Loop over atom neighbours
		if (flags&KernelFlags::ExcludeSelfFlag) for (auto indexJ = otherAtoms.begin() + start;
							     indexJ < otherAtoms.end();
							     indexJ += stride)
		{
			// Grab other Atom pointer
			jj = *indexJ;

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
		else if (flags&KernelFlags::ExcludeIGEJFlag) for(auto indexJ = otherAtoms.begin()+start; indexJ < otherAtoms.end(); indexJ += stride)
		{
			// Grab other Atom pointer
			jj = *indexJ;

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
		else if (flags&KernelFlags::ExcludeIntraIGEJFlag) for(auto indexJ = otherAtoms.begin()+start; indexJ < otherAtoms.end(); indexJ += stride)
		{
			// Grab other Atom pointer
			jj = *indexJ;

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
		else for(auto indexJ = otherAtoms.begin()+start; indexJ < otherAtoms.end(); indexJ += stride)
		{
			// Grab other Atom pointer
			jj = *indexJ;

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
void ForceKernel::forces(const Atom* i, ProcessPool::DivisionStrategy strategy)
{
#ifdef CHECKS
	if (i == NULL)
	{
		Messenger::error("NULL_POINTER - NULL Atom pointer passed to ForceKernel::forces(Atom,DivisionStrategy).\n");
		return;
	}
#endif
	Cell* cellI = i->cell();

	// This Atom with other Atoms in the same Cell
	forces(i, cellI, KernelFlags::ExcludeSelfFlag, strategy);

	// This Atom with other Atoms in neighbour Cells
	for (auto* neighbour : cellI->cellNeighbours()) forces(i, neighbour, KernelFlags::NoFlags, strategy);

	// This Atom with other Atoms in neighbour Cells which require minimum image
	for (auto* neighbour : cellI->mimCellNeighbours()) forces(i, neighbour, KernelFlags::ApplyMinimumImageFlag, strategy);
}

/*
 * Intramolecular Terms
 */

// Calculate Bond forces
void ForceKernel::forces(const SpeciesBond* b, const Atom* i, const Atom* j)
{
	// Determine whether we need to apply minimum image to the vector calculation
	Vec3<double> vecji;
	if (i->cell()->mimRequired(j->cell())) vecji = box_->minimumVector(i, j);
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

// Calculate Bond forces for specified Atom only
void ForceKernel::forces(const Atom* onlyThis, const SpeciesBond* b, const Atom* i, const Atom* j)
{
#ifdef CHECKS
	if ((i != onlyThis) && (j != onlyThis))
	{
		Messenger::error("Forces requested for specific Atom in Bond, but neither Atom in the Bond is the one specified.\n");
		return;
	}
#endif

	// Determine whether we need to apply minimum image to the vector calculation
	Vec3<double> vecji;
	if (i->cell()->mimRequired(j->cell())) vecji = box_->minimumVector(i, j);
	else vecji = j->r() - i->r();

	// Get distance and normalise vector ready for force calculation
	double distance = vecji.magAndNormalise();

#ifdef CHECKS
	if (distance > 5.0) printf("!!! Long bond: %i-%i = %f Angstroms\n", i->arrayIndex(), j->arrayIndex(), distance);
#endif

	// Determine final forces
	vecji *= b->force(distance);

	// Calculate forces
	int index = onlyThis->arrayIndex();
	if (onlyThis == i)
	{
		fx_[index] -= vecji.x;
		fy_[index] -= vecji.y;
		fz_[index] -= vecji.z;
	}
	else
	{
		fx_[index] += vecji.x;
		fy_[index] += vecji.y;
		fz_[index] += vecji.z;
	}
}

// Calculate Angle forces
void ForceKernel::forces(const SpeciesAngle* a, const Atom* i, const Atom* j, const Atom* k)
{
	Vec3<double> vecji, vecjk;

	// Determine whether we need to apply minimum image between 'j-i' and 'j-k'
	if (j->cell()->mimRequired(i->cell())) vecji = box_->minimumVector(j, i);
	else vecji = i->r() - j->r();
	if (j->cell()->mimRequired(k->cell())) vecjk = box_->minimumVector(j, k);
	else vecjk = k->r() - j->r();

	// Calculate angle
	const double magji = vecji.magAndNormalise();
	const double magjk = vecjk.magAndNormalise();
	double dp;
	const double angle = Box::angleInDegrees(vecji, vecjk, dp);

	// Determine Angle force vectors for atoms
	const double force = a->force(angle);
	const Vec3<double> forcei = (vecjk - vecji * dp) * force / magji;
	const Vec3<double> forcek = (vecji - vecjk * dp) * force / magjk;

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

// Calculate Angle forces for specified Atom only
void ForceKernel::forces(const Atom* onlyThis, const SpeciesAngle* a, const Atom* i, const Atom* j, const Atom* k)
{
	Vec3<double> vecji, vecjk;

#ifdef CHECKS
	if ((i != onlyThis) && (j != onlyThis) && (k != onlyThis))
	{
		Messenger::error("Forces requested for specific Atom in Angle, but no Atom in the Angle is the one specified.\n");
		return;
	}
#endif

	// Determine whether we need to apply minimum image between 'j-i' and 'j-k'
	if (j->cell()->mimRequired(i->cell())) vecji = box_->minimumVector(j, i);
	else vecji = i->r() - j->r();
	if (j->cell()->mimRequired(k->cell())) vecjk = box_->minimumVector(j, k);
	else vecjk = k->r() - j->r();

	// Calculate angle
	const double magji = vecji.magAndNormalise();
	const double magjk = vecjk.magAndNormalise();
	double dp;
	const double angle = Box::angleInDegrees(vecji, vecjk, dp);

	// Determine Angle force vectors for atoms
	const double force = a->force(angle);
	const Vec3<double> forcei = (vecjk - vecji * dp) * force / magji;
	const Vec3<double> forcek = (vecji - vecjk * dp) * force / magjk;

	// Store forces
	int index = onlyThis->arrayIndex();
	if (onlyThis == i)
	{
		fx_[index] += forcei.x;
		fy_[index] += forcei.y;
		fz_[index] += forcei.z;
	}
	else if (onlyThis == j)
	{
		fx_[index] -= forcei.x + forcek.x;
		fy_[index] -= forcei.y + forcek.y;
		fz_[index] -= forcei.z + forcek.z;
	}
	else
	{
		fx_[index] += forcek.x;
		fy_[index] += forcek.y;
		fz_[index] += forcek.z;
	}
}

// Return Torsion force
void ForceKernel::forces(const SpeciesTorsion* t, const Atom* i, const Atom* j, const Atom* k, const Atom* l)
{
	// Calculate vectors, ensuring we account for minimum image
	Vec3<double> vecji, vecjk, veckl;
	if (j->cell()->mimRequired(i->cell())) vecji = box_->minimumVector(j, i);
	else vecji = i->r() - j->r();
	if (j->cell()->mimRequired(k->cell())) vecjk = box_->minimumVector(j, k);
	else vecjk = k->r() - j->r();
	if (k->cell()->mimRequired(l->cell())) veckl = box_->minimumVector(k, l);
	else veckl = l->r() - k->r();

	// Calculate cross products and torsion angle formed (in radians)
	Vec3<double> xpj = vecji * vecjk;
	Vec3<double> xpk = veckl * vecjk;
	const double magxpj = xpj.magAndNormalise();
	const double magxpk = xpk.magAndNormalise();
	double dp = xpj.dp(xpk);
	if (dp < -1.0) dp = -1.0;
	else if (dp > 1.0) dp = 1.0;
	const double phi = acos(dp);
	const double du_dphi = t->force(phi*DEGRAD);

	/*
	 * Construct derivatives of perpendicular axis (cross product) w.r.t. component vectors.
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

	Matrix3 dxpj_dij, dxpj_dkj, dxpk_dkj, dxpk_dlk;
	dxpj_dij.makeCrossProductMatrix(vecjk);
	Vec3<double> temp = -vecji;
	dxpj_dkj.makeCrossProductMatrix(temp);
	temp = -veckl;
	dxpk_dkj.makeCrossProductMatrix(temp);
	dxpk_dlk.makeCrossProductMatrix(vecjk);

	// Construct derivatives of cos(phi) w.r.t. perpendicular axes
	const Vec3<double> dcos_dxpj = (xpk - xpj * dp) / magxpj;
	const Vec3<double> dcos_dxpk = (xpj - xpk * dp) / magxpk;

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
}

// Calculate Torsion forces for specified Atom only
void ForceKernel::forces(const Atom* onlyThis, const SpeciesTorsion* t, const Atom* i, const Atom* j, const Atom* k, const Atom* l)
{
	// Calculate vectors, ensuring we account for minimum image
	Vec3<double> vecji, vecjk, veckl;
	if (j->cell()->mimRequired(i->cell())) vecji = box_->minimumVector(j, i);
	else vecji = i->r() - j->r();
	if (j->cell()->mimRequired(k->cell())) vecjk = box_->minimumVector(j, k);
	else vecjk = k->r() - j->r();
	if (k->cell()->mimRequired(l->cell())) veckl = box_->minimumVector(k, l);
	else veckl = l->r() - k->r();

	// Calculate cross products and torsion angle formed (in radians)
	Vec3<double> xpj = vecji * vecjk;
	Vec3<double> xpk = veckl * vecjk;
	const double magxpj = xpj.magAndNormalise();
	const double magxpk = xpk.magAndNormalise();
	double dp = xpj.dp(xpk);
	if (dp < -1.0) dp = -1.0;
	else if (dp > 1.0) dp = 1.0;
	const double phi = acos(dp);
	const double du_dphi = t->force(phi*DEGRAD);

	/*
	 * Construct derivatives of perpendicular axis (cross product) w.r.t. component vectors.
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

	Matrix3 dxpj_dij, dxpj_dkj, dxpk_dkj, dxpk_dlk;
	dxpj_dij.makeCrossProductMatrix(vecjk);
	Vec3<double> temp = -vecji;
	dxpj_dkj.makeCrossProductMatrix(temp);
	temp = -veckl;
	dxpk_dkj.makeCrossProductMatrix(temp);
	dxpk_dlk.makeCrossProductMatrix(vecjk);

	// Construct derivatives of cos(phi) w.r.t. perpendicular axes
	const Vec3<double> dcos_dxpj = (xpk - xpj * dp) / magxpj;
	const Vec3<double> dcos_dxpk = (xpj - xpk * dp) / magxpk;

// 			printf("i-j-k-l %i-%i-%i-%i %f %f %f\n",i,j,k,l, phi, dphi_dcosphi, du_dphi);
	// Calculate forces for specified atom
	int index = onlyThis->arrayIndex();
	if (onlyThis == i)
	{
		fx_[index] += du_dphi * dcos_dxpj.dp(dxpj_dij.columnAsVec3(0));
		fy_[index] += du_dphi * dcos_dxpj.dp(dxpj_dij.columnAsVec3(1));
		fz_[index] += du_dphi * dcos_dxpj.dp(dxpj_dij.columnAsVec3(2));
	}
	else if (onlyThis == j)
	{
		fx_[index] += du_dphi * ( dcos_dxpj.dp( -dxpj_dij.columnAsVec3(0) - dxpj_dkj.columnAsVec3(0) ) - dcos_dxpk.dp(dxpk_dkj.columnAsVec3(0)) );
		fy_[index] += du_dphi * ( dcos_dxpj.dp( -dxpj_dij.columnAsVec3(1) - dxpj_dkj.columnAsVec3(1) ) - dcos_dxpk.dp(dxpk_dkj.columnAsVec3(1)) );
		fz_[index] += du_dphi * ( dcos_dxpj.dp( -dxpj_dij.columnAsVec3(2) - dxpj_dkj.columnAsVec3(2) ) - dcos_dxpk.dp(dxpk_dkj.columnAsVec3(2)) );
	}
	else if (onlyThis == k)
	{
		fx_[index] += du_dphi * ( dcos_dxpk.dp( dxpk_dkj.columnAsVec3(0) - dxpk_dlk.columnAsVec3(0) ) + dcos_dxpj.dp(dxpj_dkj.columnAsVec3(0)) );
		fy_[index] += du_dphi * ( dcos_dxpk.dp( dxpk_dkj.columnAsVec3(1) - dxpk_dlk.columnAsVec3(1) ) + dcos_dxpj.dp(dxpj_dkj.columnAsVec3(1)) );
		fz_[index] += du_dphi * ( dcos_dxpk.dp( dxpk_dkj.columnAsVec3(2) - dxpk_dlk.columnAsVec3(2) ) + dcos_dxpj.dp(dxpj_dkj.columnAsVec3(2)) );
	}
	else
	{
		fx_[index] += du_dphi * dcos_dxpk.dp(dxpk_dlk.columnAsVec3(0));
		fy_[index] += du_dphi * dcos_dxpk.dp(dxpk_dlk.columnAsVec3(1));
		fz_[index] += du_dphi * dcos_dxpk.dp(dxpk_dlk.columnAsVec3(2));
	}
}
