/*
	*** Generic Item Container - Array2D<double>
	*** src/templates/genericitemcontainer_array2ddouble.h
	Copyright T. Youngs 2012-2017

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

#ifndef DUQ_GENERICITEMCONTAINER_ARRAY2DDOUBLE_H
#define DUQ_GENERICITEMCONTAINER_ARRAY2DDOUBLE_H

#include "templates/genericitemcontainer.h"

// GenericItemContainer< Array2D<double> >
template <> class GenericItemContainer< Array2D<double> > : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer< Array2D<double> >(const char* name, int flags = 0) : GenericItem(name, flags)
	{
		itemClass_ = GenericItem::Array2DDoubleClass;
	}
	// Data item
	Array2D<double> data;


	/*
	 * Read / Write
	 */
	public:
	// Read item contents from specified LineParser
	bool read(LineParser& parser)
	{
		// First line is number of elements in array
		if (parser.getArgsDelim(LineParser::Defaults) != 0) return false;
		int nRows = parser.argi(0), nColumns = parser.argi(1);
		data.initialise(nRows, nColumns);

		// Read in data
		for (int row=0; row<nRows; ++row)
		{
			for (int column=0; column<nColumns; ++column)
			{
				if (!parser.getArgsDelim(LineParser::Defaults)) return false;
				data.ref(row, column) = parser.argd(0);
			}
		}
		return true;
	}
	// Write item contents to specified LineParser
	bool write(LineParser& parser)
	{
		// Write number of rows and columns
		if (!parser.writeLineF("%i  %i\n", data.nRows(), data.nColumns())) return false;

		// Write items
		for (int row=0; row<data.nRows(); ++row)
		{
			for (int column=0; column<data.nColumns(); ++column) if (!parser.writeLineF("%16.9e\n", data.value(row, column))) return false;
		}

		return true;
	}


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast item contsnts
	bool broadcast(ProcessPool& procPool, int root)
	{
		return procPool.broadcast(data, root);
	}
};

#endif
