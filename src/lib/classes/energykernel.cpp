/*
	*** EnergyKernel
	*** src/lib/classes/energykernel.cpp
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

#include "classes/energykernel.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/configuration.h"
#include "classes/potentialmap.h"
#include "classes/molecule.h"
#include "classes/species.h"
#include "base/comms.h"

/*!
 * \brief Constructor
 * \details Constructor for EnergyKernel.
 */
EnergyKernel::EnergyKernel(const Configuration& config, const PotentialMap& potentialMap, double energyCutoff) : configuration_(config), potentialMap_(potentialMap)
{
	box_ = config.box();
	cutoffDistanceSquared_ = (energyCutoff < 0.0 ? potentialMap_.rangeSquared() : energyCutoff);
}

/*!
 * \brief Destructor
 * \details Destructor for EnergyKernel. 
 */
EnergyKernel::~EnergyKernel()
{
}

/*
// Internal Routines
*/

/*!
 * \brief Return PairPotential energy between atoms provided as pointers (no minimum image calculation)
 */
double EnergyKernel::energyWithoutMim(const Atom* i, const Atom* j)
{
// 	printf("EnergyKernel:;atoms(*,*) - energy %i-%i is %f at %f lit\n", min(i->index(),j->index()), max(i->index(),j->index()), potentialMap_.energy(i->atomTypeIndex(), j->atomTypeIndex(), (i->r() - j->r()).magnitudeSq()), (i->r() - j->r()).magnitudeSq());
	return potentialMap_.energy(i->atomTypeIndex(), j->atomTypeIndex(), (i->r() - j->r()).magnitudeSq());
}

/*!
 * \brief Return PairPotential energy between atoms provided as info and reference (minimum image calculation)
 */
double EnergyKernel::energyWithoutMim(const int typeI, const Vec3<double>& rI, const Atom* j)
{
	return potentialMap_.energy(typeI, j->atomTypeIndex(), (rI - j->r()).magnitudeSq());
}

/*!
 * \brief Return PairPotential energy between atoms provided as references (no minimum image calculation)
 */
double EnergyKernel::energyWithoutMim(const Atom& i, const Atom& j)
{
// 	printf("EnergyKernel::atoms(&,&) - energy %i-%i is %f at %f lit\n", min(i.index(),j.index()), max(i.index(),j.index()), potentialMap_.energy(i.atomTypeIndex(), j.atomTypeIndex(), (i.r() - j.r()).magnitudeSq()), (i.r() - j.r()).magnitudeSq());
	return potentialMap_.energy(i.atomTypeIndex(), j.atomTypeIndex(), (i.r() - j.r()).magnitudeSq());
}

/*!
 * \brief Return PairPotential energy between atoms provided as reference/pointer (no minimum image calculation)
 */
double EnergyKernel::energyWithoutMim(const Atom& i, const Atom* j)
{
// 	printf("EnergyKernel - energy %i-%i is %f at %f\n", i.index(), j.index(), potentialMap_.energy(i, j, (i.r() - j->r()).magnitudeSq()), (i.r() - j->r()).magnitudeSq());
	return potentialMap_.energy(i.atomTypeIndex(), j->atomTypeIndex(), (i.r() - j->r()).magnitudeSq());
}

/*!
 * \brief Return PairPotential energy between atom (pointer) and grain provided (no minimum image calculation)
 */
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

/*!
 * \brief Return PairPotential energy between atom (reference) and grain provided (no minimum image calculation)
 */
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

/*!
 * \brief Return PairPotential energy between Grains provided (no minimum image calculation)
 */
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

/*!
 * \brief Return PairPotential energy between atoms provided as pointers (minimum image calculation)
 */
double EnergyKernel::energyWithMim(const Atom* i, const Atom* j)
{
// 	msg.print("EnergyKernel::atoms(*,*) - energy %i-%i is %f at %f mim\n", min(i->index(),j->index()), max(i->index(),j->index()), potentialMap_.energy(i->atomTypeIndex(), j->atomTypeIndex(), box_->minimumDistanceSquared(j, i)), box_->minimumDistanceSquared(j, i));
	return potentialMap_.energy(i->atomTypeIndex(), j->atomTypeIndex(), box_->minimumDistanceSquared(j, i));
}

