/*
	*** Generic Item Container - Array2D<T>
	*** src/templates/genericitemcontainer_array2d.h
	Copyright T. Youngs 2012-2018

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
	GenericItem* createItem(const char* className, const char* name, int flags = 0)
	{
		if (DUQSys::sameString(className, itemClassName())) return new GenericItemContainer< Array2D<T> >(name, flags);
		return NULL;
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
		parser.writeLineF("%i  %i  %s\n", data.nRows(), data.nColumns(), DUQSys::btoa(data.halved()));
		for (int n=0; n<data.linearArraySize(); ++n) if (!data.linearArray()[n].write(parser)) return false;
		return true;
	}
	// Read data through specified parser
	bool read(LineParser& parser)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		int nRows = parser.argi(0), nColumns = parser.argi(1);
		data.initialise(nRows, nColumns, parser.argb(2));

		for (int n=0; n<data.linearArraySize(); ++n) if (!data.linearArray()[n].read(parser)) return false;
		return true;
	}


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast item contents
	bool broadcast(ProcessPool& procPool, int root)
	{
#ifdef PARALLEL
		int nRows, nColumns;
		bool half;
		if (procPool.poolRank() == root)
		{
			// Broadcast array size first...
			nRows = data.nRows();
			if (!procPool.broadcast(nRows, root)) return false;
			nColumns = data.nColumns();
			if (!procPool.broadcast(nColumns, root)) return false;
			half = data.halved();
			if (!procPool.broadcast(half, root)) return false;
			
			// Now broadcast Array data
			if ((nRows*nColumns) > 0)
			{
				for (int n=0; n<data.linearArraySize(); ++n) if (!data.linearArray()[n].broadcast(procPool, root)) return false;
			}
		}
		else
		{
			// Slaves receive the size, and then create and receive the array
			if (!procPool.broadcast(nRows, root)) return false;
			if (!procPool.broadcast(nColumns, root)) return false;
			if (!procPool.broadcast(half, root)) return false;
	
			// Resize and receive array
			data.initialise(nRows, nColumns, half);
			if ((nRows*nColumns) > 0)
			{
				for (int n=0; n<data.linearArraySize(); ++n) if (!data.linearArray()[n].broadcast(procPool, root)) return false;
			}
			else data.clear();
		}
#endif
		return true;
	}
	// Return equality between items
	bool equality(ProcessPool& procPool)
	{
		// Verify array size and state first
		if (!procPool.equality(data.nRows())) return Messenger::error("Array2D<double> nRows are not equal.\n");
		if (!procPool.equality(data.nColumns())) return Messenger::error("Array2D<double> nColumns are not equal.\n");
		if (!procPool.equality(data.halved())) return Messenger::error("Array2D<double> half-status are not equivalent.\n");
		// Keep it simple (and slow) and check/send one value at a time
		for (int n=0; n<data.linearArraySize(); ++n) if (!data.linearArray()[n].equality(procPool)) return false;
		return true;
	}
};

#endif
