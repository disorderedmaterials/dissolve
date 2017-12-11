/*
	*** Generic Item Container - Array2D<T>
	*** src/templates/genericitemcontainer_array2d.h
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

#ifndef DUQ_GENERICITEMCONTAINER_ARRAY2D_H
#define DUQ_GENERICITEMCONTAINER_ARRAY2D_H

#include "templates/genericitemcontainer.h"

// GenericItemContainer< Array2D<T> >
template <class T> class GenericItemContainer< Array2D<T> > : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer< Array2D<T> >(const char* name, int flags = 0) : GenericItem(name, flags)
	{
	}
	// Data item
	Array2D<T> data;


	/*
	 * Item Class
	 */
	protected:
	// Create a new GenericItem containing same class as current type
	GenericItem* newItem(const char* name, int flags = 0)
	{
		return new GenericItemContainer< Array2D<T> >(name, flags);
	}

	public:
	// Return class name contained in item
	const char* itemClassName()
	{
		static CharString className("Array2D<%s>", T::itemClassName());
		return className.get();
	}


	/*
	 * I/O
	 */
	public:
	// Write data through specified parser
	bool write(LineParser& parser)
	{
		parser.writeLineF("%i  %i\n", data.nRows(), data.nColumns());
		for (int row=0; row<data.nRows(); ++row)
		{
			for (int column=0; column<data.nColumns(); ++column)
			{
				if (!data.ref(row, column).write(parser)) return false;
			}
		}
		return true;
	}
	// Read data through specified parser
	bool read(LineParser& parser)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		int nRows = parser.argi(0), nColumns = parser.argi(1);
		data.initialise(nRows, nColumns);

		// Read in data
		for (int row=0; row<nRows; ++row)
		{
			for (int column=0; column<nColumns; ++column)
			{
				if (!data.ref(row, column).read(parser)) return false;
			}
		}
		return true;
	}


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast item contents
	bool broadcast(ProcessPool& procPool, int root)
	{
		for (int row=0; row<data.nRows(); ++row)
		{
			for (int column=0; column<data.nColumns(); ++column)
			{
				if (!data.ref(row, column).broadcast(procPool, root)) return false;
			}
		}
		return true;
	}
};

#endif
