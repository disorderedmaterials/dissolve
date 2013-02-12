/*
	*** Configuration
	*** src/lib/classes/configuration.cpp
	Copyright T. Youngs 2012-2013

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
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/grain.h"
#include "classes/molecule.h"
#include "classes/species.h"
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
	atoms_ = NULL;
	grains_ = NULL;
	nAtoms_ = 0;
	nGrains_ = 0;
	box_ = NULL;
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
	clear();

	// Create arrays
	nAtoms_ = source.nAtoms_;
	atoms_ = new Atom[nAtoms_];
	nGrains_ = source.nGrains_;
	grains_ = new Grain[nGrains_];

	// Set Grain indices
	int n;
	for (n=0; n<nGrains_; ++n) grains_[n].setIndex(n);

	// Copy Atom data
	for (int n=0; n<nAtoms_; ++n) atoms_[n].copyProperties(&source.atoms_[n]);

	// Copy basic Molecule information, and assign Atom/Grains	
	int atomCount = 0, grainCount = 0, startAtom = 0;
	Bond* b, *newBond;
	Angle* a, *newAngle;
	Molecule* newMol;
	for (Molecule* mol = source.molecules_.first(); mol != NULL; mol = mol->next)
	{
		// Create Molecule copy
		newMol = molecules_.add();
		newMol->initialise(mol->species(), molecules_.nItems()-1);

		// Assign Atoms
		for (n = 0; n<newMol->nAtoms(); ++n)
		{
			if (atomCount >= nAtoms_)
			{
				msg.error("Mismatch between size of Atom array in Configuration, and number of Atoms needed by Molecules (in assignment operator).\n");
				return;
			}
			newMol->setAtom(n, &atoms_[atomCount++]);
		}
		
		// Assign Grains
		for (n = 0; n<newMol->nGrains(); ++n)
		{
			if (grainCount >= nGrains_)
			{
				msg.error("Mismatch between size of Grain array in Configuration, and number of Grains needed by Molecules (in assignment operator).\n");
				return;
			}
			newMol->setGrain(n, &grains_[grainCount]);
			grains_[grainCount].setParent(newMol);
			++grainCount;
		}
		
		// Create scaling matrix
		newMol->createScalingMatrix();
		
		// Instantiate (without setting Atom properties)
		newMol->instantiate(FALSE);

		// Increase startAtom
		startAtom += newMol->nAtoms();
	}

	// Copy Bond list
	for (Bond* b = source.bonds_.first(); b != NULL; b = b->next)
	{
		newMol = molecules_[b->molecule()->index()];
		newBond = bonds_.add();
		newBond->setMolecule(newMol);
		newBond->setAtoms(&atoms_[b->indexI()], &atoms_[b->indexJ()]);
		newBond->setEquilibrium(b->equilibrium());
		newBond->setForceConstant(b->forceConstant());
	
		// Attached Atom lists...
		newBond->createAttachedAtomArray(0, b->nAttached(0));
		for (n=0; n<b->nAttached(0); ++n) newBond->attached(0)[n] = &atoms_[b->attached(0)[n]->index()];
		newBond->createAttachedAtomArray(1, b->nAttached(1));
		for (n=0; n<b->nAttached(1); ++n) newBond->attached(1)[n] = &atoms_[b->attached(1)[n]->index()];

		// Add to Molecule
		newMol->addBond(newBond);
	}

	// Angles - make copies of those in the Molecule's parent Species
	for (a = source.angles_.first(); a != NULL; a = a->next)
	{
		newMol = molecules_[a->molecule()->index()];
		newAngle = angles_.add();
		newAngle->setMolecule(newMol);
		newAngle->setAtoms(&atoms_[a->indexI()], &atoms_[a->indexJ()], &atoms_[a->indexK()]);
		newAngle->setEquilibrium(a->equilibrium());
		newAngle->setForceConstant(a->forceConstant());
		
		// Attached Atom lists...
		newAngle->createAttachedAtomArray(0, a->nAttached(0));
		for (n=0; n<a->nAttached(0); ++n) newAngle->attached(0)[n] = &atoms_[a->attached(0)[n]->index()];
		newAngle->createAttachedAtomArray(1, a->nAttached(1));
		for (n=0; n<a->nAttached(1); ++n) newAngle->attached(1)[n] = &atoms_[a->attached(1)[n]->index()];

		// Add to Molecule
		newMol->addAngle(newAngle);
	}

	// Create inter-Grain terms lists
	identifyInterGrainTerms();

	// Copy Box
	if (source.box_ != NULL)
	{
		// Create a new Box of the correct class, sending dummy parameters to the constructor
		if (source.box_->type() == Box::NonPeriodicBox) box_ = new NonPeriodicBox(1.0);
		else if (source.box_->type() == Box::CubicBox) box_ = new CubicBox(1.0);
		else if (source.box_->type() == Box::OrthorhombicBox) box_ = new OrthorhombicBox(1.0, Vec3<double>(1.0,1.0,1.0));
		else if (source.box_->type() == Box::MonoclinicBox) box_ = new MonoclinicBox(1.0, Vec3<double>(1.0,1.0,1.0), 90.0);
		else if (source.box_->type() == Box::TriclinicBox) box_ = new TriclinicBox(1.0, Vec3<double>(1.0,1.0,1.0), Vec3<double>(90,90,90));
		
		// Now copy proper Box information
		(*box_) = (*source.box_);
	}

	changeCount_ = source.changeCount_;

	return;
}

/*!
 * \brief Clear data
 */
