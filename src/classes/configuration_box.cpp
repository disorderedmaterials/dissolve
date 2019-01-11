/*
	*** Configuration - Periodic Box Functions
	*** src/classes/configuration_box.cpp
	Copyright T. Youngs 2012-2018

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
#include "base/processpool.h"

// Set relative box lengths
void Configuration::setRelativeBoxLengths(const Vec3<double> lengths)
{
	relativeBoxLengths_ = lengths;
}

// Set relative Box length
void Configuration::setRelativeBoxLength(int index, double length)
{
	relativeBoxLengths_[index] = length;
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

// Set Box angle
void Configuration::setBoxAngle(int index, double angle)
{
	boxAngles_[index] = angle;
}

// Return box angles
Vec3<double> Configuration::boxAngles() const
{
	return boxAngles_;
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

// Return requested side length for individual Cell
double Configuration::requestedCellDivisionLength() const
{
	return requestedCellDivisionLength_;
}

// Return Box
const Box* Configuration::box() const
{
	return box_;
}

// Set up periodic Box
bool Configuration::setUpBox(ProcessPool& procPool, double ppRange, int nExpectedAtoms, int boxNormalisationNPoints)
{
	// Remove old box if present
	if (box_ != NULL)
	{
		Messenger::printVerbose("Removing existing box definition...\n");
		delete box_;
	}

	// Determine volume for box, if a density was supplied. Otherwise, set to -1.0 to keep current cell lengths
	double volume = -1.0;
	if (density_ > 0.0) volume = nExpectedAtoms / atomicDensity();

	if (nonPeriodic_)
	{
		// Might need to increase pseudo-box volume to accommodate three times the ppRange
		if (volume < pow(ppRange*3.0, 3.0)) volume = pow(ppRange*3.0, 3.0);
		box_ = new NonPeriodicBox(volume);
	}
	else box_ = Box::generate(relativeBoxLengths_, boxAngles_, volume);

	// Need to calculate atomic density if it wasn't provided
	if (density_ < 0.0) density_ = nExpectedAtoms / box_->volume();

	// Store box axis lengths as new relativeBoxLengths_
	relativeBoxLengths_.set(box_->axisLength(0), box_->axisLength(1), box_->axisLength(2));

	Messenger::print("%s box created for Configuration '%s':\n", Box::boxType(box_->type()), name());
	Matrix3 axes = box_->axes();
	Messenger::print("Axes Matrix : A = %10.4e %10.4e %10.4e, length = %10.4e Angstroms\n", axes[0], axes[1], axes[2], relativeBoxLengths_.x);
	Messenger::print("              B = %10.4e %10.4e %10.4e, length = %10.4e Angstroms\n", axes[3], axes[4], axes[5], relativeBoxLengths_.y);
	Messenger::print("              C = %10.4e %10.4e %10.4e, length = %10.4e Angstroms\n", axes[6], axes[7], axes[8], relativeBoxLengths_.z);

	// Check cell lengths against pair potential range
	if (ppRange > box_->inscribedSphereRadius())
	{
		Messenger::error("PairPotential range (%f) is longer than the shortest non-minimum image distance (%f).\n", ppRange, box_->inscribedSphereRadius());
		return false;
	}

	// Generate cells within unit cell
	cells_.generate(box_, requestedCellDivisionLength_, ppRange, atomicDensity());

	// Determine maximal extent of RDF (from origin to centre of box)
	Vec3<double> half = box()->axes() * Vec3<double>(0.5,0.5,0.5);
	double maxR = half.magnitude(), inscribedSphereRadius = box()->inscribedSphereRadius();
	Messenger::print("\n");
	Messenger::print("Maximal extent for g(r) is %f Angstrom (half cell diagonal distance).\n", maxR);
	Messenger::print("Inscribed sphere radius (maximum RDF range avoiding periodic images) is %f Angstroms.\n", inscribedSphereRadius);
	if (requestedRDFRange_ < -1.5)
	{
		Messenger::print("Using maximal non-minimum image range for g(r).\n");
		rdfRange_ = inscribedSphereRadius;
	}
	else if (requestedRDFRange_ < -0.5)
	{
		Messenger::print("Using 90%% of maximal extent for g(r).\n");
		rdfRange_ = 0.90*maxR;
	}
	else
	{
		Messenger::print("Specific RDF range supplied (%f Angstroms).\n", requestedRDFRange_);
		rdfRange_ = requestedRDFRange_;
		if (rdfRange_ < 0.0)
		{
			Messenger::error("Negative RDF range requested.\n");
			return false;
		}
		else if (rdfRange_ > maxR)
		{
			Messenger::error("Requested RDF range is greater then the maximum possible extent for the Box.\n");
			return false;
		}
		else if (rdfRange_ > (0.90*maxR)) Messenger::warn("Requested RDF range is greater than 90%% of the maximum possible extent for the Box. FT may be suspect!\n");
	}
	// 'Snap' rdfRange_ to nearest bin width...
	rdfRange_ = int(rdfRange_/rdfBinWidth_) * rdfBinWidth_;
	Messenger::print("RDF range (snapped to bin width) is %f Angstroms.\n", rdfRange_);

	/*
	 * Load or calculate Box normalisation file (if we need one)
	 */
	if (rdfRange_ <= inscribedSphereRadius)
	{
		Messenger::print("No need for Box normalisation array since rdfRange is within periodic range.\n");
		boxNormalisation_.clear();
		double x = rdfBinWidth_*0.5;
		while (x < rdfRange_)
		{
			boxNormalisation_.addPoint(x, 1.0);
			x += rdfBinWidth_;
		}
	}
	else
	{
		// Attempt to load existing Box normalisation file
		if (!boxNormalisationFileName_.isEmpty())
		{
			// Master will open file and attempt to read it...
			bool loadResult;
			if (procPool.isMaster()) loadResult = boxNormalisation_.load(boxNormalisationFileName_);
			if (!procPool.broadcast(loadResult)) return false;

			// Did we successfully load the file?
			if (loadResult)
			{
				Messenger::print("Successfully loaded box normalisation data from file '%s'.\n", boxNormalisationFileName_.get());
				if (!boxNormalisation_.broadcast(procPool)) return false;
			}
			else Messenger::print("Couldn't load Box normalisation data - it will be calculated.\n");
		}

		// Did we successfully load the file?
		if (boxNormalisation_.nValues() <= 1)
		{
			// Only calculate if RDF range is greater than the inscribed sphere radius
			if (rdfRange_ <= inscribedSphereRadius)
			{
				Messenger::print("No need to calculate Box normalisation array since rdfRange is within periodic range.\n");
				boxNormalisation_.clear();
				double x = rdfBinWidth_*0.5;
				while (x < rdfRange_)
				{
					boxNormalisation_.addPoint(x, 1.0);
					x += rdfBinWidth_;
				}
			}
			else
			{
				Messenger::print("Calculating box normalisation array for g(r)...\n");
				if (!box()->calculateRDFNormalisation(procPool, boxNormalisation_, rdfRange_, rdfBinWidth_, boxNormalisationNPoints)) return false;
				
				// Save normalisation file so we don't have to recalculate it next time
				if (procPool.isMaster()) boxNormalisation_.save(boxNormalisationFileName_);
			}
		}
	}

	// Update the Box normalisation interpolation
	boxNormalisationInterpolation_.interpolate(Interpolator::LinearInterpolation);

	return true;
}

// Scale Box lengths (and associated Cells) by specified factor
void Configuration::scaleBox(double factor)
{
	// Scale Box
	box_->scale(factor);

	// Apply factor to Cells
	cells_.scale(factor);
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
const Data1D& Configuration::boxNormalisation() const
{
	return boxNormalisation_;
}

// Return interpolation of Box normalisation function
Interpolator& Configuration::boxNormalisationInterpolation()
{
	return boxNormalisationInterpolation_;
}


// Return cell array
CellArray& Configuration::cells()
{
	return cells_;
}
