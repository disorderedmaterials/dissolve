/*
	*** Molecule Definition
	*** src/lib/classes/molecule.cpp
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

#include "classes/molecule.h"
#include "classes/species.h"
#include "classes/graindefinition.h"
#include "base/messenger.h"

/*!
 * \brief Constructor
 * \details Constructor for Molecule. 
 */
Molecule::Molecule() : ListItem<Molecule>()
{
	nAtoms_ = 0;
	atoms_ = NULL;
	nGrains_ = 0;
	grains_ = NULL;
	scalingMatrix_ = NULL;
	index_ = -1;
}

/*!
 * \brief Destructor
 * \details Destructor for Molecule. 
 */
Molecule::~Molecule()
{
	if (atoms_ != NULL) delete[] atoms_;
	if (grains_ != NULL) delete[] grains_;
	if (scalingMatrix_ != NULL) delete[] scalingMatrix_;
}

/*
// Atoms / Grains
*/

/*!
 * \brief Initialise Molecule arrays suitable for Species provided
 * \details Constructs the necessary Atom and Grain arrays to hold the contents of a copy of the specified Species.
 * No coordinate information is set at this point - this is done by a future call to instantiate().
 */
bool Molecule::initialise(Species* sp, int index)
{
	// Check for NULL pointer
	species_ = sp;
	if (species_ == NULL)
	{
		msg.error("NULL_POINTER - NULL Species pointer passed to Molecule::initialise().\n");
		return FALSE;
	}
	
	// Check for double initialisation
	if (atoms_ != NULL)
	{
		printf("DOUBLE_INIT - Arrays already exist in Molecule::initialise().\n");
		return FALSE;
	}
	
	// Get data from Species and create local arrays
	nAtoms_ = species_->nAtoms();
	atoms_ = new Atom*[nAtoms_];
	nGrains_ = species_->nGrainDefinitions();
	grains_ = new Grain*[nGrains_];
	
	index_ = index;

	return TRUE;
}

/*!
 * \brief Return Species from which Molecule was initialised
 */
Species* Molecule::species()
{
	return species_;
}

/*!
 * \brief Return size of Atom array
 */
int Molecule::nAtoms()
{
	return nAtoms_;
}

/*!
 * \brief Set nth Atom pointer
 */
void Molecule::setAtom(int n, Atom* i)
{
#ifdef CHECKS
	if (i == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer passed to Molecule::setAtom().\n");
		return;
	}
	if ((n < 0) || (n >= nAtoms_))
	{
		msg.print("OUT_OF_RANGE - Atom index %i is out of range in Molecule::setAtom().\n", n);
		return;
	}
#endif
	atoms_[n] = i;
}

/*!
 * \brief Return nth Atom pointer
 */
Atom* Molecule::atom(int n)
{
#ifdef CHECKS
	if ((n < 0) || (n >= nAtoms_))
	{
		msg.print("OUT_OF_RANGE - Atom index %i is out of range in Molecule::atom().\n", n);
		return NULL;
	}
#endif
	return atoms_[n];
}

/*!
 * \brief Return size of Grain array
 */
int Molecule::nGrains()
{
	return nGrains_;
}

/*!
 * \brief Set nth Grain pointer
 */
void Molecule::setGrain(int n, Grain* grain)
{
#ifdef CHECKS
	if (grain == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer passed to Molecule::setGrain().\n");
		return;
	}
	if ((n < 0) || (n >= nGrains_))
	{
		msg.print("OUT_OF_RANGE - Atom index %i is out of range in Molecule::setGrain().\n", n);
		return;
	}
#endif
	grains_[n] = grain;
}

/*!
 * \brief Return nth Grain pointer
 */
Grain* Molecule::grain(int n)
{
#ifdef CHECKS
	if ((n < 0) || (n >= nGrains_))
	{
		msg.print("OUT_OF_RANGE - Grain index %i is out of range in Molecule::grain().\n", n);
		return NULL;
	}
#endif
	return grains_[n];
}

