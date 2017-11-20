/*
	*** Transformer
	*** src/gui/uchroma/classes/transformer.cpp
	Copyright T. Youngs 2013-2017

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/uchroma/classes/transformer.h"
#include "gui/uchroma/expression/variable.h"
#include "templates/array.h"

// Constructor
Transformer::Transformer()
{
	// Add permanent variable trio to equation
	x_ = equation_.createVariable("x", NULL, true);
	y_ = equation_.createVariable("y", NULL, true);
	z_ = equation_.createVariable("z", NULL, true);
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
bool Transformer::enabled()
{
	return enabled_;
}

// Set equation, returning if Tree construction was successful
bool Transformer::setEquation(const char* equation)
{
	text_ = equation;
	valid_ = equation_.generate(equation);
	return valid_;
}

// Return text used to generate last equation_
const char* Transformer::text()
{
	return text_;
}

// Return whether current equation is valid
bool Transformer::valid()
{
	return valid_;
}

// Transform single value
double Transformer::transform(double x, double y, double z)
{
	// If equation is not valid, just return
	if (!valid_)
	{
		Messenger::print("Equation is not valid, so returning 0.0.\n");
		return 0.0;
	}

	x_->set(x);
	y_->set(y);
	z_->set(z);

	bool success;
	return equation_.execute(success);
}

// Transform whole array, including application of pre/post transform shift
Array<double> Transformer::transformArray(Array<double> sourceX, Array<double> sourceY, double z, int target)
{
	// If transform is not enabled, return original array
	if (!enabled_) return (target == 0 ? sourceX : sourceY);

	// If equation is not valid, just return original array
	if (!valid_)
	{
		Messenger::print("Equation is not valid, so returning original array.\n");
		return (target == 0 ? sourceX : sourceY);
	}

	if (sourceX.nItems() != sourceY.nItems())
	{
		Messenger::print("Error in Transformer::transformArray() - x and y array sizes do not match.\n");
		return Array<double>();
	}

	// Create new array, and create reference to target array
	Array<double> newArray(sourceX.nItems());

	z_->set(z);
	bool success;
	// Loop over x points
	for (int n=0; n<sourceX.nItems(); ++n)
	{
		// Set x and y values in equation
		x_->set(sourceX[n]);
		y_->set(sourceY[n]);
		newArray[n] = equation_.execute(success);
		if (!success) break;
	}

	return newArray;
}
