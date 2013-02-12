/*
	*** dUQ - Composition
	*** src/lib/main/composition.cpp
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

#include "main/duq.h"
#include "classes/species.h"
#include "base/flag.h"

/*
// System Composition
*/

/*!
 * \brief Return total relative population of Species
 * \details Returns the total sum of all relative populations for all component Species
 */
double DUQ::totalRelative() const
{
	double total = 0.0;
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next) total += sp->relativePopulation();
	return total;
}

/*!
 * \brief Set multiplier for System components
 */
void DUQ::setMultiplier(int mult)
{
	multiplier_ = mult;
	
	SET_MODIFIED
}

/*!
 * \brief Return multiplier for System components
 */
int DUQ::multiplier() const
{
	return multiplier_;
}

/*!
 * \brief Set the atomic density of the system (atoms/A3)
 */
void DUQ::setAtomicDensity(double density)
{
	density_ = density;
	densityIsAtomic_ = TRUE;
}

/*!
 * \brief Set the chemical density of the system (g/cm3)
 */
void DUQ::setChemicalDensity(double density)
{
	density_ = density;
	densityIsAtomic_ = FALSE;
}

/*!
 * \brief Return the density of the system
 */
double DUQ::density() const
{
	return density_;
}

/*!
 * \brief Return whether the density is in atomic units (atoms/A3) or chemistry units (g/cm3)
 */
bool DUQ::densityIsAtomic() const
{
	return densityIsAtomic_;
}

/*!
 * \brief Return the atomic density of the system
 */
double DUQ::atomicDensity() const
{
	// Calculate atomic density from chemical density?
	if (densityIsAtomic_) return density_;
	
	// Determine total atomic mass and number of atoms in system
	double mass = 0.0, nAtoms = 0.0;
	for (Species* sp = species_.first(); sp != NULL; sp = sp->next)
	{
		mass += multiplier_*sp->relativePopulation() * sp->mass();
		nAtoms += multiplier_*sp->relativePopulation() * sp->nAtoms();
	}

	// Convert density from g/cm3 to g/A3
	double rho = nAtoms  / ((mass / AVOGADRO) / (density_ / 1.0E24));
	
	msg.printVerbose("Converting %f atoms (mass = %f, density = %f) to atomic density = %f\n", nAtoms, mass, density_, rho);

	return rho;
}

/*!
 * \brief Set relative box lengths
 */
void DUQ::setRelativeBoxLengths(const Vec3<double> lengths)
{
	relativeBoxLengths_ = lengths;
}

/*!
 * \brief Return relative box lengths
 */
Vec3<double> DUQ::relativeBoxLengths() const
{
	return relativeBoxLengths_;
}

/*!
 * \brief Set box angles
 */
void DUQ::setBoxAngles(const Vec3<double> angles)
{
	boxAngles_ = angles;
}

/*!
 * \brief Return box angles
 */
Vec3<double> DUQ::boxAngles() const
{
	return boxAngles_;
}
