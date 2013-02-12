/*
	*** Simple Flag
	*** src/lib/base/flag.cpp
	Copyright T. Youngs 2012-2013

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

#include "base/flag.h"

// External Declarations
Flag dUQFlag;

// Constructor
Flag::Flag()
{
	flag_ = false;
}

/*
// Flag and Access
*/

// Set flag to TRUE
void Flag::setTrue()
{
	flag_ = true;
}

// Set flag to FALSE
void Flag::setFalse()
{
	flag_ = false;
}

// Return flag
bool Flag::flag()
{
	return flag_;
}