/*!
 * \brief Instantiate Molecule, setting Atom and Grain data
 */
bool Molecule::instantiate(bool setAtomData, Vec3<double> centre)
{
	// Loop over GrainDefinitions in source Species
	int grainIndex = 0;
	Atom *i;
	for (GrainDefinition* gd = species_->grainDefinitions(); gd != NULL; gd = gd->next)
	{
		// Initialise Grain
		if (!grains_[grainIndex]->initialise(gd))
		{
			msg.error("Failed to initialise Grain when instantiating Molecule for Species '%s'\n", species_->name());
			return FALSE;
		}

		// Add Atom pointers to Grain, using indices in the Species Atoms as offsets
		for (RefListItem<Atom,int>* ri = gd->atoms(); ri != NULL; ri = ri->next)
		{
			i = atoms_[ri->item->index()];
			i->setElement(ri->item->element());
			grains_[grainIndex]->addAtom(i);
		}
		++grainIndex;
	}
	
	// Set Atom data?
	if (setAtomData)
	{
		for (int n=0; n<nAtoms_; ++n)
		{
			atoms_[n]->setCharge(species_->atom(n)->charge());
			atoms_[n]->setAtomType(species_->atom(n)->atomType());
			atoms_[n]->setCoordinates(species_->atom(n)->r() + centre);
		}
	}
	return TRUE;
}

/*!
 * \brief Return index of Molecule
 */
int Molecule::index()
{
	return index_;
}

/*
// Intramolecular Terms
*/

/*!
 * \brief Clear intramolecular lists
 */
void Molecule::clearIntramolecular()
{
	bonds_.clear();
	angles_.clear();
}

/*!
 * \brief Add Bond reference
 */
void Molecule::addBond(Bond* b)
{
	bonds_.add(b);
}

/*!
 * \brief Return Bond list
 */
RefList<Bond,int>& Molecule::bonds()
{
	return bonds_;
}

/*!
 * \brief Add Angle reference
 */
void Molecule::addAngle(Angle* a)
{
	angles_.add(a);
}

/*!
 * \brief Return Angle list
 */
RefList<Angle,int>& Molecule::angles()
{
	return angles_;
}

/*!
 * \brief Create scaling matrix
 */
void Molecule::createScalingMatrix()
{
	int n, m, rootIndex = atoms_[0]->index();

	if (scalingMatrix_ != NULL) delete[] scalingMatrix_;
	scalingMatrix_ = new double*[nAtoms_];
	for (n=0; n<nAtoms_; ++n) scalingMatrix_[n] = new double[nAtoms_];
	
	// Unitise matrix (i.e. set all Atom pairs to interact fully)
	for (n=0; n<nAtoms_; ++n) for (m=0; m<nAtoms_; ++m) scalingMatrix_[n][m] = 1.0;
	
	// 'Torsion' interactions (set to 0.5)
	for (RefListItem<Bond,int>* refBond = bonds_.first(); refBond != NULL; refBond = refBond->next)
	{
		// TODO
	}
	
	// Bond interactions (set to 0.0)
	for (RefListItem<Bond,int>* refBond = bonds_.first(); refBond != NULL; refBond = refBond->next)
	{
		n = refBond->item->indexI() - rootIndex;
		m = refBond->item->indexJ() - rootIndex;
		scalingMatrix_[n][m] = 0.0;
		scalingMatrix_[m][n] = 0.0;
	}

	// Angle interactions (set to 0.0)
	for (RefListItem<Angle,int>* refAngle = angles_.first(); refAngle != NULL; refAngle = refAngle->next)
	{
		n = refAngle->item->indexI() - rootIndex;
		m = refAngle->item->indexK() - rootIndex;
		scalingMatrix_[n][m] = 0.0;
		scalingMatrix_[m][n] = 0.0;
	}
}

/*!
 * \brief Return scaling factor for supplied indices
 */
