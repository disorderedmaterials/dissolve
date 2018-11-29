/*
	*** Equation Variable
	*** src/gui/uchroma/classes/equationvariable.h
	Copyright T. Youngs 2012-2018.

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

#ifndef DISSOLVE_UCHROMAEQUATIONVARIABLE_H
#define DISSOLVE_UCHROMAEQUATIONVARIABLE_H

#include "base/charstring.h"
#include "templates/listitem.h"

// Forward Declarations
class ExpressionVariable;

// Equation Variable
class EquationVariable : public ListItem<EquationVariable>
{
	public:
	// Constructor / Destructor
	EquationVariable();
	~EquationVariable();


	/*
	 * Variable Target
	 */
	private:
	// Name of target variable
	CharString name_;
	// Target ExpressionVariable pointer
	ExpressionVariable* variable_;
	// Value
	double value_;
	// Whether minimum maximum limits are enabled
	bool minimumLimitEnabled_, maximumLimitEnabled_;
	// Values for minimum / maximum limits
	double minimumLimit_, maximumLimit_;
	// Whether the variable is to be fit
	bool fit_;
	// Whether the variable is used in the current equation
	bool used_;

	public:
	// Set name
	void setName(const char* name);
	// Return name
	const char* name();
	// Set variable target
	void setVariable(ExpressionVariable* variable);
	// Reset variable target (to NULL) and usage status
	void resetVariable();
	// Return variable target
	ExpressionVariable* variable();
	// Set value
	void setValue(double value);
	// Return value
	double value();
	// Poke value to variable target
	void pokeValueToVariable();
	// Set minimum limit
	void setMinimumLimit(bool enabled, double value);
	// Return whether minimum limit is enabled
	bool minimumLimitEnabled();
	// Return minimum limit value
	double minimumLimit();
	// Set maximum limit
	void setMaximumLimit(bool enabled, double value);
	// Return whether maximum limit is enabled
	bool maximumLimitEnabled();
	// Return maximum limit value
	double maximumLimit();
	// Set whether this variable should be fit
	void setFit(bool fit);
	// Return whether this variable should be fit
	bool fit();
	// Set whether this variable is used in the current equation
	void setUsed(bool used);
	// Return whether this variable is used in the current equation
	bool used();
};

#endif
