/*
	*** Reference Point
	*** src/gui/referencepoint.cpp
	Copyright T. Youngs 2012-2019

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

#include "gui/referencepoint.h"

// Constructor / Destructor
ReferencePoint::ReferencePoint() : ListItem<ReferencePoint>()
{
}

ReferencePoint::~ReferencePoint()
{
}

// Set suffix for data items
void ReferencePoint::setSuffix(const char* suffix)
{
	suffix_ = suffix;
}

// Return suffix for data items
const char* ReferencePoint::suffix() const
{
	return suffix_.get();
}

// Set restart file from which the reference point data was read
void ReferencePoint::setRestartFile(const char* restartFile)
{
	restartFile_ = restartFile;
}

// Return restart file from which the reference point data was read
const char* ReferencePoint::restartFile() const
{
	return restartFile_.get();
}
