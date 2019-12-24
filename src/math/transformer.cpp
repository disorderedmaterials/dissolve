/*
	*** Transformer
	*** src/math/transformer.cpp
	Copyright T. Youngs 2013-2019

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

#include "math/transformer.h"
#include "expression/generator.h"
#include "expression/variable.h"
#include "math/data1d.h"
#include "templates/array.h"
#include "math/data2d.h"
#include "data3d.h"


// Constructor
Transformer::Transformer()
{
	// Add persistent variable trio to equation
	x_ = equation_.createDoubleVariable("x", true);
	y_ = equation_.createDoubleVariable("y", true);
	z_ = equation_.createDoubleVariable("z", true);
	value_ = equation_.createDoubleVariable("value", true);
	valid_ = false;
}

// Destructor
Transformer::~Transformer()
{
}

// Copy constructor
Transformer::Transformer(const Transformer& source)
{
	(*this) = source;
}

// Assignment operator
void Transformer::operator=(const Transformer& source)
{
	// Set equation from old expression
	setEquation(source.text_.get());
	enabled_ = source.enabled_;
}

// Set whether transform is enabled
void Transformer::setEnabled(bool b)
{
	enabled_ = b;
}

// Return whether transform is enabled
bool Transformer::enabled() const
{
	return enabled_;
}

// Set equation, returning if construction was successful
bool Transformer::setEquation(const char* equation)
{
	text_ = equation;
	valid_ = equation_.set(text_);

	return valid_;
}

// Return text used to generate last equation_
const char* Transformer::text() const
{
	return text_.get();
}

// Return whether current equation is valid
bool Transformer::valid() const
{
	return valid_;
}

// Transform whole array, including application of pre/post transform shift
Array<double> Transformer::transformArray(Array<double> sourceX, Array<double> sourceY, Array<double> sourceZ, int target)
{
	// If transform is not enabled, return original array
	if (!enabled_)
	{
		switch (target)
		{
			case 0:
				return sourceX;
				break;
			case 1:
				return sourceY;
				break;
			case 2:
				return sourceZ;
		}
	}
	
	// If equation is not valid, just return original array
	if (!valid())
	{
		Messenger::printVerbose("Equation is not valid, so returning original array.\n");
		switch (target)
		{
			case 0: 
				return sourceX;
				break;
			case 1:
				return sourceY;
				break;
			case 2:
				return sourceZ;
		}
	}

	//??????????????????ASK TRISTAN - are x,y,z equal lengths???????????????????????
	if (sourceX.nItems() != sourceY.nItems())
	{
		Messenger::print("Error in Transformer::transformArray() - x and y array sizes do not match.\n");
		return Array<double>();
	}

	// Create new array
	Array<double> newArray(sourceX.nItems());

	//z_->set(0.0);
	
	// Loop over x points
	for (int n=0; n<sourceX.nItems(); ++n)
	{
		// Set x and y values in equation
		x_->set(sourceX[n]);
		y_->set(sourceY[n]);
		z_->set(sourceZ[n]);
		newArray[n] = equation_.asDouble();
	}

	return newArray;
}

// Transform 2D array
Array2D<double> Transformer::transformArray(Array2D<double> sourceValues, Array<double> sourceX, Array<double> sourceY)
{
	// If transform is not enabled, return original array
	if (!enabled_) return sourceValues;

	// If equation is not valid, just return original array
	if (!valid())
	{
		Messenger::print("Equation is not valid, so returning original array.\n");
		return (sourceValues);
	}
	

	Array2D<double> newArray2D(sourceValues.nRows(), sourceValues.nColumns());
	if (sourceValues.halved())
	{
		
		for(int r=0; r<sourceValues.nRows(); ++r)
			for(int c=r; c<sourceValues.nColumns(); ++c)
			{
				x_->set(sourceX.constAt(r));
				y_->set(sourceY.constAt(c));
				z_->set(sourceValues.constAt(r,c));
				newArray2D.at(r,c) = equation_.asDouble();
			}
	}
	else
	{	
		for(int i=0; i<sourceValues.nRows(); ++i)
			for(int j=0; j<sourceValues.nColumns(); ++j)
			{
				x_->set(sourceX.constAt(i));
				y_->set(sourceY.constAt(j));
				z_->set(sourceValues.constAt(i,j));
				newArray2D.at(i,j) = equation_.asDouble();
			}
	}
	
	return newArray2D;
}

/*
 * Static Functions
 */

// Transform Data1D values with supplied transformer
void Transformer::transform(Data1D& data, Transformer& valueTransformer)
{
	// If Transformer isn't enabled, return now
	if (!valueTransformer.enabled()) return;

	// Get references to x and value arrays, and take copies of each
	const Array<double>& xAxis = data.constXAxis();
	Array<double>& values = data.values();

	// Data1D x and value (y) arrays are of same size - loop over number of values
	for (int n=0; n<data.nValues(); ++n)
	{
		// Set values in equations
		valueTransformer.x_->set(xAxis.constAt(n));
		valueTransformer.y_->set(values.constAt(n));
		valueTransformer.value_->set(values.constAt(n));

		// Perform transform
		values[n] = valueTransformer.equation_.asDouble();
	}
}

// Transform Data2D values with supplied transformer
void Transformer::transform(Data2D& data, Transformer& valueTransformer)
{
	// If Transformer isn't enabled, return now
	if (!valueTransformer.enabled()) return;

	// Get references to x and value arrays, and take copies of each
	const Array<double>& xAxis = data.constXAxis();
	const Array<double>& yAxis = data.constYAxis();
	Array2D<double>& values = data.values();

	// Data2D x and y arrays may be of different sizes
	for (int i=0; i<xAxis.nItems(); ++i)
	{
		// Set x value in equation
		valueTransformer.x_->set(xAxis.constAt(i));

		// Loop over Y axis points
		for (int j=0; j<yAxis.nItems(); ++j)
		{
			// Set y and value (z) values in equation
			valueTransformer.y_->set(yAxis.constAt(j));
			valueTransformer.z_->set(values.at(i, j));
			valueTransformer.value_->set(values.at(i, j));

			// Perform transforms
			values.at(i,j) = valueTransformer.equation_.asDouble();
		}
	}
}

// Transform Data3D values with supplied transformer
void Transformer::transform(Data3D& data, Transformer& valueTransformer)
{
	// If Transformer isn't enabled, return now
	if (!valueTransformer.enabled()) return;

	// Get references to x and value arrays, and take copies of each
	const Array<double>& xAxis = data.constXAxis();
	const Array<double>& yAxis = data.constYAxis();
	const Array<double>& zAxis = data.constZAxis();
	Array3D<double>& values = data.values();

	// Data3D x, y and z arrays may be of different sizes
	for (int i=0; i<xAxis.nItems(); ++i)
	{
		// Set x value in equation
		valueTransformer.x_->set(xAxis.constAt(i));

		// Loop over Y axis points
		for (int j=0; j<yAxis.nItems(); ++j)
		{
			// Set y and value (z) values in equation
			valueTransformer.y_->set(yAxis.constAt(j));

			// Loop over z values
			for (int k=0; k<zAxis.nItems(); ++k)
			{
				valueTransformer.z_->set(values.at(i, j, k));
				valueTransformer.value_->set(values.at(i, j, k));

				// Perform transforms
				values.at(i, j, k) = valueTransformer.equation_.asDouble();
			}
		}
	}
}