void Configuration::clear()
{
	molecules_.clear();
	if (grains_ != NULL) delete[] grains_;
	grains_ = NULL;
	if (atoms_ != NULL) delete[] atoms_;
	atoms_ = NULL;
	nGrains_ = 0;
	nAtoms_ = 0;
	bonds_.clear();
	angles_.clear();
	if (box_ == NULL) delete box_;
	box_ = NULL;
}

/*
// Contents
*/

/*!
 * \brief Return Box
 */
const Box* Configuration::box()
{
	return box_;
}

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
 * \brief Return number of Grains
 */
int Configuration::nGrains() const
{
	return nGrains_;
}

/*!
 * \brief Return Grain array
 */
Grain* Configuration::grains()
{
	return grains_;
}

/*!
 *  \brief Return nth Grain
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
 * \brief Return number of Atoms
 */
int Configuration::nAtoms() const
{
	return nAtoms_;
}

/*!
 * \brief Return Atom array
 */
Atom* Configuration::atoms()
{
	return atoms_;
}

/*!
 * \brief Return nth Atom
 */
Atom& Configuration::atom(int n)
{
#ifdef CHECKS
	static Atom dummy;
	if ((n < 0) || (n >= nAtoms_))
	{
		msg.print("OUT_OF_RANGE - Atom index %i passed to Configuration::atom() is out of range (nAtoms_ = %i).\n", n, nAtoms_);
		return dummy;
	}
#endif
	return atoms_[n];
}

/*!
 * \brief Setup periodic Box
 */
bool Configuration::setupBox(double ppRange, Vec3<double> relativeLengths, Vec3<double> angles, double atomicDensity, bool nonPeriodic)
{
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
	else if (rightBeta && rightGamma) box_ = new MonoclinicBox(volume, relativeLengths, angles.x);	// Monoclinic
	else
	{
		// Triclinic
		box_ = new TriclinicBox(volume, relativeLengths, angles);
	}
	
	msg.print("--> %s Box created for system.\n", Box::boxType(box_->type()));
	Matrix3 axes = box_->axes();
	msg.print("--> Axes Matrix : A = %10.4e %10.4e %10.4e, length = %10.4e Angstroms\n", axes[0], axes[1], axes[2], box_->axisLength(0));
	msg.print("-->               B = %10.4e %10.4e %10.4e, length = %10.4e Angstroms\n", axes[3], axes[4], axes[5], box_->axisLength(1));
	msg.print("-->               C = %10.4e %10.4e %10.4e, length = %10.4e Angstroms\n", axes[6], axes[7], axes[8], box_->axisLength(2));
	
	// Check cell lengths against pair potential range
	if (ppRange > box_->inscribedSphereRadius())
	{
		msg.error("PairPotential range (%f) is longer than the shortest non-minimum image distance (%f).\n", ppRange, box_->inscribedSphereRadius());
		return FALSE;
	}

	// Create Cells
	return box_->generateCells(7.0, ppRange);
}

