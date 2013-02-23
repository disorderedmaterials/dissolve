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
#include "classes/potentialmap.h"
#include "classes/angle.h"
#include "classes/molecule.h"
#include "base/comms.h"

/*!
 * \brief Constructor
 * \details Constructor for EnergyKernel.
 */
EnergyKernel::EnergyKernel(const Box* box, const PotentialMap& potentialMap) : box_(box), potentialMap_(potentialMap)
{
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
 * \brief Return PairPotential energy between Atoms provided (no minimum image calculation)
 */
double EnergyKernel::energyWithoutMim(const Atom* i, const Atom* j)
{
// 	printf("EnergyKernel - energy %i-%i is %f at %f\n", i->index(), j->index(), potentialMap_.energy(i, j, (i->r() - j->r()).magnitudeSq()), (i->r() - j->r()).magnitudeSq());
	return potentialMap_.energy(i, j, (i->r() - j->r()).magnitudeSq());
}

/*!
 * \brief Return PairPotential energy between Atom and Grain provided (no minimum image calculation)
 */
double EnergyKernel::energyWithoutMim(const Atom* i, const Grain* grain, bool excludeIgtJ)
{
	double totalEnergy = 0.0;
	int m, nAtomsJ = grain->nAtoms();
	if (excludeIgtJ)
	{
		for (m=0; m<nAtomsJ; ++m) if (i->index() <= grain->atom(m)->index()) totalEnergy += energyWithoutMim(i, grain->atom(m));
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
				scale = grainI->parent()->scaling(i->index(), j->index());
				if (scale < 1.0e-3) continue;
				totalEnergy += energyWithoutMim(i, j) * scale;
			}
		}
	}
	return totalEnergy;
}

/*!
 * \brief Return PairPotential energy between Atoms provided (minimum image calculation)
 */
double EnergyKernel::energyWithMim(const Atom* i, const Atom* j)
{
// 	msg.print("EnergyKernel - energy %i-%i is %f at %f\n", i->index(), j->index(), potentialMap_.energy(i, j, box_->minimumDistanceSquared(j, i)), box_->minimumDistanceSquared(j, i));
	return potentialMap_.energy(i, j, box_->minimumDistanceSquared(j, i));
}

/*!
 * \brief Return PairPotential energy between Atom and Grain provided (minimum image calculation)
 */
double EnergyKernel::energyWithMim(const Atom* i, const Grain* grain, bool excludeIgtJ)
{
	double totalEnergy = 0.0;
	int m, nAtomsJ = grain->nAtoms();

	if (excludeIgtJ)
	{
		for (m=0; m<nAtomsJ; ++m) if (i->index() <= grain->atom(m)->index()) totalEnergy += energyWithMim(i, grain->atom(m));
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
				scale = grainI->parent()->scaling(i->index(), j->index());
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
 * \brief Return PairPotential energy between Atoms provided
 */
double EnergyKernel::energy(const Atom* i, const Atom* j, bool applyMim, bool excludeIgtJ)
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
	if (excludeIgtJ && (i->index() > j->index())) return 0.0;

	if (applyMim) return energyWithMim(i, j);
	else return energyWithoutMim(i, j);
}

/*!
 * \brief Return PairPotential energy between Atom and Grain provided
 */
double EnergyKernel::energy(const Atom* i, const Grain* grain, bool applyMim, bool excludeIgtJ)
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
	if (applyMim) return energyWithMim(i, grain, excludeIgtJ);
	else return energyWithoutMim(i, grain, excludeIgtJ);
}

/*!
 * \brief Return PairPotential energy between Grains provided
 */
double EnergyKernel::energy(const Grain* grainI, const Grain* grainJ, double cutoffSq, bool applyMim, bool excludeIgtJ)
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
	double rSq;
	if (applyMim) rSq = box_->minimumDistanceSquared(grainI->centre(), grainJ->centre());
	else rSq = (grainI->centre() - grainJ->centre()).magnitudeSq();
