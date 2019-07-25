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
	Messenger::print("Configuration box volume is %f cubic Angstroms (reciprocal volume = %e)\n", box_->volume(), box_->reciprocalVolume());

	Messenger::print("%s box created for Configuration '%s':\n", Box::boxTypes().keyword(box_->type()), name());
	Matrix3 axes = box_->axes();
	Messenger::print("Axes Matrix : A = %10.4e %10.4e %10.4e, length = %10.4e Angstroms\n", axes[0], axes[1], axes[2], box_->axisLengths().x);
	Messenger::print("              B = %10.4e %10.4e %10.4e, length = %10.4e Angstroms\n", axes[3], axes[4], axes[5], box_->axisLengths().y);
	Messenger::print("              C = %10.4e %10.4e %10.4e, length = %10.4e Angstroms\n", axes[6], axes[7], axes[8], box_->axisLengths().z);

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
