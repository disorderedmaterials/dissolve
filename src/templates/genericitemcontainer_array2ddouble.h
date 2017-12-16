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
	}
	// Data item
	Array2D<double> data;


	/*
	 * Item Class
	 */
	protected:
	// Create a new GenericItem containing same class as current type
	GenericItem* createItem(const char* className, const char* name, int flags = 0)
	{
		if (DUQSys::sameString(className, itemClassName())) return new GenericItemContainer< Array2D<double> >(name, flags);
		return NULL;
	}

	public:
	// Return class name contained in item
	const char* itemClassName()
	{
		return "Array2D<double>";
	}


	/*
	 * I/O
	 */
	public:
	// Write data through specified parser
	bool write(LineParser& parser)
	{
		parser.writeLineF("%i  %i  %s\n", data.nRows(), data.nColumns(), DUQSys::btoa(data.halved()));
		for (int n=0; n<data.linearArraySize(); ++n) if (!parser.writeLineF("%16.9e\n", data.linearArray()[n])) return false;
		return true;
	}
	// Read data through specified parser
	bool read(LineParser& parser)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		int nRows = parser.argi(0), nColumns = parser.argi(1);
		data.initialise(nRows, nColumns, parser.argb(2));

		for (int n=0; n<data.linearArraySize(); ++n)
		{
			if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
			data.linearArray()[n] = parser.argd(0);
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
		return procPool.broadcast(data, root);
	}
};

#endif