// 	msg.print("GG %3i %3i MIM=%i EXC=%i rSq=%f (%f,%f,%f) (%f,%f,%f)\n", grainI->index(), grainJ->index(), applyMim, excludeIgtJ, rSq, grainI->centre().x, grainI->centre().y, grainI->centre().z, grainJ->centre().x, grainJ->centre().y, grainJ->centre().z);
	if (rSq > cutoffSq) return 0.0;
	
	// Check exclusion of I > J
	if (excludeIgtJ && (grainI->index() > grainJ->index())) return 0.0;

	if (applyMim) return energyWithMim(grainI, grainJ);
	else return energyWithoutMim(grainI, grainJ);
}

/*!
 * \brief Return PairPotential energy between Atom and Cell contents
 * \details Calculate the energy between the Atom specified and the entire (Grain) contents of the given Cell,
 * applying minimum image calculations if necessary.
 */
double EnergyKernel::energy(const Atom* i, Cell* cell, bool applyMim, bool excludeIgtJ)
{
#ifdef CHECKS
	if (i == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer passed to EnergyKernel::energy(Atom,Cell,bool,bool).\n");
		return 0.0;
	}
	if (cell == NULL)
	{
		msg.error("NULL_POINTER - NULL Cell pointer passed to EnergyKernel::energy(Atom,Cell,bool,bool).\n");
		return 0.0;
	}
#endif
	double totalEnergy = 0.0;

	// Loop over Cell Grains
	for (RefListItem<Grain,int>* grainRef = cell->grains(); grainRef != NULL; grainRef = grainRef->next)
	{
		totalEnergy += energy(i, grainRef->item, applyMim, excludeIgtJ);
	}
	
	return totalEnergy;
}

/*!
 * \brief Return PairPotential energy between Grain and Cell contents
 * \details Calculate the energy between the Grain specified and the entire (Grain) contents of the given Cell,
 * applying minimum image calculations if necessary.
 */
double EnergyKernel::energy(const Grain* grain, Cell* cell, double cutoffSq, bool applyMim, bool excludeIgtJ, DUQComm::CommGroup group)
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

	// Communication group determines loop/summation style
	if (group == DUQComm::Solo)
	{
		// Straight loop over Cell Grains
		for (RefListItem<Grain,int>* grainRef = cell->grains(); grainRef != NULL; grainRef = grainRef->next)
		{
			totalEnergy += energy(grain, grainRef->item, cutoffSq, applyMim, excludeIgtJ);
		}
	}
	else if (group == DUQComm::Group)
	{
		// Striped loop over Cell neighbours (Process Groups)
		Grain* otherGrain;
		for (int n=Comm.localGroupRank(); n<cell->nGrains(); n += Comm.localGroupSize())
		{
			otherGrain = cell->grain(n);
			if (grain == otherGrain) continue;
			totalEnergy += energy(grain, otherGrain, cutoffSq, applyMim, excludeIgtJ);
		}
		// Reduce energy to all processes within group
		Comm.allSum(&totalEnergy, 1, DUQComm::Group);
	}
	else
	{
		// Striped loop over Cell neighbours (all processes in World)
		Grain* otherGrain;
		for (int n=Comm.rank(); n<cell->nGrains(); n += Comm.nProcesses())
		{
			otherGrain = cell->grain(n);
			if (grain == otherGrain) continue;
			totalEnergy += energy(grain, otherGrain, cutoffSq, applyMim, excludeIgtJ);
		}
		// Reduce energy to all processes
		Comm.allSum(&totalEnergy, 1);
	}

	return totalEnergy;
}

/*!
 * \brief Return PairPotential energy between Atom and list of Cells
 */
