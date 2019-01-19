/*
	*** EquationVariable
	*** src/gui/uchroma/classes/equationvariable.cpp
	Copyright T. Youngs 2012-2019

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

#include "gui/uchroma/classes/equationvariable.h"
#include "base/messenger.h"
#include "expression/variable.h"

// Constructor
EquationVariable::EquationVariable() : ListItem<EquationVariable>()
{
	variable_ = NULL;
	value_ = 1.0;
	minimumLimitEnabled_ = false;
	maximumLimitEnabled_ = false;
	minimumLimit_ = -10.0;
	maximumLimit_ = 10.0;
	used_ = false;
	fit_ = true;
}

// Destructor
EquationVariable::~EquationVariable()
{
}

// Set name
void EquationVariable::setName(const char* name)
{
	name_ = name;
}

// Return name
const char* EquationVariable::name()
{
	return name_.get();
}

// Set variable target
void EquationVariable::setVariable(ExpressionVariable* variable)
{
	variable_ = variable;
}

// Reset variable target (to NULL) and usage status
void EquationVariable::resetVariable()
{
	variable_ = NULL;
	used_ = false;
}

// Return variable target
ExpressionVariable* EquationVariable::variable()
{
	return variable_;
}

// Set initial value
void EquationVariable::setValue(double value)
{
	value_ = value;
}

// Return initial value
double EquationVariable::value()
{
	return value_;
}

// Poke value to variable target
void EquationVariable::pokeValueToVariable()
{
	if (variable_ == NULL ) Messenger::print("Internal Error: Tried to poke a value into a variable which didn't exist.\n");
	else variable_->set(value_);
}

// Set minimum limit
void EquationVariable::setMinimumLimit(bool enabled, double value)
{
	minimumLimit_ = value;
	minimumLimitEnabled_ = enabled;
}

// Return whether minimum limit is enabled
bool EquationVariable::minimumLimitEnabled()
{
	return minimumLimitEnabled_;
}

// Return minimum limit value
double EquationVariable::minimumLimit()
{
	return minimumLimit_;
}

// Set maximum limit
void EquationVariable::setMaximumLimit(bool enabled, double value)
{
	maximumLimit_ = value;
	maximumLimitEnabled_ = enabled;
}

// Return whether maximum limit is enabled
bool EquationVariable::maximumLimitEnabled()
{
	return maximumLimitEnabled_;
}

// Return maximum limit value
double EquationVariable::maximumLimit()
{
	return maximumLimit_;
}

// Set whether this variable should be fit
void EquationVariable::setFit(bool fit)
{
	fit_ = fit;
}

// Return whether this variable should be fit
bool EquationVariable::fit()
{
	return fit_;
}

// Set whether this variable is used in the current equation
void EquationVariable::setUsed(bool used)
{
	used_ = used;
}

// Return whether this variable is used in the current equation
bool EquationVariable::used()
{
	return used_;
}