/*!
 * \brief Return PairPotential energy between atoms provided as info and reference (minimum image calculation)
 */
double EnergyKernel::energyWithMim(const int typeI, const Vec3<double>& rI, const Atom* j)
{
	return potentialMap_.energy(typeI, j->atomTypeIndex(), box_->minimumDistanceSquared(j->r(), rI));
}

/*!
 * \brief Return PairPotential energy between atoms provided as references (minimum image calculation)
 */
double EnergyKernel::energyWithMim(const Atom& i, const Atom& j)
{
// 	msg.print("EnergyKernel::atoms(&,&) - energy %i-%i is %f at %f mim\n", min(i.index(),j.index()), max(i.index(), j.index()), potentialMap_.energy(i.atomTypeIndex(), j.atomTypeIndex(), box_->minimumDistanceSquared(j, i)), box_->minimumDistanceSquared(j, i));
	return potentialMap_.energy(i.atomTypeIndex(), j.atomTypeIndex(), box_->minimumDistanceSquared(j, i));
}

/*!
 * \brief Return PairPotential energy between atoms provided as reference/pointer (minimum image calculation)
 */
double EnergyKernel::energyWithMim(const Atom& i, const Atom* j)
{
// 	msg.print("EnergyKernel - energy %i-%i is %f at %f\n", i->index(), j->index(), potentialMap_.energy(i, j, box_->minimumDistanceSquared(j, i)), box_->minimumDistanceSquared(j, i));
	return potentialMap_.energy(i.atomTypeIndex(), j->atomTypeIndex(), box_->minimumDistanceSquared(j, i.r()));
}

/*!
 * \brief Return PairPotential energy between atom (pointer) and grain provided (minimum image calculation)
 */
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

/*!
 * \brief Return PairPotential energy between atom (reference) and grain provided (minimum image calculation)
 */
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

/*!
 * \brief Return PairPotential energy between Grains provided (minimum image calculation)
 */
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
// PairPotential Terms
*/

/*!
 * \brief Return PairPotential energy between atoms (provided as pointers)
 */