double EnergyKernel::energy(const Atom* i, RefList<Cell,bool>& neighbours, bool excludeIgtJ, DUQComm::CommGroup group)
{
#ifdef CHECKS
	if (i == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer passed to EnergyKernel::energy(Atom,RefList<Cell>,bool,ParallelStyle).\n");
		return 0.0;
	}
#endif
	double totalEnergy = 0.0;

	// Communication group determines loop/summation style
	if (group == DUQComm::Solo)
	{
		// Straight loop over Cell neighbours
		for (RefListItem<Cell,bool>* cellRef = neighbours.first(); cellRef != NULL; cellRef = cellRef->next)
		{
			totalEnergy += energy(i, cellRef->item, cellRef->data, excludeIgtJ);
		}
	}
	else if (group == DUQComm::Group)
	{
		// Striped loop over Cell neighbours (Process Groups)
		Cell* cell;
		bool applyMim;
		for (int n=Comm.localGroupRank(); n<neighbours.nItems(); n += Comm.localGroupSize())
		{
			cell = neighbours[n]->item;
			applyMim = neighbours[n]->data;
			totalEnergy += energy(i, cell, applyMim, excludeIgtJ);
		}
		// Reduce energy to all processes within group
		Comm.allSum(&totalEnergy, 1, DUQComm::Group);
	}
	else
	{
		// Striped loop over Cell neighbours (all processes in World)
		Cell* cell;
		bool applyMim;
		for (int n=Comm.rank(); n<neighbours.nItems(); n += Comm.nProcesses())
		{
			cell = neighbours[n]->item;
			applyMim = neighbours[n]->data;
			totalEnergy += energy(i, cell, applyMim, excludeIgtJ);
		}
		// Reduce energy to all processes
		Comm.allSum(&totalEnergy, 1);
	}

	return totalEnergy;
}

/*!
 * \brief Return intermolecular energy between Grain and list of Cells
 */
