/*
	*** Configuration - Periodic Box Functions
	*** src/classes/configuration_box.cpp
	Copyright T. Youngs 2012-2018

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

#include "classes/configuration.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/grain.h"
#include "base/processpool.h"

// Set relative box lengths
void Configuration::setRelativeBoxLengths(const Vec3<double> lengths)
{
	relativeBoxLengths_ = lengths;
}

// Return relative box lengths
Vec3<double> Configuration::relativeBoxLengths() const
{
	return relativeBoxLengths_;
}

// Set box angles
void Configuration::setBoxAngles(const Vec3<double> angles)
{
	boxAngles_ = angles;
}

// Return box angles
Vec3<double> Configuration::boxAngles() const
{
	return boxAngles_;
}

// Set whether the configuration is non-periodic
void Configuration::setNonPeriodic(bool b)
{
	nonPeriodic_ = b;
}

// Return whether the configuration is non-periodicI
bool Configuration::nonPeriodic() const
{
	return nonPeriodic_;
}

// Set requested side length for individual Cell
void Configuration::setRequestedCellDivisionLength(double a)
{
	requestedCellDivisionLength_ = a;
}

// Return equested side length for individual Cell
double Configuration::requestedCellDivisionLength()
{
	return requestedCellDivisionLength_;
}

// Return Box
const Box* Configuration::box() const
{
	return box_;
}

// Set up periodic Box
bool Configuration::setUpBox(double ppRange, int nExpectedAtoms)
{
	// Remove old box if present
	if (box_ != NULL)
	{
		Messenger::print("Removing existing box definition...\n");
		delete box_;
	}

	// Determine volume for box, if a density was supplied. Otherwise, set to -1.0 to keep current cell lengths
	double volume = -1.0;
	if (density_ > 0.0) volume = nExpectedAtoms / atomicDensity();

	// Determine box type from supplied lengths / angles
	bool rightAlpha = (fabs(boxAngles_.x-90.0) < 0.001);
	bool rightBeta = (fabs(boxAngles_.y-90.0) < 0.001);
	bool rightGamma = (fabs(boxAngles_.z-90.0) < 0.001);
	if (nonPeriodic_)
	{
		// Might need to increase pseudo-box volume to accommodate three times the ppRange
		if (volume < pow(ppRange*3.0, 3.0)) volume = pow(ppRange*3.0, 3.0);
		box_ = new NonPeriodicBox(volume);
	}
	else if (rightAlpha && rightBeta && rightGamma)
	{
		// Cubic or orthorhombic
		bool abSame = (fabs(relativeBoxLengths_.x-relativeBoxLengths_.y) < 0.0001);
		bool acSame = (fabs(relativeBoxLengths_.x-relativeBoxLengths_.z) < 0.0001);
		if (abSame && acSame) box_ = new CubicBox(volume, relativeBoxLengths_.x);
		else box_ = new OrthorhombicBox(volume, relativeBoxLengths_);
	}
	else if (rightAlpha && (!rightBeta) && rightGamma) box_ = new MonoclinicBox(volume, relativeBoxLengths_, boxAngles_.y);	// Monoclinic
	else
	{
		// Triclinic
		box_ = new TriclinicBox(volume, relativeBoxLengths_, boxAngles_);
	}

	// Need to calculate atomic density if it wasn't provided
	if (density_ < 0.0) density_ = nExpectedAtoms / box_->volume();

	Messenger::print("--> %s box created for system.\n", Box::boxType(box_->type()));
	Matrix3 axes = box_->axes();
	Messenger::print("--> Axes Matrix : A = %10.4e %10.4e %10.4e, length = %10.4e Angstroms\n", axes[0], axes[1], axes[2], box_->axisLength(0));
	Messenger::print("-->               B = %10.4e %10.4e %10.4e, length = %10.4e Angstroms\n", axes[3], axes[4], axes[5], box_->axisLength(1));
	Messenger::print("-->               C = %10.4e %10.4e %10.4e, length = %10.4e Angstroms\n", axes[6], axes[7], axes[8], box_->axisLength(2));

	// Check cell lengths against pair potential range
	if (ppRange > box_->inscribedSphereRadius())
	{
		Messenger::error("PairPotential range (%f) is longer than the shortest non-minimum image distance (%f).\n", ppRange, box_->inscribedSphereRadius());
		return false;
	}

	// Generate cells within unit cell
	cells_.generate(box_, requestedCellDivisionLength_, ppRange, atomicDensity());
	 
	return true;
}

// Set box normalisation array to load/save for this configuration
void Configuration::setBoxNormalisationFile(const char* filename)
{
	boxNormalisationFileName_ = filename;
}

// Return box normalisation file to load/save for this configuration
const char* Configuration::boxNormalisationFileName() const
{
	return boxNormalisationFileName_.get();
}

// Return current Box normalisation array
const XYData& Configuration::boxNormalisation() const
{
	return boxNormalisation_;
}

// Return cell array
CellArray& Configuration::cells()
{
	return cells_;
}
