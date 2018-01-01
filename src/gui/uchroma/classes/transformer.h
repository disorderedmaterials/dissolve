/*
	*** Transformer
	*** src/gui/uchroma/classes/transformer.h
	Copyright T. Youngs 2013-2018

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

#ifndef DUQ_UCHROMATRANSFORMER_H
#define DUQ_UCHROMATRANSFORMER_H

#include "gui/uchroma/expression/expression.h"
#include "base/charstring.h"
#include "templates/array.h"

// Forward Declarations
class Variable;

// Transformer
class Transformer
{
	public:
	// Constructor / Destructor
	Transformer();
	~Transformer();
	// Copy constructor
	Transformer(const Transformer& source);
	// Assignment operator
	void operator=(const Transformer& source);


	/*
	 * Tree containing transform equation
	 */
	private:
	// Whether transform is enabled
	bool enabled_;
	// Transform equation
	Expression equation_;
	// Text used to generate last equation_
	CharString text_;
	// Whether current equation is valid
	bool valid_;
	// X variable in equation
	Variable* x_;
	// Y variable in equation
	Variable* y_;
	// Z variable in equation
	Variable* z_;

	public:
	// Set whether transform is enabled
	void setEnabled(bool b);
	// Return whether transform is enabled
	bool enabled();
	// Set equation, returning if Tree construction was successful
	bool setEquation(const char* equation);
	// Return text used to generate last equation_
	const char* text();
	// Return whether current equation is valid
	bool valid();
	// Transform single value
	double transform(double x, double y, double z);
	// Transform whole array, including application of pre/post transform shift
	Array<double> transformArray(Array< double > sourceX, Array< double > sourceY, double z, int target);
};

#endif
