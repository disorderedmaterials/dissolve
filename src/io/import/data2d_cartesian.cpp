/*
	*** Import - Cartesian Data2D
	*** src/io/import/data2d_cartesian.cpp
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

#include "io/import/data2d.h"
#include "math/data2d.h"
#include "math/filters.h"
#include "base/lineparser.h"

// Read cartesian data from specified file
bool Data2DImportFileFormat::importCartesian(LineParser& parser, Data2D& data)
{
	// Expect blocks with three columns (x, y, f(x,y))

	// Must have a template - otherwise tricky to work out axis limits without reading the whole file in twice
// 	if (
	
// 	return data.load(parser);
	return false;
}

