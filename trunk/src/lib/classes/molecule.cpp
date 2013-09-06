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
#include "classes/grain.h"
#include "classes/atomtype.h"
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
		return false;
	}
	
	// Check for double initialisation
	if (atoms_ != NULL)
	{
		printf("DOUBLE_INIT - Arrays already exist in Molecule::initialise().\n");
		return false;
	}
	
	// Get data from Species and create local arrays
	nAtoms_ = species_->nAtoms();
	atoms_ = new Atom**[nAtoms_];
	nGrains_ = species_->nGrains();
	grains_ = new Grain*[nGrains_];
	
	index_ = index;

	return true;
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
 * \brief Set nth pointer to Atom pointer
 */
bool Molecule::setupAtom(int n, Atom** i, SpeciesAtom* source)
{
#ifdef CHECKS
	if (i == NULL)
	{
		msg.error("NULL_POINTER - NULL Atom pointer passed to Molecule::setupAtom().\n");
		return false;
	}
	if (source == NULL)
	{
		msg.error("NULL_POINTER - NULL SpeciesAtom pointer passed to Molecule::setupAtom().\n");
		return false;
	}
	if ((n < 0) || (n >= nAtoms_))
	{
		msg.print("OUT_OF_RANGE - Atom index %i is out of range in Molecule::setupAtom().\n", n);
		return false;
	}
#endif
	atoms_[n] = i;

	// Copy information from supplied SpeciesAtom
	(*atoms_[n])->setElement(source->element());
	(*atoms_[n])->setCharge(source->charge());
	(*atoms_[n])->setAtomTypeIndex(source->atomType()->index());
	(*atoms_[n])->setMolecule(this, source->index());
	(*atoms_[n])->setCoordinatesNasty(source->r());

	return true;
}

/*!
 * \brief Return nth Atom pointer
 */
Atom* Molecule::atom(int n) const
{
#ifdef CHECKS
	static Atom dummy;
	if ((n < 0) || (n >= nAtoms_))
	{
		msg.print("OUT_OF_RANGE - Atom index %i is out of range in Molecule::atom().\n", n);
		return &dummy;
	}
	if (atoms_[n] == NULL)
	{
		msg.print("NULL_POINTER - The pointer to Atom %i in Molecule::atom() is NULL.\n", n);
		return &dummy;
	}
#endif
	return (*atoms_[n]);
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
bool Molecule::setupGrain(int n, Grain* grain, SpeciesGrain* source)
{
#ifdef CHECKS
	if (grain == NULL)
	{
		msg.error("NULL_POINTER - NULL Grain pointer passed to Molecule::setupGrain().\n");
		return false;
	}
	if (source == NULL)
	{
		msg.error("NULL_POINTER - NULL SpeciesGrain pointer passed to Molecule::setupGrain().\n");
		return false;
	}
	if ((n < 0) || (n >= nGrains_))
	{
		msg.print("OUT_OF_RANGE - Grain index %i is out of range in Molecule::setupGrain().\n", n);
		return false;
	}
#endif
	grains_[n] = grain;
	grains_[n]->setParent(this);
	if (!grains_[n]->initialise(source)) return false;

	// Set Atoms within Grain
	for (int i=0; i<source->nAtoms(); ++i) if (!grains_[n]->addAtom( atoms_[source->atom(i)->item->index()] )) return false;
	
	return true;
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
 * \brief Return index of Molecule
 */
int Molecule::index()
{
	return index_;
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
	XXX This is tragically bad, since no minimum image is performed.  Fix this!
	for (int n=0; n<nAtoms_; ++n) cog += atom(n)->r();
	return (cog / nAtoms_);
}

/*!
 * \brief Transform molecule
 */
void Molecule::applyTransform(const Matrix3& transform)
{
	// Calculate Molecule centre of geometry
	Vec3<double> newR, cog = centre();
	
	// Apply transform
	for (int n=0; n<nAtoms_; ++n)
	{
		newR = transform * (atom(n)->r() - cog) + cog;
		atom(n)->setCoordinates(newR);
	}
}


/*!
 * \brief Transform selected Atoms
 */
void Molecule::applyTransform(const Matrix3& transform, const Vec3< double >& origin, int nTargetAtoms, int* targetIndices)
{
	// Loop over supplied Atoms
	Vec3<double> newR;
	Atom* i;
	for (int n=0; n< nTargetAtoms; ++n)
	{
		i = atom(targetIndices[n]);
		newR = transform * (i->r() - origin) + origin;
		i->setCoordinates(newR);
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
	for (SpeciesBond* b = species_->bonds(); b!= NULL; b = b->next)
	{
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
		localI = atom(b->i()->index());
		localJ = atom(b->j()->index());

		// Create axis rotation matrix
		axis = localI->r() - localJ->r();
		transform.createRotationAxis(axis.x, axis.y, axis.z, dUQMath::random()*360.0, true);

		// Perform transform
		applyTransform(transform, terminus == 0 ? localI->r() : localJ->r(), b->nAttached(terminus), b->attachedIndices(terminus));
	}
}

// Shake geometry
void shakeGeometry(); // TODO