/*!
 * \brief Instantiate Configuration
 * \details Once Molecules have been added to the Configuration, the master arrays of Atoms and Grains are created by calling
 * instantiate(). Following their creation, instantiate() then assigns the new Atoms/Grains to the proper Molecules, and creates
 * a random Configuration from which to start a simulation. The Master process performs all coordinate generation - these are
 * subsequently sent to all processes.
 */
bool Configuration::instantiate()
{
	if (molecules_.nItems() == 0)
	{
		msg.error("No Molecules have been added to the Configuration - nothing to do.\n");
		return FALSE;
	}
	if (nAtoms_== 0)
	{
		msg.error("Molecules have been added to the Configuration, but no Atoms were counted.\n");
		return FALSE;
	}
	if (nGrains_ == 0)
	{
		msg.error("Molecules have been added to the Configuration, but no Grains were counted.\n");
		return FALSE;
	}
	
	msg.print("--> Creating arrays for %i Atoms and %i Grains...\n", nAtoms_, nGrains_);
	try
	{
		atoms_ = new Atom[nAtoms_];
		grains_ = new Grain[nGrains_];
	}
	catch (bad_alloc& alloc)
	{
		msg.error("Failed to allocate sufficient memory. Exception was : %s\n", alloc.what());
		return FALSE;
	}

	// Set indices
	int n;
	for (n=0; n<nAtoms_; ++n) atoms_[n].setIndex(n);
	for (n=0; n<nGrains_; ++n) grains_[n].setIndex(n);

	// Assign Atoms and Grains to Molecules
	int atomCount = 0, grainCount = 0, startAtom = 0;
	Bond* b, *newBond;
	Angle* a, *newAngle;
	for (Molecule* mol = molecules_.first(); mol != NULL; mol = mol->next)
	{
		// Atoms
		for (n = 0; n<mol->nAtoms(); ++n)
		{
			if (atomCount >= nAtoms_)
			{
				msg.error("Mismatch between size of Atom array in Configuration, and number of Atoms needed by Molecules.\n");
				return FALSE;
			}
			mol->setAtom(n, &atoms_[atomCount++]);
		}
		
		// Grains
		for (n = 0; n<mol->nGrains(); ++n)
		{
			if (grainCount >= nGrains_)
			{
				msg.error("Mismatch between size of Grain array in Configuration, and number of Grains needed by Molecules.\n");
				return FALSE;
			}
			mol->setGrain(n, &grains_[grainCount]);
			grains_[grainCount].setParent(mol);
			++grainCount;
		}

		// Bonds - make copies of those in the Molecule's parent Species
		for (b = mol->species()->bonds(); b != NULL; b = b->next)
		{
			newBond = bonds_.add();
			newBond->setMolecule(mol);
			newBond->setAtoms(&atoms_[startAtom+b->indexI()], &atoms_[startAtom+b->indexJ()]);
			newBond->setEquilibrium(b->equilibrium());
			newBond->setForceConstant(b->forceConstant());
			
			// Attached Atom lists...
			newBond->createAttachedAtomArray(0, b->nAttached(0));
			for (n=0; n<b->nAttached(0); ++n) newBond->attached(0)[n] = &atoms_[startAtom+b->attached(0)[n]->index()];
			newBond->createAttachedAtomArray(1, b->nAttached(1));
			for (n=0; n<b->nAttached(1); ++n) newBond->attached(1)[n] = &atoms_[startAtom+b->attached(1)[n]->index()];

			// Add to Molecule
			mol->addBond(newBond);
		}

		// Angles - make copies of those in the Molecule's parent Species
		for (a = mol->species()->angles(); a != NULL; a = a->next)
		{
			newAngle = angles_.add();
			newAngle->setMolecule(mol);
			newAngle->setAtoms(&atoms_[startAtom+a->indexI()], &atoms_[startAtom+a->indexJ()], &atoms_[startAtom+a->indexK()]);
			newAngle->setEquilibrium(a->equilibrium());
			newAngle->setForceConstant(a->forceConstant());
			
			// Attached Atom lists...
			newAngle->createAttachedAtomArray(0, a->nAttached(0));
			for (n=0; n<a->nAttached(0); ++n) newAngle->attached(0)[n] = &atoms_[startAtom+a->attached(0)[n]->index()];
			newAngle->createAttachedAtomArray(1, a->nAttached(1));
			for (n=0; n<a->nAttached(1); ++n) newAngle->attached(1)[n] = &atoms_[startAtom+a->attached(1)[n]->index()];

			// Add to Molecule
			mol->addAngle(newAngle);
		}

		// Create scaling matrix
		mol->createScalingMatrix();

		// Increase startAtom
		startAtom += mol->nAtoms();
	}

	// Instantiate Molecules (all processes)
	Vec3<double> centre;
	msg.print("--> Creating %i Molecule instances...\n", molecules_.nItems());
	for (Molecule* mol = molecules_.first(); mol != NULL; mol = mol->next)
	{
		centre = box_->randomCoordinate();
		if (!mol->instantiate(TRUE, centre))
		{
			msg.error("Failed to instantiate Molecule.\n");
			return FALSE;
		}
	}

	// Create inter-Grain terms lists
	identifyInterGrainTerms();
	
	return TRUE;
}