double EnergyKernel::energy(const Grain* grain, RefList<Cell,bool>& neighbours, double cutoffSq, bool excludeIgtJ, DUQComm::CommGroup group)
{
#ifdef CHECKS
	if (grain == NULL)
	{
		msg.error("NULL_POINTER - NULL Grain pointer passed to EnergyKernel::energy(Grain,RefList<Cell>,bool,ParallelStyle).\n");
		return 0.0;
	}
#endif
	double totalEnergy = 0.0;

	// Communication group determines loop/summation style
	if (group == DUQComm::Solo)
	{
		// Straight loop over Cell neighbours
		for (RefListItem<Cell,bool>* cellRef = neighbours.first(); cellRef != NULL; cellRef = cellRef->next)
		{
			totalEnergy += energy(grain, cellRef->item, cellRef->data, excludeIgtJ);
		}
	}
	else if (group == DUQComm::Group)
	{
		// Striped loop over Cell neighbours (Process Groups)
		Cell* cell;
		bool applyMim;
		for (int n=Comm.localGroupRank(); n<neighbours.nItems(); n += Comm.localGroupSize())
		{
			cell = neighbours[n]->item;
			applyMim = neighbours[n]->data;
			totalEnergy += energy(grain, cell, cutoffSq, applyMim, excludeIgtJ);
		}
		// Reduce energy to all processes within group
		Comm.allSum(&totalEnergy, 1, DUQComm::Group);
	}
	else
	{
		// Striped loop over Cell neighbours (individual processes)
		Cell* cell;
		bool applyMim;
		for (int n=Comm.rank(); n<neighbours.nItems(); n += Comm.nProcesses())
		{
			cell = neighbours[n]->item;
			applyMim = neighbours[n]->data;
			totalEnergy += energy(grain, cell, cutoffSq, applyMim, excludeIgtJ);
		}
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
double EnergyKernel::energy(const Bond* b)
{
	// Determine whether we need to apply minimum image to the distance calculation
	if (box_->useMim(b->i()->grain()->cell(), b->j()->grain()->cell())) return b->energy(box_->minimumDistance(b->i(), b->j()));
	else return b->energy((b->i()->r() - b->j()->r()).magnitude());
}

/*!
 * \brief Return Angle energy
 */
double EnergyKernel::energy(const Angle* a)
{
	Vec3<double> vecji, vecjk;

	// Determine whether we need to apply minimum image between 'j-i' and 'j-k'
	if (box_->useMim(a->j()->grain()->cell(), a->i()->grain()->cell())) vecji = box_->minimumVector(a->j(), a->i());
	else vecji = a->i()->r() - a->j()->r();
	if (box_->useMim(a->j()->grain()->cell(), a->k()->grain()->cell())) vecjk = box_->minimumVector(a->j(), a->k());
	else vecjk = a->k()->r() - a->j()->r();
	
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

	// Add inter-Grain Bond terms
	for (RefListItem<Bond,int>* refBond = grain->bondConnections(); refBond != NULL; refBond = refBond->next) intraEnergy += energy(refBond->item) * termFactor;

	// Add inter-Grain Angle terms
	for (RefListItem<Angle,int>* refAngle = grain->angleConnections(); refAngle != NULL; refAngle = refAngle->next) intraEnergy += energy(refAngle->item) * termFactor;

	// Add intra-Grain Bond terms
	for (RefListItem<Bond,int>* refBond = grain->internalBonds(); refBond != NULL; refBond = refBond->next) intraEnergy += energy(refBond->item) * termFactor;

	// Add intra-Grain Angle terms
	for (RefListItem<Angle,int>* refAngle = grain->internalAngles(); refAngle != NULL; refAngle = refAngle->next) intraEnergy += energy(refAngle->item) * termFactor;

	return intraEnergy;

}

/*
// Molecule Terms
*/

/*!
 * \brief Return total Molecule energy
 * \details Calculates the total interaction energy of a Molecule with the rest of the system, and includes PairPotential and corrected intramolecular terms.
 * The argument 'halfPP' controls whether the total energy returned is suitable for summation into a total system energy (halfPP = TRUE) or whether a single
 * Molecule energy is required (halfPP = FALSE) and controls whether intermolecular Grain corrections are performed, as well as halving the total Grain energy.
 */
double EnergyKernel::energy(Molecule* mol, double cutoffSq, DUQComm::CommGroup group, bool halfPP, double ppFactorIntra, double termFactor)
{
	double totalEnergy = 0.0, grainEnergy = 0.0, intraEnergy = 0.0, interMolGrainCorrect = 0.0;
	
	// Accumulate total Grain energy with Cells.
	// This will double-count all Grain-Grain interactions within the Molecule, so we accumulate interMolGrainCorrect
	// to correct for this as we go along. Note that this part of the Grain energy is *not* multiplied by ppFactorGrain.
	Grain* grain;
	for (int n=0; n<mol->nGrains(); ++n)
	{
		grain = mol->grain(n);
		grainEnergy += energy(grain, grain->cell(), cutoffSq, FALSE, FALSE, group);
		grainEnergy += energy(grain, grain->cell()->neighbours(), cutoffSq, FALSE, group);
		
		if (!halfPP) for (int m=n+1; m<mol->nGrains(); ++m) interMolGrainCorrect -= energy(grain, mol->grain(m), FALSE, FALSE);
	}
	totalEnergy += (halfPP ? 0.5 : 1.0)*grainEnergy + interMolGrainCorrect;

	// Bond energy
	RefList<Bond,int>& bonds = mol->bonds();
	for (int n=0; n<bonds.nItems(); ++n) intraEnergy += energy(bonds[n]->item) * termFactor;

	// Angle energy
	RefList<Angle,int>& angles = mol->angles();
	for (int n=0; n<angles.nItems(); ++n) intraEnergy += energy(angles[n]->item) * termFactor;
// 	printf("grainEnergy = %f, corrIntraEnergy = %f, interMolGrainCorrect = %f\n", (halfPP ? 0.5 : 1.0)*grainEnergy, intraEnergy, interMolGrainCorrect);
	
	totalEnergy += termFactor*intraEnergy;

	return totalEnergy;
}
