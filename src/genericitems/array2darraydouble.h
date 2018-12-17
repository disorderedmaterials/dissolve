/*
	*** Generic Item Container - Array2D< Array<double> >
	*** src/genericitems/array2darraydouble.h
	Copyright T. Youngs 2012-2018

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

#ifndef DISSOLVE_GENERICITEMCONTAINER_ARRAY2DARRAYDOUBLE_H
#define DISSOLVE_GENERICITEMCONTAINER_ARRAY2DARRAYDOUBLE_H

#include "genericitems/container.h"

// GenericItemContainer< Array2D< Array<double> > >
template <> class GenericItemContainer< Array2D< Array<double> > > : public GenericItem
{
	public:
	// Constructor
	GenericItemContainer< Array2D< Array<double> > >(const char* name, int flags = 0) : GenericItem(name, flags)
	{
	}
	// Data item
	Array2D< Array<double> > data;


	/*
	 * Item Class
	 */
	protected:
	// Create a new GenericItem containing same class as current type
	GenericItem* createItem(const char* className, const char* name, int flags = 0)
	{
		if (DissolveSys::sameString(className, itemClassName())) return new GenericItemContainer< Array2D< Array<double> > >(name, flags);
		return NULL;
	}

	public:
	// Return class name contained in item
	const char* itemClassName()
	{
		return "Array2D<Array<double>>";
	}


	/*
	 * I/O
	 */
	public:
	// Write data through specified parser
	bool write(LineParser& parser)
	{
		return write(data, parser);
	}
	// Read data through specified parser
	bool read(LineParser& parser, const CoreData& coreData)
	{
		return read(data, parser);
	}
	// Write specified data through specified parser
	static bool write(const Array2D< Array<double> >& thisData, LineParser& parser)
	{
		parser.writeLineF("%i  %i  %s\n", thisData.nRows(), thisData.nColumns(), DissolveSys::btoa(thisData.halved()));
		for (int n=0; n<thisData.linearArraySize(); ++n)
		{
			const Array<double>& arrayData = thisData.constLinearValue(n);

			parser.writeLineF("%i\n", arrayData.nItems());
			for (int m=0; m<arrayData.nItems(); ++m)
			{
				if (!parser.writeLineF("%16.9e\n", arrayData.constAt(m))) return false;
			}
		}
		return true;
	}
	// Read specified data through specified parser
	static bool read(Array2D< Array<double> >& thisData, LineParser& parser)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		int nRows = parser.argi(0), nColumns = parser.argi(1);
		thisData.initialise(nRows, nColumns, parser.argb(2));

		for (int n=0; n<thisData.linearArraySize(); ++n)
		{
			if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
			int nItems = parser.argi(0);
			thisData.linearArray()[n].createEmpty(nItems);
			for (int m=0; m<nItems; ++m)
			{
				if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
				thisData.linearArray()[n].add(parser.argd(0));
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
#ifdef PARALLEL
		int nRows, nColumns;
		bool half;
		if (root == procPool.poolRank())
		{
			// Broadcast array size first...
			nRows = data.nRows();
			if (!procPool.broadcast(nRows, root))
			{
				Messenger::print("Failed to broadcast Array2D< Array<double> > nRows from root rank %i.\n", root);
				return false;
			}
			nColumns = data.nColumns();
			if (!procPool.broadcast(nColumns, root))
			{
				Messenger::print("Failed to broadcast Array2D< Array<double> > nColmnns from root rank %i.\n", root);
				return false;
			}
			half = data.halved();
			if (!procPool.broadcast(half, root))
			{
				Messenger::print("Failed to broadcast Array2D< Array<double> > half-diagonal status from root rank %i.\n", root);
				return false;
			}
			
			// Now broadcast Array elements
			for (int n=0; n<data.linearArraySize(); ++n)
			{
				if (!procPool.broadcast(data.linearArray()[n], root)) return false;
			}
		}
		else
		{
			// Slaves receive the size, and then create and receive the array
			if (!procPool.broadcast(nRows, root))
			{
				Messenger::print("Slave %i (world rank %i) failed to receive Array2D< Array<double> > nRows from root rank %i.\n", procPool.poolRank(), procPool.worldRank(), root);
				return false;
			}
			if (!procPool.broadcast(nColumns, root))
			{
				Messenger::print("Slave %i (world rank %i) failed to receive Array2D< Array<double> > nRows from root rank %i.\n", procPool.poolRank(), procPool.worldRank(), root);
				return false;
			}
			if (!procPool.broadcast(half, root))
			{
				Messenger::print("Slave %i (world rank %i) failed to receive Array2D< Array<double> > halved status from root rank %i.\n", procPool.poolRank(), procPool.worldRank(), root);
				return false;
			}

			// Resize and receive array
			data.initialise(nRows, nColumns, half);
			for (int n=0; n<data.linearArraySize(); ++n)
			{
				if (!procPool.broadcast(data.linearArray()[n], root)) return false;
			}
		}
#endif
		return true;
	}
	// Return equality between items
	bool equality(ProcessPool& procPool)
	{
#ifdef PARALLEL
		// Verify array size and state first
		if (!procPool.equality(data.nRows())) return Messenger::error("Array2D<double> nRows are not equal (process %i has %i).\n", procPool.poolRank(), data.nRows());
		if (!procPool.equality(data.nColumns())) return Messenger::error("Array2D<double> nColumns are not equal (process %i has %i).\n", procPool.poolRank(), data.nColumns());
		if (!procPool.equality(data.halved())) return Messenger::error("Array2D<double> half-status are not equivalent (process %i has %i).\n", procPool.poolRank(), data.halved());

		// Keep it simple (and slow) and check/send one object at a time
		for (int n=0; n<data.linearArraySize(); ++n) if (!procPool.equality(data.linearArray()[n])) return Messenger::error("Array<double> index %i is not equivalent (process %i.\n", procPool.poolRank());
#endif
		return true;
	}
};

#endif