/*!
 * \brief Identify inter-Grain terms
 */
void Configuration::identifyInterGrainTerms()
{
	// Bonds
	for (Bond* b = bonds_.first(); b != NULL; b = b->next)
	{
#ifdef CHECKS
		if ((b->i() == NULL) || (b->j() == NULL))
		{
			msg.error("NULL_POINTER - One or both Atom pointers in a Bond are NULL (%p, %p)\n", b->i(), b->j());
			continue;
		}
		if ((b->i()->grain() == NULL) || (b->j()->grain() == NULL))
		{
			msg.error("NULL_POINTER - One or both Grain pointers in a Bond are NULL (%p, %p)\n", b->i()->grain(), b->j()->grain());
			continue;
		}
#endif

		// Is this an entirely INTRA-Grain Bond?
		if (b->i()->grain() == b->j()->grain())
		{
			b->i()->grain()->addInternalBond(b);
			continue;
		}
		
		// Bond is between two Grains, so add it to both and set the interGrain flag
		b->setInterGrain(TRUE);
		b->i()->grain()->addBondConnection(b);
		b->j()->grain()->addBondConnection(b);
	}

	// Angles
	for (Angle* a = angles_.first(); a != NULL; a = a->next)
	{
#ifdef CHECKS
		if ((a->i() == NULL) || (a->j() == NULL) || (a->k() == NULL))
		{
			msg.error("NULL_POINTER - One or more Atom pointers in an Angle are NULL (%p, %p, %p)\n", a->i(), a->j(), a->k());
			continue;
		}
		if ((a->i()->grain() == NULL) || (a->j()->grain() == NULL) || (a->k()->grain() == NULL))
		{
			msg.error("NULL_POINTER - One or more Grain pointers in an Angle are NULL (%p, %p, %p)\n", a->i()->grain(), a->j()->grain(), a->k()->grain());
			continue;
		}
#endif

		// Is this an entirely INTRA-Grain Angle?
		if ((a->i()->grain() == a->j()->grain()) && (a->j()->grain() == a->k()->grain()))
		{
			a->i()->grain()->addInternalAngle(a);
			continue;
		}
		
		// Angle is between at least two Grains, so add it to the relevant parties.
		// Always add to central Atom...
		a->j()->grain()->addAngleConnection(a);
		
		// If Atom 'i' is in a different Grain, add it to that...
		if (a->j()->grain() != a->i()->grain()) a->i()->grain()->addAngleConnection(a);
		
		// If Atom 'k' is in *another* different Grain, add it to that too
		
		// Only add to third Atom if its different again...
		if ((a->k()->grain() != a->i()->grain()) && (a->k()->grain() != a->j()->grain())) a->k()->grain()->addAngleConnection(a);
		
		// Finally, if 'i' and 'k' are in different Grains, set the intraGrain flag to intramolecular correction energies are calculated correctly
		if (a->i()->grain() != a->k()->grain()) a->setInterGrain(TRUE);
	}
}

/*!
 * \brief Generate random configuration
 * \details For the current list of Molecules, give each a random position, orientation, and geometry within the box.
 */