double Molecule::scaling(int indexI, int indexJ)
{
	// Get local Atom indices
	indexI -= atoms_[0]->index();
	indexJ -= atoms_[0]->index();
#ifdef CHECKS
	if ((indexI < 0) || (indexI >= nAtoms_))
	{
		msg.print("OUT_OF_RANGE - After rooting, supplied Atom indexI is out of range (%i) (nAtoms_ = %i).\n", indexI, nAtoms_);
		return 0.0;
	}
	if ((indexJ < 0) || (indexJ >= nAtoms_))
	{
		msg.print("OUT_OF_RANGE - After rooting, supplied Atom indexJ is out of range (%i) (nAtoms_ = %i).\n", indexJ, nAtoms_);
		return 0.0;
	}
#endif
	return scalingMatrix_[indexI][indexJ];
}

/*
// Manipulations
*/

/*!
 * \brief Calculate and return centre of geometry
 */
Vec3<double> Molecule::centre() const
{
	Vec3<double> cog;
	for (int n=0; n<nAtoms_; ++n) cog += atoms_[n]->r();
	return (cog / nAtoms_);
}

/*!
 * \brief Transform Molecule
 */
void Molecule::applyTransform(const Matrix3& transform)
{
	// Calculate Molecule centre of geometry
	Vec3<double> newR, cog = centre();
	
	// Apply transform
	for (int n=0; n<nAtoms_; ++n)
	{
		newR = transform * (atoms_[n]->r() - cog) + cog;
		atoms_[n]->setCoordinates(newR);
	}
}


/*!
 * \brief Transform selected Atoms
 */
void Molecule::applyTransform(const Matrix3& transform, const Vec3<double>& origin, int count, Atom** attachedAtoms)
{
	// Loop over supplied Atoms
	Vec3<double> newR;
	for (int n=0; n<count; ++n)
	{
		newR = transform * (attachedAtoms[n]->r() - origin) + origin;
		attachedAtoms[n]->setCoordinates(newR);
	}
}

/*!
 * \brief Randomise geometry
 * \details Randomise the geometry of the Molecule, exploring conformation space as best we can. This routine should be used when
 * instantiating the initial ensemble of Molecules such that a variety of conformations exist in the starting system. Each defined
 * Bond is taken in turn, and the connect Atoms at one end (chosen randomly) are rotated about the Bond axis.
 */ 
void Molecule::randomiseGeometry()
{
#ifdef CHECKS
	// Check for NULL Species pointer
	if (species_ == NULL)
	{
		msg.error("NULL_POINTER - Found NULL species_ pointer in Molecule::randomiseGeometry().\n");
		return;
	}
#endif
	// Use Bond definitions in parent Species
	int terminus;
	Matrix3 transform;
	Vec3<double> axis;
	Atom* localI, *localJ;
	Bond* b;
	for (RefListItem<Bond,int>* refBond = bonds_.first(); refBond != NULL; refBond = refBond->next)
	{
		b = refBond->item;

		// Select random terminus
		terminus = dUQMath::random() > 0.5;
		if (b->nAttached(terminus) < 2) continue;

		// Get local Atom pointers
#ifdef CHECKS
		if (b->i() == NULL)
		{
			msg.error("NULL_POINTER - NULL Atom pointer 'i' found in Molecule::randomiseGeometry().\n");
			return;
		}
		if (b->j() == NULL)
		{
			msg.error("NULL_POINTER - NULL Atom pointer 'j' found in Molecule::randomiseGeometry().\n");
			return;
		}
#endif
		localI = b->i();
		localJ = b->j();

		// Create axis rotation matrix
		axis = localI->r() - localJ->r();
		transform.createRotationAxis(axis.x, axis.y, axis.z, dUQMath::random()*360.0, TRUE);

		// Perform transform
		applyTransform(transform, terminus == 0 ? localI->r() : localJ->r(), b->nAttached(terminus), b->attached(terminus));
	}
}

// Shake geometry
void shakeGeometry();

