/*
	*** Named Value
	*** src/gui/uchroma/classes/namedvalue.cpp
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

#include "gui/uchroma/classes/namedvalue.h"

// Constructor
NamedValue::NamedValue() : ListItem<NamedValue>()
{
	// Variable Target
	name_ = "UnnamedValue";
	value_ = 0.0;
}

// Destructor
NamedValue::~NamedValue()
{
}

// Copy constructor
NamedValue::NamedValue(const NamedValue& source)
{
	(*this) = source;
}

// Assignment operator
void NamedValue::operator=(const NamedValue& source)
{
	// Variable Target
	name_ = source.name_;
	value_ = source.value_;
}

// Set name
void NamedValue::setName(const char* name)
{
	name_ = name;
}

// Return name
const char* NamedValue::name()
{
	return name_.get();
}

// Return whether name matches
bool NamedValue::isNamed(const char* name)
{
	return name_ == name;
}

// Set variable value
void NamedValue::setValue(double value)
{
	value_ = value;
}

// Return variable value
double NamedValue::value()
{
	return value_;
}