bool Configuration::randomise()
{
	Matrix3 transform;
	msg.print("--> Generating random positions/orientations for %i molecules...\n", molecules_.nItems());
	for (Molecule* mol = molecules_.first(); mol != NULL; mol = mol->next)
	{
		// Apply random rotation to Molecule
		transform.createRotationXY(dUQMath::randomPlusMinusOne()*180.0, dUQMath::randomPlusMinusOne()*180.0);
		mol->applyTransform(transform);
		
		// Randomisation of intramolecular geometry (rotations around bonds)
		mol->randomiseGeometry();
	}

	// Coordinates have changed, so increment change counter
	++changeCount_;

	return TRUE;
}

/*!
 * \brief Load starting coordinates from file
 */
bool Configuration::loadInitialCoordinates(const char* fileName)
{
	// Construct a temporary Species to store the coordinates
	Species species;
	if (!species.load(fileName)) return FALSE;

	// Species now contains stuff - does the number of Atoms match the Configuration?
	if (nAtoms_ != species.nAtoms())
	{
		msg.error("Number of Atoms in initial coordinates file (%i) does not match that in Configuration (%i).\n", species.nAtoms(), nAtoms_);
		return FALSE;
	}
	
	// Copy coordinates from Species to Configuration
	for (int n=0; n<nAtoms_; ++n) atoms_[n].setCoordinates( species.atom(n)->r() );

	// Coordinates have changed, so increment change counter
	++changeCount_;

	return TRUE;
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
// Intramolecular Terms
*/

/*!
 * \brief Return number of Bonds
 */
int Configuration::nBonds() const
{
	return bonds_.nItems();
}

/*!
 * \brief Return first Bond in list
 */
Bond* Configuration::bonds()
{
	return bonds_.first();
}

/*!
 * \brief Return nth Bond
 */
Bond* Configuration::bond(int n)
{
#ifdef CHECKS
	if ((n < 0) || (n >= bonds_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Bond index %i given to Configuration::bond() is out of range (nBonds = %i).\n", n, bonds_.nItems());
		return NULL;
	}
#endif
	return bonds_[n];
}

/*!
 * \brief Return number of Angles
 */
int Configuration::nAngles() const
{
	return angles_.nItems();
}

/*!
 * \brief Return first Angle in list
 */
Angle* Configuration::angles()
{
	return angles_.first();
}

/*!
 * \brief Return nth Angle
 */
Angle* Configuration::angle(int n)
{
#ifdef CHECKS
	if ((n < 0) || (n >= angles_.nItems()))
	{
		msg.print("OUT_OF_RANGE - Angle index %i given to Configuration::angle() is out of range (nAngles = %i).\n", n, angles_.nItems());
		return NULL;
	}
#endif
	return angles_[n];
}

/*
// Box Pass-Thru
*/

/*!
 * \brief Initialise Cells for distribution
 */
void Configuration::initialiseCellDistribution()
{
	box_->initialiseCellDistribution();
}

/*!
 * \brief Return next available Cell for calculation
 */
int Configuration::nextAvailableCell(bool willBeModified, bool allowRepeats)
{
	return box_->nextAvailableCell(willBeModified, allowRepeats);
}

/*!
 * \brief Unlock Cell specified, once calculation is complete
 */
bool Configuration::finishedWithCell(bool willBeModified, int id)
{
	return box_->finishedWithCell(willBeModified, id);
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
			x[n] = atoms_[n].r().x;
			y[n] = atoms_[n].r().y;
			z[n] = atoms_[n].r().z;
		}
	}
	else msg.print("--> Waiting for Master to send Configuration.\n");

	if (!Comm.broadcast(x, nAtoms_)) return FALSE;
	if (!Comm.broadcast(y, nAtoms_)) return FALSE;
	if (!Comm.broadcast(z, nAtoms_)) return FALSE;
	
	// Slaves then store values into Atoms...
	if (Comm.slave()) for (int n=0; n<nAtoms_; ++n) atoms_[n].setCoordinates(x[n], y[n], z[n]);

	delete[] x;
	delete[] y;
	delete[] z;
	
	// Broadcast changeCount_
	if (!Comm.broadcast(&changeCount_, 1)) return FALSE;
#endif
	return TRUE;
}

