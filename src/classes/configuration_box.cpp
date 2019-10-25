/*
	*** Configuration - Periodic Box Functions
	*** src/classes/configuration_box.cpp
	Copyright T. Youngs 2012-2019

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

#include "classes/configuration.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/grain.h"
#include "modules/energy/energy.h"
#include "base/lineparser.h"
#include "base/processpool.h"

// Create Box definition with  specifeid lengths and angles
bool Configuration::createBox(const Vec3<double> lengths, const Vec3<double> angles, bool nonPeriodic)
{
	// Remove old box if present
	if (box_ != NULL)
	{
		Messenger::printVerbose("Removing existing Box definition for Configuration '%s'...\n", niceName());
		delete box_;
		box_ = NULL;
	}

	if (nonPeriodic) box_ = new NonPeriodicBox(1.0);
	else box_ = Box::generate(lengths, angles);

	return true;
}

// Return Box
const Box* Configuration::box() const
{
	return box_;
}

// Scale Box lengths (and associated Cells) by specified factor
void Configuration::scaleBox(double factor)
{
	// Scale Box
	box_->scale(factor);

	// Apply factor to Cells
	cells_.scale(factor);
}

// Set requested size factor for Box
void Configuration::setRequestedSizeFactor(double factor)
{
	requestedSizeFactor_ = factor;
}

// Return requested size factor for Box
double Configuration::requestedSizeFactor()
{
	return requestedSizeFactor_;
}

// Return last size factor applied to Box / Cells
double Configuration::appliedSizeFactor()
{
	return appliedSizeFactor_;
}

// Set requested side length for individual Cell
void Configuration::setRequestedCellDivisionLength(double a)
{
	requestedCellDivisionLength_ = a;
}

// Return requested side length for individual Cell
double Configuration::requestedCellDivisionLength() const
{
	return requestedCellDivisionLength_;
}

// Return cell array
CellArray& Configuration::cells()
{
	return cells_;
}

// Return cell array
const CellArray& Configuration::constCells() const
{
	return cells_;
}


// Scale Box, Cells, and Molecule geometric centres according to current size factor
void Configuration::applySizeFactor(const PotentialMap& potentialMap)
{
	const double reductionFactor = 0.95;

	while (true)
	{
		// Calculate ratio between current and applied size factors for use later on
		const double sizeFactorRatio = requestedSizeFactor_ / appliedSizeFactor_;

		// Check current vs applied size factors (via the ratio) - if unequal, perform scaling and set the new applied size factor
		if (fabs(sizeFactorRatio - 1.0) > 1.0e-5)
		{
			Messenger::print("Requested SizeFactor for Configuration is %f, current SizeFactor is %f, so scaling Box contents.\n", requestedSizeFactor_, appliedSizeFactor_);

			/*
			 * Recalculate all Atom positions, molecule-by-molecule
			 * 
			 * First, work out the centre of geometry of the Molecule, and fold it into the Box.
			 * Calculate the scaled centre of geometry coordinate by dividing by the old scale factor, and multiplying by the new one.
			 * Calculate the minimum image delta between each Atom and the original center of geometry.
			 * Add this delta on to the new centre of geometry to get the new Atom coordinate.
			 */

			Vec3<double> oldCog, newCog, newPos;
			for (int n=0; n<molecules_.nItems(); ++n)
			{
				// Get Molecule pointer
				Molecule* mol = molecules_[n];

				// Calculate current and new centre of geometry
				oldCog = box()->fold(mol->centreOfGeometry(box()));
				newCog = oldCog * sizeFactorRatio;

				// Loop over Atoms in Molecule, setting new coordinates as we go. Remove Atom from its current Cell at the same time
				for (int m=0; m<mol->nAtoms(); ++m)
				{
					// Get Atom pointer
					Atom* i = mol->atom(m);

					// Remove from its current Cell
					if (i->cell()) i->cell()->removeAtom(i);

					// Calculate and set new position
					newPos = newCog + box()->minimumVector(i->r(), oldCog);
					i->setCoordinates(newPos);
				}
			}

			// Now scale the Box and its Cells
			scaleBox(sizeFactorRatio);

			// Re-assign all Atoms to Cells
			updateCellContents();

			// Store new size factors
			appliedSizeFactor_ = requestedSizeFactor_;

			// Can now break out of the loop
			break;
		}

		// Now check the current sizeFactor or energy
		//  -- If the current sizeFactor is 1.0, break
		//  -- Otherwise, check energy - if it is negative, reduce requested size factor and loop
		//  -- If energy is positive, break
		if (fabs(requestedSizeFactor_ - 1.0) < 1.0e-5) break;
		else if (EnergyModule::interMolecularEnergy(processPool_, this, potentialMap) <= 0.0)
		{
			requestedSizeFactor_ *= reductionFactor;
			if (requestedSizeFactor_ < 1.0) requestedSizeFactor_ = 1.0;
			Messenger::print("Intermolecular energy is zero or negative, so reducing SizeFactor to %f\n", requestedSizeFactor_);
		}
		else
		{
			Messenger::print("Intermolecular energy is positive, so SizeFactor remains at %f\n", requestedSizeFactor_);
			break;
		}
	}
}