double EnergyKernel::energy(const Atom* i, const Atom* j, bool applyMim, bool excludeIgeJ)
{
#ifdef CHECKS
	if (i == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer (i) passed to EnergyKernel::energy(Atom,Atom,bool,bool).\n");
		return 0.0;
	}
	if (j == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer (j) passed to EnergyKernel::energy(Atom,Atom,bool,bool).\n");
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

/*!
 * \brief Return PairPotential energy between atoms (provided as references)
 */
double EnergyKernel::energy(const Atom& i, const Atom& j, bool applyMim, bool excludeIgeJ)
{
	// If Atoms are the same, we refuse to calculate
	if (i.index() == j.index())
	{
// 		printf("Warning: Refusing to calculate self-energy in EnergyKernel::energy(Atom,Atom,bool,bool).\n");
		return 0.0;
	}
	
	// Check indices of Atoms if required
	if (excludeIgeJ && (i.index() >= j.index())) return 0.0;

	if (applyMim) return energyWithMim(i, j);
	else return energyWithoutMim(i, j);
}

/*!
 * \brief Return PairPotential energy between atom (pointer) and grain provided
 */
double EnergyKernel::energy(const Atom* i, const Grain* grain, bool applyMim, bool excludeIgeJ)
{
#ifdef CHECKS
	if (i == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer passed to EnergyKernel::energy(Atom,Grain,bool,bool).\n");
		return 0.0;
	}
	if (grain == NULL)
	{
		msg.error("NULL_POINTER - NULL Grain pointer passed to EnergyKernel::energy(Atom,Grain,bool,bool).\n");
		return 0.0;
	}
#endif
	if (applyMim) return energyWithMim(i, grain, excludeIgeJ);
	else return energyWithoutMim(i, grain, excludeIgeJ);
}

/*!
 * \brief Return PairPotential energy between atom (reference) and grain provided
 */
double EnergyKernel::energy(const Atom& i, const Grain* grain, bool applyMim, bool excludeIgeJ)
{
#ifdef CHECKS
	if (grain == NULL)
	{
		msg.error("NULL_POINTER - NULL Grain pointer passed to EnergyKernel::energy(Atom,Grain,bool,bool).\n");
		return 0.0;
	}
#endif
	if (applyMim) return energyWithMim(i, grain, excludeIgeJ);
	else return energyWithoutMim(i, grain, excludeIgeJ);
}

/*!
 * \brief Return PairPotential energy between Grains provided
 */
double EnergyKernel::energy(const Grain* grainI, const Grain* grainJ, bool applyMim, bool excludeIgeJ)
{
#ifdef CHECKS
	if (grainI == NULL)
	{
		msg.error("NULL_POINTER - NULL Grain pointer (grainI) passed to EnergyKernel::energy(Grain,Grain,bool,bool).\n");
		return 0.0;
	}
	if (grainJ == NULL)
	{
		msg.error("NULL_POINTER - NULL Grain pointer (grainJ) passed to EnergyKernel::energy(Grain,Grain,bool,bool).\n");
		return 0.0;
	}
#endif
	// If Grains are the same, we refuse to calculate
	if (grainI == grainJ) return 0.0;

	// Check Grain-Grain distance
// 	double rSq;
// 	if (applyMim) rSq = box_->minimumDistanceSquared(grainI->centre(), grainJ->centre());
// 	else rSq = (grainI->centre() - grainJ->centre()).magnitudeSq();
// 	msg.print("GG %3i %3i MIM=%i EXC=%i rSq=%f (%f,%f,%f) (%f,%f,%f)\n", grainI->index(), grainJ->index(), applyMim, excludeIgtJ, rSq, grainI->centre().x, grainI->centre().y, grainI->centre().z, grainJ->centre().x, grainJ->centre().y, grainJ->centre().z);
// 	if (rSq > cutoffSq) return 0.0;
	
	// Check exclusion of I > J
	if (excludeIgeJ && (grainI->index() >= grainJ->index())) return 0.0;

	if (applyMim) return energyWithMim(grainI, grainJ);
	else return energyWithoutMim(grainI, grainJ);
}

/*!
 * \brief Return PairPotential energy between atoms in supplied cells
 * \details Calculate the energy between the atoms in the specified cells, applying minimum image calculations if necessary.
 */
double EnergyKernel::energy(Cell* centralCell, Cell* otherCell, bool applyMim, bool excludeIgeJ, DUQComm::CommGroup group)
{
#ifdef CHECKS
	if (centralCell == NULL)
	{
		msg.error("NULL_POINTER - NULL central Cell pointer passed to EnergyKernel::energy(Cell,Cell,bool,bool).\n");
		return 0.0;
	}
	if (otherCell == NULL)
	{
		msg.error("NULL_POINTER - NULL other Cell pointer passed to EnergyKernel::energy(Cell,Cell,bool,bool).\n");
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
	start = Comm.interleavedLoopStart(group);
	stride = Comm.interleavedLoopStride(group);

	// Loop over central cell atoms
	if (applyMim)
	{
		for (i = start; i < centralCell->atoms().nItems(); i += stride)
		{
			ii = centralAtoms[i];
			molI = ii->molecule();
			indexI = ii->index();
			typeI = ii->atomTypeIndex();
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
				if (molI != jj->molecule()) totalEnergy += potentialMap_.energy(typeI, jj->atomTypeIndex(), rSq);
				else
				{
					scale = molI->species()->scaling(ii->moleculeAtomIndex(), jj->moleculeAtomIndex());
					if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(typeI, jj->atomTypeIndex(), rSq) * scale;
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
			typeI = ii->atomTypeIndex();
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
				if (molI != jj->molecule()) totalEnergy += potentialMap_.energy(typeI, jj->atomTypeIndex(), rSq);
				else
				{
					scale = molI->species()->scaling(ii->moleculeAtomIndex(), jj->moleculeAtomIndex());
					if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(typeI, jj->atomTypeIndex(), rSq) * scale;
				}
			}
		}
	}
	
	// Sum over processes if necessary
	if (group == DUQComm::Group) Comm.allSum(&totalEnergy, 1, DUQComm::Group);
	else if (group == DUQComm::World) Comm.allSum(&totalEnergy, 1);

	return totalEnergy;
}

/*!
 * \brief Return PairPotential energy between cell and atomic neighbours
 */
double EnergyKernel::energy(Cell* centralCell, bool excludeIgeJ, DUQComm::CommGroup group)
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
	start = Comm.interleavedLoopStart(group);
	stride = Comm.interleavedLoopStride(group);

	// Straight loop over atoms *not* requiring mim
	for (j = 0; j < centralCell->atomNeighbours().nItems(); ++j)
	{
		jj = neighbours[j];
		molJ = jj->molecule();
		indexJ = jj->index();
		typeJ = jj->atomTypeIndex();
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
			if (ii->molecule() != molJ) totalEnergy += potentialMap_.energy(typeJ, ii->atomTypeIndex(), rSq);
			else
			{
				scale = ii->molecule()->species()->scaling(ii->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(typeJ, ii->atomTypeIndex(), rSq);
			}
		}
	}

	// Straight loop over atoms requiring mim
	for (j = 0; j < centralCell->mimAtomNeighbours().nItems(); ++j)
	{
		jj = mimNeighbours[j];
		molJ = jj->molecule();
		indexJ = jj->index();
		typeJ = jj->atomTypeIndex();
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
			if (ii->molecule() != molJ) totalEnergy += potentialMap_.energy(typeJ, ii->atomTypeIndex(), rSq);
			else
			{
				scale = ii->molecule()->species()->scaling(ii->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale < 1.0e-3) totalEnergy += potentialMap_.energy(typeJ, ii->atomTypeIndex(), rSq) * scale;
			}
		}
	}

	// Sum over processes if necessary
	if (group == DUQComm::Group) Comm.allSum(&totalEnergy, 1, DUQComm::Group);
	else if (group == DUQComm::World) Comm.allSum(&totalEnergy, 1);

	return totalEnergy;
}

/*!
 * \brief Return PairPotential energy between atom and cell
 * \details Calculate the energy between the atom and the supplied cell, applying minimum image calculations if necessary.
 */
double EnergyKernel::energy(const Atom* i, Cell* cell, bool applyMim, DUQComm::CommGroup group)
{
#ifdef CHECKS
	if (cell == NULL)
	{
		msg.error("NULL_POINTER - NULL cell pointer passed to EnergyKernel::energy(Atom,Cell,bool,bool).\n");
		return 0.0;
	}
#endif
	double totalEnergy = 0.0;
	Atom** cellAtoms = cell->atoms().objects();
	Atom* jj;
	int j, start = 0, stride = 1;
	double rSq, scale;
	
	// Grab some information on the supplied atom
	int indexI = i->index(), typeI = i->atomTypeIndex();
	Molecule* moleculeI = i->molecule();
	const Vec3<double> rI = i->r();

	// Communication group determines loop/summation style
	start = Comm.interleavedLoopStart(group);
	stride = Comm.interleavedLoopStride(group);

	// Loop over cell atoms
	if (applyMim)
	{
		for (j = start; j < cell->atoms().nItems(); j += stride)
		{
			jj = cellAtoms[j];

			// Check exclusion of I == J
			if (i == jj) continue;

			// Calculate rSquared distance betwenn atoms, and check it against the stored cutoff distance
			rSq = box_->minimumDistanceSquared(rI, jj->r());
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.energy(typeI, jj->atomTypeIndex(), rSq);
			else
			{
				scale = moleculeI->species()->scaling(i->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(typeI, jj->atomTypeIndex(), rSq) * scale;
			}
		}
	}
	else
	{
		for (j = start; j < cell->atoms().nItems(); j += stride)
		{
			jj = cellAtoms[j];

			// Check exclusion of I == J
			if (i == jj) continue;

			// Calculate rSquared distance betwenn atoms, and check it against the stored cutoff distance
			rSq = (rI - jj->r()).magnitudeSq();
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.energy(typeI, jj->atomTypeIndex(), rSq);
			else
			{
				scale = moleculeI->species()->scaling(i->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(typeI, jj->atomTypeIndex(), rSq) * scale;
			}
		}
	}
	
	// Sum over processes if necessary
	if (group == DUQComm::Group) Comm.allSum(&totalEnergy, 1, DUQComm::Group);
	else if (group == DUQComm::World) Comm.allSum(&totalEnergy, 1);

	return totalEnergy;
}

/*!
 * \brief Return PairPotential energy between atom and list of neighbouring cells
 * \details Calculate the energy between the supplied atom and list of neighbouring cells. Note that it is assumed that the supplied atom
 * is in a cell which does *not* appear in the list.
 */
double EnergyKernel::energy(const Atom* i, OrderedList<Atom>& neighbours, bool applyMim, DUQComm::CommGroup group)
{
#ifdef CHECKS
	if (cell == NULL)
	{
		msg.error("NULL_POINTER - NULL cell pointer passed to EnergyKernel::energy(Atom,Cell,bool,bool).\n");
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
	int indexI = i->index(), typeI = i->atomTypeIndex();
	Molecule* moleculeI = i->molecule();
	Vec3<double> rI = i->r();

	// Communication group determines loop/summation style
	start = Comm.interleavedLoopStart(group);
	stride = Comm.interleavedLoopStride(group);

	// Loop over cell atoms
	if (applyMim)
	{
		// Loop over atom neighbours
		for (j=start; j<nNeighbourAtoms; j += stride)
		{
			jj = neighbourAtoms[j];

			// Calculate rSquared distance betwenn atoms, and check it against the stored cutoff distance
			rSq = box_->minimumDistanceSquared(rI, jj->r());
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.energy(typeI, jj->atomTypeIndex(), rSq);
			else
			{
				scale = moleculeI->species()->scaling(i->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(typeI, jj->atomTypeIndex(), rSq) * scale;
			}
		}
	}
	else
	{
		// Loop over atom neighbours
		for (j = start; j < nNeighbourAtoms; j += stride)
		{
			jj = neighbourAtoms[j];

			// Calculate rSquared distance betwenn atoms, and check it against the stored cutoff distance
			rSq = (rI - jj->r()).magnitudeSq();
			if (rSq > cutoffDistanceSquared_) continue;

			// Check for atoms in the same species
			if (moleculeI != jj->molecule()) totalEnergy += potentialMap_.energy(typeI, jj->atomTypeIndex(), rSq);
			else
			{
				scale = moleculeI->species()->scaling(i->moleculeAtomIndex(), jj->moleculeAtomIndex());
				if (scale > 1.0e-3) totalEnergy += potentialMap_.energy(typeI, jj->atomTypeIndex(), rSq) * scale;
			}
		}
	}

	// Sum over processes if necessary
	if (group == DUQComm::Group) Comm.allSum(&totalEnergy, 1, DUQComm::Group);
	else if (group == DUQComm::World) Comm.allSum(&totalEnergy, 1);

	return totalEnergy;
}

/*!
 * \brief Return PairPotential energy between Grain and Cell contents
 * \details Calculate the energy between the Grain specified and the entire (Grain) contents of the given Cell,
 * applying minimum image calculations if necessary.
 */
double EnergyKernel::energy(const Grain* grain, Cell* cell, bool applyMim, bool excludeIgeJ, DUQComm::CommGroup group)
{
#ifdef CHECKS
	if (grain == NULL)
	{
		msg.error("NULL_POINTER - NULL Grain pointer passed to EnergyKernel::energy(Grain,Cell,bool).\n");
		return 0.0;
	}
	if (cell == NULL)
	{
		msg.error("NULL_POINTER - NULL Cell pointer passed to EnergyKernel::energy(Grain,Cell,bool).\n");
		return 0.0;
	}
#endif
	double totalEnergy = 0.0;
	Atom** cellAtoms = cell->atoms().objects();
	int i, j, start, stride, typeI, indexI;
	Vec3<double> rI;
	Molecule* moleculeI;
	Atom* atomI;
	double scale;

	// Communication group determines loop/summation style
	start = Comm.interleavedLoopStart(group);
	stride = Comm.interleavedLoopStride(group);
	
	// Interleaved loop over grains in target cell
	if (applyMim)
	{
		// Loop over grain atoms
		for (i = 0; i<grain->nAtoms(); ++i)
		{
			atomI = grain->atom(i);
			indexI = atomI->index();
			rI = atomI->r();
			typeI = atomI->atomTypeIndex();
			moleculeI = atomI->molecule();

			// Loop over cell atoms
			for (j = start; j < cell->atoms().nItems(); j += stride)
			{
				Atom* atomJ = cellAtoms[j];

				// Check exclusion of I == J
				if (indexI == atomJ->index()) continue;

				// Check for atoms in the same species
				if (moleculeI == atomJ->molecule())
				{
					scale = moleculeI->species()->scaling(atomI->moleculeAtomIndex(), atomJ->moleculeAtomIndex());
					if (scale < 1.0e-3) continue;
					totalEnergy += energyWithMim(typeI, rI, atomJ) * scale;
				}
				else totalEnergy += energyWithMim(typeI, rI, atomJ);
			}
		}
	}
	else
	{
		// Loop over grain atoms
		for (i = 0; i<grain->nAtoms(); ++i)
		{
			atomI = grain->atom(i);
			indexI = atomI->index();
			rI = atomI->r();
			typeI = atomI->atomTypeIndex();
			moleculeI = atomI->molecule();

			// Loop over cell atoms
			for (j = start; j < cell->atoms().nItems(); j += stride)
			{
				Atom* atomJ = cellAtoms[j];

				// Check exclusion of I == J
				if (indexI == atomJ->index()) continue;

				// Check for atoms in the same species
				if (moleculeI == atomJ->molecule())
				{
					scale = moleculeI->species()->scaling(atomI->moleculeAtomIndex(), atomJ->moleculeAtomIndex());
					if (scale < 1.0e-3) continue;
					totalEnergy += energyWithoutMim(typeI, rI, atomJ) * scale;
				}
				else totalEnergy += energyWithoutMim(typeI, rI, atomJ);
			}
		}
	}

	// Sum over processes if necessary
	if (group == DUQComm::Group) Comm.allSum(&totalEnergy, 1, DUQComm::Group);
	else if (group == DUQComm::World) Comm.allSum(&totalEnergy, 1);
	
	return totalEnergy;
}

/*!
 * \brief Return intermolecular energy between Grain and list of Cells
 */
double EnergyKernel::energy(const Grain* grain, int nNeighbours, Cell** neighbours, bool applyMim, bool excludeIgeJ, DUQComm::CommGroup group)
{
#ifdef CHECKS
	if (grain == NULL)
	{
		msg.error("NULL_POINTER - NULL Grain pointer passed to EnergyKernel::energy(Grain,RefList<Cell>,bool,bool,ParallelStyle).\n");
		return 0.0;
	}
#endif
	double totalEnergy = 0.0;

	// Communication group determines loop/summation style
	if (group == DUQComm::Solo)
	{
		// Straight loop over Cell neighbours
		for (int n=0; n<nNeighbours; ++n) totalEnergy += energy(grain, neighbours[n], applyMim, excludeIgeJ);
	}
	else if (group == DUQComm::Group)
	{
		// Striped loop over Cell neighbours (Process Groups)
		for (int n=Comm.localGroupRank(); n<nNeighbours; n += Comm.localGroupSize()) totalEnergy += energy(grain, neighbours[n], applyMim, excludeIgeJ);
		// Reduce energy to all processes within group
		Comm.allSum(&totalEnergy, 1, DUQComm::Group);
	}
	else
	{
		// Striped loop over Cell neighbours (individual processes)
		for (int n=Comm.rank(); n<nNeighbours; n += Comm.nProcesses()) totalEnergy += energy(grain, neighbours[n], applyMim, excludeIgeJ);
		// Reduce energy to all processes within group
		Comm.allSum(&totalEnergy, 1);
	}
// 	msg.print("totalEnergy = %f\n", totalEnergy);
	return totalEnergy;
}

/*
// Intramolecular Terms
*/

/*!
 * \brief Return Bond energy
 */
double EnergyKernel::energy(const Molecule* mol, const SpeciesBond* b)
{
	// Determine whether we need to apply minimum image to the distance calculation
	Atom* i, *j;
	i = mol->atom(b->indexI());
	j = mol->atom(b->indexJ());
	if (configuration_.useMim(i->cell(), j->cell())) return b->energy(box_->minimumDistance(i, j));
	else return b->energy((i->r() - j->r()).magnitude());
}

/*!
 * \brief Return Angle energy
 */
double EnergyKernel::energy(const Molecule* mol, const SpeciesAngle* a)
{
	Vec3<double> vecji, vecjk;

	// Determine whether we need to apply minimum image between 'j-i' and 'j-k'
	Atom* i, *j, *k;
	i = mol->atom(a->indexI());
	j = mol->atom(a->indexJ());
	k = mol->atom(a->indexK());
	if (configuration_.useMim(j->cell(), i->cell())) vecji = box_->minimumVector(j, i);
	else vecji = i->r() - j->r();
	if (configuration_.useMim(j->cell(), k->cell())) vecjk = box_->minimumVector(j, k);
	else vecjk = k->r() - j->r();
	
	// Normalise vectors
	vecji.normalise();
	vecjk.normalise();

	// Determine Angle energy
	return a->energy(Box::angle(vecji, vecjk));
}

/*!
 * \brief Return full intramolecular energy
 * \details Calculates the full intramolecular energy of the Grain, including internal and connection terms.
 */
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
// Molecule Terms
*/

/*!
 * \brief Return total Molecule energy
 * \details Calculates the total interaction energy of a Molecule with the rest of the system, and includes PairPotential and corrected intramolecular terms.
 * The argument 'halfPP' controls whether the total energy returned is suitable for summation into a total system energy (halfPP = true) or whether a single
 * Molecule energy is required (halfPP = false) and controls whether intermolecular Grain corrections are performed, as well as halving the total Grain energy.
 */
double EnergyKernel::energy(Molecule* mol, DUQComm::CommGroup group, bool halfPP, double ppFactorIntra, double termFactor)
{
	double totalEnergy = 0.0, grainEnergy = 0.0, intraEnergy = 0.0, interMolGrainCorrect = 0.0;
	
	// Accumulate total Grain energy with Cells.
	// This will double-count all Grain-Grain interactions within the Molecule, so we accumulate interMolGrainCorrect
	// to correct for this as we go along. Note that this part of the Grain energy is *not* multiplied by ppFactorGrain.
	Grain* grain;
	for (int n=0; n<mol->nGrains(); ++n)
	{
		grain = mol->grain(n);
		grainEnergy += energy(grain, grain->cell(), false, false, group);
// 		grainEnergy += energy(grain, grain->cell()->neighbours(), cutoffSq, false, group);
		printf("EnergyKernel::energy(Molecule*) is horribly broken......\n");
		
		if (!halfPP) for (int m=n+1; m<mol->nGrains(); ++m) interMolGrainCorrect -= energy(grain, mol->grain(m), false, false);
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
