/*
	*** Species Molecule Definition
	*** src/lib/classes/molecule.h
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

#ifndef DUQ_MOLECULE_H
#define DUQ_MOLECULE_H

#include "templates/reflist.h"
#include "templates/vector3.h"

// Forward Declarations
class Angle;
class Atom;
class Bond;
class Box;
class Grain;
class Species;
class SpeciesAtom;
class SpeciesGrain;
class Matrix3;

/*!
 * \brief Molecule Definition
 * \details A Molecule can be thought of as an instance of a Species which has a physical presence in the Model.
 * A Molecule does not itself own its Atoms or Grains. Instead, pointers to the relevant Atoms/Grains in the parent
 * Configuration are stored.
 */
class Molecule : public ListItem<Molecule>
{
	public:
	// Constructor
	Molecule();
	// Destructor
	~Molecule();


	/*!
	 * \name Atoms / Grains
	 */
	///@{
	private:
	// Source Species
	Species* species_;
	// Number of Atom (pointers) in Atom array
	int nAtoms_;
	// Array of pointers to Atom pointers
	Atom*** atoms_;
	// Number of Grain (pointers) in the Grain array
	int nGrains_;
	// Array of Grain pointers
	Grain** grains_;
	// Index of Molecule
	int index_;

	public:
	// Initialise Molecule arrays suitable for Species provided
	bool initialise(Species* sp, int index);
	// Return Species from which Molecule was initialised
	Species* species();
	// Return size of Atom array
	int nAtoms();
	// Set nth pointer to Atom pointer
	bool setupAtom(int n, Atom** i, SpeciesAtom* source);
	// Return nth Atom pointer
	Atom* atom(int n) const;
	// Return size of Grain array
	int nGrains();
	// Set nth Grain pointer
	bool setupGrain(int n, Grain* grain, SpeciesGrain* source);
	// Return nth Grain pointer
	Grain* grain(int n);
	// Return index of Molecule
	int index();
	///@}


	/*!
	 * \name Manipulations
	 */
	///@{
	public:
	// Calculate and return centre of geometry
	Vec3<double> centre(const Box* box) const;
	// Transform molecule with supplied matrix and translation vector
	void applyTransform(const Box* box, const Matrix3& transform);
	// Transform selected Atoms
	void applyTransform(const Box* box, const Matrix3& transform, const Vec3<double>& origin, int nTargetAtoms, int* targetIndices);
	// Set centre of geometry of molecule
	void setCentre(const Box* box, const Vec3<double> newCentre);
	// Randomise geometry
	void randomiseGeometry(const Box* box);
	// Shake geometry
	void shakeGeometry();
	///@}
};

#endif
