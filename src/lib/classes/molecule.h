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
class Grain;
class Species;
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
	// Array of Atom pointers
	Atom** atoms_;
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
	// Set nth Atom pointer
	void setAtom(int n, Atom* i);
	// Return nth Atom pointer
	Atom* atom(int n);
	// Return size of Grain array
	int nGrains();
	// Set nth Grain pointer
	void setGrain(int n, Grain* grain);
	// Return nth Grain pointer
	Grain* grain(int n);
	// Instantiate Molecule, setting Atom and Grain data
	bool instantiate(bool setAtomData, Vec3<double> centre = Vec3<double>());
	// Return index of Molecule
	int index();
	///@}


	/*!
	 * \name Intramolecular Terms
	 */
	///@{
	private:
	// Reflist of Bonds
	RefList<Bond,int> bonds_;
	// Reflist of Angles
	RefList<Angle,int> angles_;
	// Scaling matrix for intramolecular interactions
	double **scalingMatrix_;

	public:
	// Clear intramolecular lists
	void clearIntramolecular();
	// Add Bond reference
	void addBond(Bond* b);
	// Return Bond list
	RefList<Bond,int>& bonds();
	// Add Angle reference
	void addAngle(Angle* a);
	// Return Angle list
	RefList<Angle,int>& angles();
	// Create scaling matrix
	void createScalingMatrix();
	// Return scaling factor for supplied indices
	double scaling(int indexI, int indexJ);
	///@}


	/*!
	 * \name Manipulations
	 */
	///@{
	public:
	// Calculate and return centre of geometry
	Vec3<double> centre() const;
	// Transform Molecule
	void applyTransform(const Matrix3& transform);
	// Transform selected Atoms
	void applyTransform(const Matrix3& transform, const Vec3<double>& origin, int count, Atom** attachedAtoms);
	// Randomise geometry
	void randomiseGeometry();
	// Shake geometry
	void shakeGeometry();
	///@}
};

#endif
