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
